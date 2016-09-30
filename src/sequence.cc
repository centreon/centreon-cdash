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

#include "com/centreon/cdash/sequence.hh"
#include "com/centreon/exceptions/basic.hh"

using namespace com::centreon;
using namespace com::centreon::cdash;

/**
 *  Default constructor.
 */
sequence::sequence()
  : _task_index(0) {}

/**
 *  Move constructor.
 *
 *  @param[in] tsk  The object to move from.
 */
sequence::sequence(sequence&& seq) noexcept
  : _tasks(std::move(seq._tasks)),
    _task_index(std::move(seq._task_index)) {}

/**
 *  Move assignment operator.
 *
 *  @param[in] seq  The object to move from.
 *
 *  @return         Reference to this object.
 */
sequence& sequence::operator=(sequence&& seq) noexcept {
  if (this != &seq) {
    _tasks = std::move(seq._tasks);
    _task_index = std::move(seq._task_index);
  }
  return (*this);
}

/**
 *  Get the current task.
 *
 *  @return  The current task.
 */
task const& sequence::get_current_task() const {
  return (_tasks.at(_task_index));
}

/**
 *  Move the sequence to the next task.
 *
 *  @return  True if the next task exists.
 */
bool sequence::next_task() {
  ++_task_index;
  return (!ended());
}

/**
 *  Has the sequence ended?
 *
 *  @return  True or false.
 */
bool sequence::ended() const noexcept {
  return (_task_index >= _tasks.size());
}

/**
 *  Reset the sequence to its first task.
 */
void sequence::reset() noexcept {
  _task_index = 0;
}

/**
 *  Add a task to the sequence.
 *
 *  @param[in] tsk  The task to add.
 */
void sequence::add_task(task tsk) {
  _tasks.emplace_back(std::move(tsk));
}
