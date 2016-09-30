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

#ifndef CCC_TASK_PROCESS_HH
#  define CCC_TASK_PROCESS_HH

#  include <string>
#  include "com/centreon/concurrency/mutex.hh"
#  include "com/centreon/cdash/task.hh"
#  include "com/centreon/cdash/sequence.hh"
#  include "com/centreon/aws/ec2/instance.hh"
#  include "com/centreon/aws/ec2/spot_instance.hh"
#  include "com/centreon/cdash/namespace.hh"
#  include "com/centreon/cdash/ssh_wrapper.hh"
#  include "com/centreon/process.hh"
#  include "com/centreon/process_listener.hh"

CCC_BEGIN()

class             task_process : public process_listener {
  public:
                  task_process(
                    std::string profile,
                    sequence seq,
                    aws::ec2::spot_instance const& spot_instance);
                  ~task_process() noexcept;

    aws::ec2::spot_instance const&
                  get_spot_instance() const noexcept;
    aws::ec2::instance const&
                  get_instance() const noexcept;

    void          visit(aws::ec2::spot_instance const& spot_instance);
    void          visit(aws::ec2::instance const& instance);
    bool          is_finished();
    bool          is_in_fatal_error();

    virtual void  data_is_available(process& p) noexcept;
    virtual void  data_is_available_err(process& p) noexcept;
    virtual void  finished(process& p) noexcept;

  private:
    concurrency::mutex
                  _mut;

    std::string   _profile;
    sequence      _sequence;
    aws::ec2::spot_instance const*
                  _spot_instance;
    aws::ec2::instance
                  _instance;

    std::vector<file>
                   _files_to_copy;
    std::vector<file>
                   _files_to_copy_back;
    std::string    _command;

    std::string   _out;
    std::string   _err_out;

    // The state of this state machine.
    // When everything is okay, it goes like this:
    // waiting_for_spot_instance -> waiting_for_instance -> (copying_files -> running -> copying_files_back) * tasks -> ended
    // Unrepairable errors are signaled by the 'error' state.
    enum          state {
                  waiting_for_spot_instance,
                  waiting_for_instance,
                  copying_files,
                  running,
                  copying_files_back,
                  ended,
                  error
    };
    state         _state;

    //ssh_wrapper   _ssh;
    process       _process;

    void          _clear();
    void          _run();
    void          _start_next_task();
    void          _terminate_associated_instance();
    std::string const&
                  _get_ip() const noexcept;

                  task_process() = delete;
                  task_process(task_process const&) = delete;
    task_process& operator=(task_process const&) = delete;
};

CCC_END()

#endif // !CCC_TASK_PROCESS_HH
