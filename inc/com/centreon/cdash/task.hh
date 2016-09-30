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

#ifndef CCC_TASK_HH
#  define CCC_TASK_HH

#  include <string>
#  include "com/centreon/cdash/object.hh"
#  include "com/centreon/cdash/namespace.hh"

CCC_BEGIN()

class             task {
  public:
                  task(object obj);
                  task(task&& tsk) noexcept;
    task&         operator=(task&& tsk) noexcept;

    std::string   get_ami() const;
    std::string   get_amazon_instance_type() const;
    std::vector<file> const&
                  get_files() const noexcept;
    std::vector<file> const&
                  get_returned_files() const noexcept;
    std::string   get_command() const;
    std::string   get_name() const;
    std::string   get_key_name() const;
    std::string   get_security_group() const;
    std::string   get_security_group_id() const;
    std::string   get_key_file() const;
    unsigned int  get_ssh_timeout() const;
    std::string   get_ssh_user() const;
    unsigned short get_ssh_port() const;
    bool          should_be_deleted() const noexcept;
    std::string   get_subnet_id() const;

  private:
    object        _obj;

    void          _validate() const;
    void          _resolve_file_macros();

    static constexpr unsigned int
                  _default_timeout_value = 5 * 60;

                  task() = delete;
                  task(task const&) = delete;
    task&         operator=(task const&) = delete;
};

CCC_END()

#endif // !CCC_TASK_HH
