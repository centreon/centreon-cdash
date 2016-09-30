/*
** Copyright 2015-2016 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#include "com/centreon/concurrency/locker.hh"
#include "com/centreon/cdash/ssh_wrapper.hh"
#include "com/centreon/cdash/task_process.hh"
#include "com/centreon/exceptions/basic.hh"
#include "com/centreon/cdash/log/log.hh"
#include "com/centreon/cdash/log/error.hh"
#include "com/centreon/aws/ec2/command.hh"

using namespace com::centreon;
using namespace com::centreon::cdash;
using namespace com::centreon::aws::ec2;

/**
 *  Default constructor.
 *
 *  @param[in] profile        The profile associated with this task process.
 *  @param[in] seq            The sequence of tasks associated with this task process.
 *  @param[in] spi            The spot instance associated with this task process.
 */
task_process::task_process(
                std::string profile,
                sequence seq,
                aws::ec2::spot_instance const& spi)
  : _profile(std::move(profile)),
    _sequence(std::move(seq)),
    _spot_instance(&spi),
    _state(waiting_for_spot_instance),
    _process(this) {
  LOG(_sequence.get_current_task().get_name())
    << "creating task process bound to the spot instance '"
    << _spot_instance->get_spot_instance_request_id() << "'"
       ": waiting for spot instance activation...";
  _clear();
  visit(spi);
}

/**
 *  Destructor.
 */
task_process::~task_process() noexcept {
  concurrency::locker lock(&_mut);
  if (_state == running || _state == copying_files) {
    _state = ended;
    try {
      lock.unlock();
      _process.terminate();
      _process.wait();
    } catch (std::exception const& e) {
      ERROR(_sequence.ended() ?
              "" :
              _sequence.get_current_task().get_name())
            << "error while waiting: " << e.what();
    }
  }
  lock.relock();
  _terminate_associated_instance();
}

/**
 *  Get the spot instance associated with this task process.
 *
 *  @return  The spot instance associated with this task process.
 */
aws::ec2::spot_instance const& task_process::get_spot_instance() const noexcept {
  return (*_spot_instance);
}

/**
 *  Get the instance associated with this task process.
 *
 *  @return  The instance associated with this task process.
 */
aws::ec2::instance const& task_process::get_instance() const noexcept {
  return (_instance);
}

/**
 *  Update the process with spot instance data.
 *
 *  Task process use a simple state machine to modelize the various statuses
 *  of a task.
 *
 *  @param[in] spi  The spot instance.
 */
void task_process::visit(aws::ec2::spot_instance const& spi) {
  concurrency::locker lock(&_mut);
  _spot_instance = &spi;
  spot_instance::spot_instance_state spot_state = spi.get_state();

  if (spot_state == spot_instance::failed
      || spot_state == spot_instance::canceled
      || spot_state == spot_instance::closed) {
    ERROR(_sequence.get_current_task().get_name())
      << "spot instance failed";
    lock.unlock();
    _process.terminate();
    _process.wait();
    lock.relock();
    _state = error;
  }
  else if (_state == waiting_for_spot_instance
           && spot_state == spot_instance::active) {
    LOG(_sequence.get_current_task().get_name())
      << "spot instance active, waiting for instance...";
    _state = waiting_for_instance;
  }
  else if ((_state == running || _state == copying_files)
           && spot_state == spot_instance::closed) {
    ERROR(_sequence.get_current_task().get_name())
      << "spot instance was closed while the task is running,"
         " resetting sequence of tasks and waiting for a retry...";
    _state = waiting_for_spot_instance;
    lock.unlock();
    _process.terminate();
    _process.wait();
    lock.relock();
    _sequence.reset();
    _clear();
  }
}

/**
 *  Update the process instance data.
 *
 *  Task process use a simple state machine to modelize the various statuses
 *  of a task.
 *
 *  @param[in] ins  The instance.
 */
void task_process::visit(aws::ec2::instance const& ins) {
  concurrency::locker _(&_mut);
  _instance = ins;
  instance::instance_status ins_status = ins.get_instance_status();
  if (_state == waiting_for_instance
      && ins_status == instance::running) {
    LOG(_sequence.get_current_task().get_name())
      << "new instance '" << _instance.get_instance_id()
      << "' found (IP: " << _get_ip()
      << "), starting task...";
   _run();
  }
}

/**
 *  Is this task finished?
 *
 *  @return  True or false.
 */
bool task_process::is_finished() {
  concurrency::locker _(&_mut);
  return (_state == ended);
}

/**
 *  Is this task in a fatal error ?
 *
 *  @return  True or false.
 */
bool task_process::is_in_fatal_error() {
  concurrency::locker _(&_mut);
  return (_state == error);
}

/**
 *  Data is available callback.
 *
 *  @param[in] p  The process.
 */
