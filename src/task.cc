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

#include "com/centreon/cdash/task.hh"
#include "com/centreon/exceptions/basic.hh"

using namespace com::centreon;
using namespace com::centreon::cdash;

/**
 *  Constructor.
 *
 *  @param[in] obj  The object of this task.
 */
task::task(object obj)
  : _obj(std::move(obj)) {
  // Validate the task.
  _validate();
  // Resolve macro from files.
  _resolve_file_macros();
}

/**
 *  Move constructor.
 *
 *  @param[in] tsk  The task to move.
 */
task::task(task&& tsk) noexcept
  : _obj(std::move(tsk._obj)) {}

/**
 *  Move assignment operator.
 *
 *  @param[in] tsk  The task to move.
 *
 *  @return         Reference to this object.
 */
task& task::operator=(task&& tsk) noexcept {
  if (this != &tsk) {
    _obj = std::move(tsk._obj);
  }
  return (*this);
}

/**
 *  Get the ami of this task.
 *
 *  @return  The ami of this task.
 */
std::string task::get_ami() const {
  return (_obj.macro_content("ami"));
}

/**
 *  Get the amazon instance type of this task.
 *
 *  @return  The amazon instance type of this task.
 */
std::string task::get_amazon_instance_type() const {
  return (_obj.macro_content("type"));
}

/**
 *  Get the files to be copied for this task.
 *
 *  @return  Files to be copied for this task.
 */
std::vector<file> const& task::get_files() const noexcept {
  return (_obj.get_files());
}

/**
 *  Get the files to bopied back for this task.
 *
 *  @return  Fiels to be copied back for this task.
 */
std::vector<file> const& task::get_returned_files() const noexcept {
  return (_obj.get_returned_files());
}

/**
 *  Get the command to be executed.
 *
 *  @return  The command.
 */
std::string task::get_command() const {
  return (_obj.macro_content("command"));
}

/**
 *  Get the name of the task.
 *
 *  @return  The name of the task.
 */
std::string task::get_name() const {
  return (_obj.get_name());
}

/**
 *  Get the key name used by this task.
 *
 *  @return  The key name.
 */
std::string task::get_key_name() const {
  return (_obj.macro_content("key"));
}

/**
 *  Get the key filename (optionnal)
 *
 *  @return  The key filename.
 */
std::string task::get_key_file() const {
  return (_obj.macro_content("key_file"));
}

/**
 *  Get the security group used by this task.
 *
 *  @return  The security group.
 */
std::string task::get_security_group() const {
  return (_obj.macro_content("security_group"));
}

/**
 *  Get the security group id used by this task.
 *
 *  @return  The security group id.
 */
std::string task::get_security_group_id() const {
  return (_obj.macro_content("security_group_id"));
}

/**
 *  Get the ssh timeout.
 *
 *  @return  The ssh timeout.
 */
unsigned int task::get_ssh_timeout() const {
  std::string timeout_str = _obj.macro_content("ssh_timeout");
  int timeout = 0;
  try {
   timeout = std::stoi(timeout_str);
  } catch (...) {}
  return (timeout > 0 ? timeout : _default_timeout_value);
}

/**
 *  True if the instance should be deleted at the end of the task.
 *
 *  @return  True if the instance should be deleted at the end of the task.
 */
bool task::should_be_deleted() const noexcept {
  return (_obj.macro_content("should_delete") != "false");
}

/**
 *  Get the user used by ssh.
 *
 *  @return  The user, default "centreon".
 */
std::string task::get_ssh_user() const {
  std::string user = _obj.macro_content("ssh_user");

  return (user.empty() ? "centreon" : std::move(user));
}

/**
 *  Get the port used by ssh.
 *
 *  @return  The port, default 22.
 */
unsigned short task::get_ssh_port() const {
  std::string port_str = _obj.macro_content("ssh_port");
  unsigned short port = 0;
  try {
    port = std::stoi(port_str);
  } catch (...) {}
  return (port > 0 ? port : 22);
}

/**
 *  Get the subnet id.
 *
 *  @return  The subnet id.
 */
std::string task::get_subnet_id() const {
  return (_obj.macro_content("subnet_id"));
}

/**
 *  Validate that the task is well formed.
 */
void task::_validate() const {
  if (!_obj.macro_exists("ami"))
    throw (exceptions::basic()
            << "task: couldn't validate task '"
            << _obj.get_name() << "': macro 'ami'' doesn't exist");
  if (!_obj.macro_exists("command"))
    throw (exceptions::basic()
           << "task: couldn't validate task '"
           << _obj.get_name() << "': macro 'command' doesn't exist");
  if (!_obj.macro_exists("type"))
    throw (exceptions::basic()
      << "task: couldn't validate task '"
      << _obj.get_name() << "': macro 'type' doesn't exist");
  if (!_obj.macro_exists("key"))
    throw (exceptions::basic()
      << "task: couldn't validate task '"
      << _obj.get_name() << "': macro 'key' doesn't exist");
  if (!_obj.macro_exists("security_group") && !_obj.macro_exists("security_group_id"))
    throw (exceptions::basic()
      << "task: couldn't validate task '"
      << _obj.get_name() << "': neither macro 'security_group' "
                            "or 'security_group_id' exist");
}

/**
 *  Resolve the macros of all the files flagged as needed.
 */
void task::_resolve_file_macros() {
  for (auto& file : _obj.get_files_mut()) {
    if (file.resolve_macro()) {
      std::string content = file.get_file_content();
      content = _obj.resolve_macros(content);
      file.set_resolved_file_content(content);
    }
  }
}
