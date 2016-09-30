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

#include "com/centreon/cdash/args_parser.hh"

using namespace com::centreon::cdash;

/**
 *  Args parser constructor.
 */
args_parser::args_parser() {
  misc::argument help;

  help.set_description("display help");
  help.set_long_name("help");
  help.set_name('h');
  _arguments['h'] = help;
}

/**
 *  Destructor.
 */
args_parser::~args_parser() noexcept {

}

/**
 *  Parse the arguments.
 *
 *  @param[in] argc  The number of arguments.
 *  @param[in] argv  The arguments.
 */
void args_parser::parse(int argc, char **argv) {
  _parse_arguments(argc, argv);
}

/**
 *  Get the help string.
 *
 *  @return  The help string.
 */
std::string args_parser::help() const {
  return (
    std::string("centreon_cdash\t[args] file1.xml [file2.xml[file3.xml...]]\n"
                " args:\n")
    + get_options::help());
}
