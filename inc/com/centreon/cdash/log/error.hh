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

#ifndef CCC_LOG_ERROR_HH
#  define CCC_LOG_ERROR_HH

#  include <string>
#  include <vector>
#  include <map>
#  include <memory>
#  include <fstream>
#  include "com/centreon/misc/stringifier.hh"
#  include "com/centreon/cdash/namespace.hh"
#  include "com/centreon/cdash/log/log.hh"

#  define ERROR(a) (::com::centreon::cdash::log::error(a) << "'" << __PRETTY_FUNCTION__ << "' : ")

CCC_BEGIN()

namespace log {

class             error : public log {
public:
                  error(std::string const& name = std::string());
                  ~error();

  template <typename T>
  error&            operator<<(T const& t) {
    log::operator<<(t);
    return (*this);
  }

private:
};

} //namespace log

CCC_END()

#endif // !CCC_LOG_ERROR_HH
