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
#include "com/centreon/cdash/log/engine.hh"

using namespace com::centreon::cdash::log;

engine* engine::_p_engine = nullptr;

/**
 *  Log something.
 *
 *  @param[in] name     The name of the log used for the filename.
 *  @param[in] content  The content of the log.
 */
void engine::log(
               std::string const& name,
               std::string const& content) {
  if (!_p_engine)
    _p_engine = new engine;

  _p_engine->_log(name, content);
}

/**
 *  Default constructor.
 */
engine::engine() noexcept {

}

/**
 *  Destructor
 */
engine::~engine() noexcept {

}

/**
 *  Log something.
 *
 *  @param[in] name     The name of the log used for the filename.
 *  @param[in] content  The content of the log.
 */
void engine::_log(
               std::string const& name,
               std::string const& content) {
  std::string file_name = name.empty()
              ? (std::string(_default_file_name))
              : (name + ".log");

   std::unique_ptr<std::ofstream>& ptr = _log_files[file_name];
   if (!ptr.get()) {
     try {
      ptr.reset(new std::ofstream);
      ptr->exceptions(std::ofstream::failbit | std::ofstream::badbit);
      ptr->open(file_name, std::ofstream::out | std::ofstream::trunc);
     } catch (std::exception const& e) {
      std::cerr
        << "cannot open log file '" << file_name
        << "': " << e.what() << std::endl;
      return ;
     }
   }

   try {
    ptr->write(content.c_str(), content.size());
    *ptr << "\n";
    ptr->flush();
   } catch (std::exception const& e) {
     std::cerr
       << "cannot write into log file '" << file_name
       << "': " << e.what() << std::endl;
     return ;
   }
}
