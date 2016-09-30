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

#ifndef CCC_TASK_MANAGER_HH
#  define CCC_TASK_MANAGER_HH

#  include <memory>
#  include <vector>
#  include <map>
#  include <string>
#  include "com/centreon/cdash/task.hh"
#  include "com/centreon/cdash/sequence.hh"
#  include "com/centreon/cdash/namespace.hh"
#  include "com/centreon/aws/ec2/spot_instance.hh"
#  include "com/centreon/cdash/task_process.hh"

CCC_BEGIN()

class             task_manager {
  public:
                  task_manager(
                    std::string profile);
                  task_manager(task_manager&& tsk) noexcept;
    task_manager& operator=(task_manager&& tsk) noexcept;
                  ~task_manager() noexcept;

    void          run(std::vector<sequence> sequences);

    // Used to manage signal termination.
    static volatile bool
                  should_exit;

  private:
    std::string   _profile;

    static const unsigned int
                  _validity_time_duration = 3600 * 24;
    static const unsigned int
                  _polling_duration = 15;
    static constexpr double
                  _price = 0.3;

    std::vector<aws::ec2::spot_instance>
                  _spot_instances;
    std::map<std::string, std::unique_ptr<task_process>>
                  _task_processes;

    void          _reap_finished_tasks();
    void          _create_spot_instances(
                    std::vector<sequence>& sequences);
    void          _poll_spot_instances();

                  task_manager() = delete;
                  task_manager(task_manager const&) = delete;
    task_manager& operator=(task_manager const&) = delete;
};

CCC_END()

#endif // !CCC_TASK_MANAGER_HH
