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

#include "com/centreon/exceptions/basic.hh"
#include "com/centreon/cdash/file_parser.hh"

using namespace com::centreon;
using namespace com::centreon::cdash;

/**
 *  Constructor.
 *
 *  @param[in] filename  The filename.
 */
file_parser::file_parser(std::string const& filename)
  : _file(filename) {
  if (!_file.is_regular())
    throw (exceptions::basic()
           << "file_parser: '" << filename << "' is not a regular file");
}

/**
 *  Destructor.
 */
file_parser::~file_parser() noexcept {

}

/**
 *  Parse the file.
 *
 *  @return  A tree parsed.
 */
xml::tree file_parser::parse() const {
  xml::tree ret = xml::tree(
    xmlReadFile(_file.path().c_str(), nullptr, XML_PARSE_NOBLANKS));

  if (ret.null())
    throw (exceptions::basic()
           << "file_parser: couldn't parse '"
           << _file.file_name()
           << "' : invalid xml format");

  return (std::move(ret));
}
