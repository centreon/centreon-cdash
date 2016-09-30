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

#include <unistd.h>
#include <utility>
#include "com/centreon/cdash/task_manager.hh"
#include "com/centreon/exceptions/basic.hh"
#include "com/centreon/aws/ec2/command.hh"
#include "com/centreon/cdash/log/log.hh"
#include "com/centreon/cdash/log/error.hh"

using namespace com::centreon;
using namespace com::centreon::cdash;

volatile bool task_manager::should_exit = false;

/**
 *  Constructor.
 *
 *  @param[in] profile  The profile.
 */
task_manager::task_manager(
                std::string profile)
  : _profile(std::move(profile)) {
}

/**
 *  Move constructor.
 *
 *  @param[in] tsk  The task manager to move.
 */
task_manager::task_manager(task_manager&& tsk) noexcept
  : _profile(std::move(tsk._profile)) {}

/**
 *  Move assignment operator.
 *
 *  @param[in] tsk  The task manager to move.
 *
 *  @return         Reference to this object.
 */
task_manager& task_manager::operator=(task_manager&& tsk) noexcept {
  if (this != &tsk) {
    _profile = std::move(tsk._profile);
  }
  return (*this);
}

/**
 *  Destructor.
 */
task_manager::~task_manager() noexcept {
}

/**
 *  Run.
 *
 *  @param[in] sequences  The sequences of tasks.
 */
void task_manager::run(std::vector<sequence> sequences) {
  _create_spot_instances(sequences);
  while (!should_exit) {
    if (_task_processes.empty()) {
      LOG()
        << "all task processes terminated";
      return ;
    }
    _poll_spot_instances();
    _reap_finished_tasks();
    ::sleep(_polling_duration);
  }
  _poll_spot_instances();
}

/**
 *  Reap the finished tasks.
 */
void task_manager::_reap_finished_tasks() {
  for (auto it = _task_processes.begin(),
       tmp = it,
       end = _task_processes.end();
       it != end;
       it = tmp) {
    ++tmp;
    if (it->second->is_finished()
        || it->second->is_in_fatal_error())
      _task_processes.erase(it);
  }
}

/**
 *  Create the requested spot instances.
 *
 *  @param[in] tasks  The requested spot instances.
 */
void task_manager::_create_spot_instances(
                     std::vector<sequence>& sequences) {
  aws::ec2::command cmd(_profile);
  timestamp valid_until = timestamp::now();
  valid_until.add_seconds(_validity_time_duration);

  for (auto& sequence : sequences) {
    task const& tsk = sequence.get_current_task();
    aws::ec2::launch_specification spec;
    spec.set_image_id(tsk.get_ami());
    spec.set_instance_type(tsk.get_amazon_instance_type());
    spec.set_key_name(tsk.get_key_name());
    if (!tsk.get_security_group().empty()) {
      aws::ec2::security_group sec;
      sec.set_group_name(tsk.get_security_group());
      spec.add_security_groups(sec);
    }
    if (!tsk.get_security_group_id().empty()) {
      aws::ec2::security_group sec;
      sec.set_group_id(tsk.get_security_group_id());
      spec.add_security_group_ids(sec);
    }
    spec.set_subnet_id(tsk.get_subnet_id());
    LOG()
      << "requesting spot instance for task '"
      << tsk.get_name() << "'";
    auto const& instances = cmd.request_spot_instance(
                                  _price,
                                  1,
                                  "persistent",
                                  timestamp(),
                                  valid_until,
                                  spec);
    LOG()
      << "got spot instance '"
      << instances.back().get_spot_instance_request_id()
      << "' for task '" << tsk.get_name() << "'";
    _spot_instances.insert(
      _spot_instances.end(),
      instances.begin(),
      instances.end());
    std::unique_ptr<task_process> process(
      new task_process(
            _profile,
            std::move(sequence),
            _spot_instances.back()));
    // XXX: No emplace because GCC 4.7.
    _task_processes.insert(
      std::make_pair(
        _spot_instances.back().get_spot_instance_request_id(),
        std::move(process)));
  }
}

/**
 *  Poll the spot instances from aws.
 */
void task_manager::_poll_spot_instances() {
  aws::ec2::command cmd(_profile);

  _spot_instances = cmd.get_spot_instances();
  LOG()
    << "got " << _spot_instances.size() << " spot instances from amazon";

  for (auto const& spot_instance : _spot_instances) {
    auto found = _task_processes.find(
                   spot_instance.get_spot_instance_request_id());
    if (found != _task_processes.end()) {
      found->second->visit(spot_instance);
      if (spot_instance.get_state() == aws::ec2::spot_instance::active)
        found->second->visit(
          cmd.get_instance_from_id(spot_instance.get_instance_id()));
    }
  }
}

