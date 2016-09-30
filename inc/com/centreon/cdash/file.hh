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

#ifndef CCC_FILE_HH
#  define CCC_FILE_HH

#  include <string>
#  include "com/centreon/cdash/namespace.hh"
#  include "com/centreon/io/file_entry.hh"

CCC_BEGIN()

class                   file {
  public:
                        file(
                         std::string local_filename,
                         std::string remote_filename,
                         bool resolve_macro);
                        file();
                        file(file const& other);
                        file(file&& other) noexcept;
    file&               operator=(file const& other);
    file&               operator=(file&& other) noexcept;
                        ~file() noexcept;

    std::string const&  get_local_filename() const noexcept;
    std::string const&  get_remote_filename() const noexcept;
    bool                resolve_macro() const noexcept;
    std::string         get_file_content() const;
    void                set_resolved_file_content(std::string content);
    std::string const&  get_temporary_file() const noexcept;

  private:
    std::string         _local_filename;
    std::string         _remote_filename;
    bool                _resolve_macro;
    std::string         _temporary_file;
};

CCC_END()

#endif // !CCC_FILE_PARSER_HH
