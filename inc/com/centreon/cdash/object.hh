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

#ifndef CCC_OBJECT_HH
#  define CCC_OBJECT_HH

#  include <string>
#  include <map>
#  include <vector>
#  include "com/centreon/cdash/file.hh"
#  include "com/centreon/cdash/namespace.hh"

CCC_BEGIN()

class             object {
  public:
                  object(std::string type);
                  object(object const& obj);
    object&       operator=(object const& obj);
                  object(object&& obj) noexcept;
    object&       operator=(object&& obj) noexcept;
                  ~object() noexcept;

    void          set_name(std::string name);
    std::string const&
                  get_name() const noexcept;
    std::string const&
                  get_type() const noexcept;
    bool          macro_exists(std::string const& name) const noexcept;
    std::string   macro_content(std::string const& name) const noexcept;
    void          set_macro(std::string name, std::string content);
    void          inherit_macros(object const& obj);
    std::string   resolve_macros(std::string str) const;

    void          add_file(
                    std::string local_filename,
                    std::string remote_filename,
                    bool resolve_macro);
    void          add_returned_file(
                    std::string local_filename,
                    std::string remote_filename);
    std::vector<file> const&
                  get_files() const noexcept;
    std::vector<file>&
                  get_files_mut() noexcept;
    std::vector<file> const&
                  get_returned_files() const noexcept;

  private:
    std::string  _name;
    std::string  _type;
    std::map<std::string, std::string>
                 _macros;
    std::vector<file>
                 _files;
    std::vector<file>
                 _returned_files;
};

CCC_END()

#endif // !CCC_OBJECT_HH
