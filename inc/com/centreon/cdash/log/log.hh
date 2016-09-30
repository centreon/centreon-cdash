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

#ifndef CCC_LOG_LOG_HH
#  define CCC_LOG_LOG_HH

#  include <string>
#  include <vector>
#  include <map>
#  include <memory>
#  include <fstream>
#  include "com/centreon/misc/stringifier.hh"
#  include "com/centreon/cdash/namespace.hh"

#  define LOG(a) (::com::centreon::cdash::log::log(a) << "'" << __PRETTY_FUNCTION__ << "' : ")

CCC_BEGIN()

namespace log {

class             log {
public:
                  log(std::string const& name = std::string());
                  ~log();

  template <typename T>
  log&            operator<<(T const& t) {
    _buffer << t;
    return (*this);
  }

protected:
  std::string     _name;
  misc::stringifier
                  _buffer;
};

} //namespace log

CCC_END()

#endif // !CCC_LOG_LOG_HH
