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

#include <iostream>
#include "com/centreon/cdash/log/log.hh"
#include "com/centreon/cdash/log/engine.hh"

using namespace com::centreon::cdash::log;

/**
 *  Default constructor.
 *
 *  @param[in] name  The name of the log to use.
 */
log::log(std::string const& name)
   : _name(name) {

}

/**
 *  Destructor.
 */
log::~log() {
  engine::log(_name, _buffer.data());
}
