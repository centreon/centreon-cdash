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

#ifndef CCC_SEQUENCE_HH
#  define CCC_SEQUENCE_HH

#  include <vector>
#  include <string>
#  include "com/centreon/cdash/task.hh"
#  include "com/centreon/cdash/object.hh"
#  include "com/centreon/cdash/namespace.hh"

CCC_BEGIN()

class             sequence {
  public:
                  sequence();
                  sequence(sequence&& seq) noexcept;
    sequence&     operator=(sequence&& seq) noexcept;

    task const&   get_current_task() const;
    bool          next_task();
    bool          ended() const noexcept;
    void          reset() noexcept;

    void          add_task(task tsk);

  private:
    std::vector<task>
                 _tasks;
    unsigned int _task_index;

                  sequence(sequence const&) = delete;
    sequence&     operator=(sequence const&) = delete;
};

CCC_END()

#endif // !CCC_SEQUENCE_HH