void task_process::data_is_available(process& p) noexcept {
  concurrency::locker _(&_mut);
  std::string data;
  p.read(data);
  _out.append(data);
}

/**
 *  Data is available err callback.
 *
 *  @param[in] p  The process.
 */
void task_process::data_is_available_err(process& p) noexcept {
  concurrency::locker _(&_mut);
  std::string data;
  p.read_err(data);
  _err_out.append(data);
}

/**
 *  Finished callback.
 *
 *  @param[in] p  The process that was finished.
 */
void task_process::finished(process& p) noexcept {
  concurrency::locker _(&_mut);
  if (p.exit_code() != 0 || p.exit_status() != process::normal) {
    ERROR(_sequence.get_current_task().get_name())
      << "error in process execution: '" << _err_out << "'";
    if (_state != ended)
      _run();
  }
  else if (_state == copying_files) {
    LOG(_sequence.get_current_task().get_name())
      << "copy finished";
    _run();
  }
  else if (_state == running) {
    LOG(_sequence.get_current_task().get_name())
      << "command finished, output = \n"
      << _out;
    _run();
  }
  else if (_state == copying_files_back) {
    LOG(_sequence.get_current_task().get_name())
      << "copy finished";
    _run();
  }
}

/**
 *  Clear the task process.
 */
void task_process::_clear() {
  _files_to_copy = _sequence.get_current_task().get_files();
  _files_to_copy_back = _sequence.get_current_task().get_returned_files();
  _out.clear();
  _err_out.clear();
}

/**
 *  Normal running of the task.
 */
void task_process::_run() {
  task const& current_task = _sequence.get_current_task();
  ssh_wrapper wrapper(
                _get_ip(),
                current_task.get_ssh_port(),
                current_task.get_ssh_user());

  _out.clear();
  _err_out.clear();

  if (_files_to_copy.size() != 0) {
    _state = copying_files;
    file fl = _files_to_copy.back();
    _files_to_copy.pop_back();
    LOG(current_task.get_name())
      << "copying local file '" << fl.get_local_filename()
      << "' to remote file '" << fl.get_remote_filename() << "'";
    wrapper.copy_file(
              _process,
              fl.resolve_macro() ? fl.get_temporary_file() :
                                   fl.get_local_filename(),
              fl.get_remote_filename(),
              current_task.get_key_file(),
              current_task.get_ssh_timeout());
  }
  else if (_state == copying_files || _state == waiting_for_instance) {
    _state = running;
    LOG(current_task.get_name())
      << "executing command '"
      << current_task.get_command() << "'";
    wrapper.execute(
              _process,
              current_task.get_command(),
              current_task.get_key_file(),
              current_task.get_ssh_timeout());
  }
  else if (_files_to_copy_back.size() != 0) {
    _state = copying_files_back;
    file fl = _files_to_copy_back.back();
    _files_to_copy_back.pop_back();
    LOG(current_task.get_name())
      << "copying back remote file '" << fl.get_remote_filename()
      << "' to local file '" << fl.get_local_filename() << "'";
    wrapper.copy_file_back(
              _process,
              fl.get_local_filename(),
              fl.get_remote_filename(),
              current_task.get_key_file(),
              current_task.get_ssh_timeout());
  }
  else
    _start_next_task();
}

/**
 *  Start the next task.
 */
void task_process::_start_next_task() {
  // Go to next task or end.
  if (!_sequence.next_task())
    _state = ended;
  else {
    LOG(_sequence.get_current_task().get_name())
      << "starting new task '" << _sequence.get_current_task().get_name()
      << "' in sequence for instance '" << _instance.get_instance_id() << "'";
    _clear();
    _run();
  }
}

/**
 *  Terminate the associated instances of a task.
 */
void task_process::_terminate_associated_instance() {
  _sequence.reset();
  if (_sequence.get_current_task().should_be_deleted()) {
    try {
      LOG()
        << "terminating spot instances '"
        << _spot_instance->get_spot_instance_request_id()
        << "' from amazon...";
      aws::ec2::command cmd(_profile);
      cmd.cancel_spot_instance_request(
            _spot_instance->get_spot_instance_request_id());
      if (!_instance.get_instance_id().empty())
        cmd.terminate_instance(_instance.get_instance_id());
    } catch (std::exception const& e) {
      ERROR()
         << "couldn't terminate spot instances '"
         << _spot_instance->get_spot_instance_request_id()
         << "'' from amazon: " << e.what();
    }
  }
}

/**
 *  Get a usable ip of the actual instance.
 *
 *  @return  The ip of the actual instance to use.
 */
std::string const& task_process::_get_ip() const noexcept {
  if (!_instance.get_public_ip_address().empty())
    return (_instance.get_public_ip_address());
  else
    return (_instance.get_private_ip_address());
}
