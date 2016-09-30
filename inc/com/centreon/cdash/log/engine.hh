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

#ifndef CCC_LOG_ENGINE_HH
#  define CCC_LOG_ENGINE_HH

#  include <string>
#  include <vector>
#  include <map>
#  include <memory>
#  include <fstream>
#  include "com/centreon/cdash/namespace.hh"

CCC_BEGIN()

namespace log {

class             engine {
public:
  static void     log(
                    std::string const& name,
                    std::string const& content);

private:
  static engine*  _p_engine;
  static constexpr char const*
                  _default_file_name = "cdash.log";

  std::map<std::string, std::unique_ptr<std::ofstream>>
                  _log_files;

                  engine() noexcept;
                  ~engine() noexcept;
                  engine(engine const&) = delete;
 engine&          operator=(engine const&) = delete;

 void             _log(
                     std::string const& name,
                     std::string const& content);
};

} //namespace log

CCC_END()

#endif // !CCC_LOG_ENGINE_HH
