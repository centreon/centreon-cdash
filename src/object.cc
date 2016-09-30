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

#include "com/centreon/cdash/object.hh"
#include "com/centreon/exceptions/basic.hh"

using namespace com::centreon;
using namespace com::centreon::cdash;

/**
 *  Constructor.
 *
 *  @param[in] type  The type of this object.
 */
object::object(std::string type)
  : _type(std::move(type)) {}

/**
 *  Copy constructor.
 *
 *  @param[in] obj  The object to copy.
 */
object::object(object const& obj)
  : _name(obj._name),
    _type(obj._type),
    _macros(obj._macros),
    _files(obj._files),
    _returned_files(obj._returned_files) {}

/**
 *  Assignment operator.
 *
 *  @param[in] obj  The object to copy.
 *
 *  @return         A reference to this object.
 */
object& object::operator=(object const& obj) {
  if (this != &obj) {
    _name = obj._name;
    _type = obj._type;
    _macros = obj._macros;
    _files = obj._files;
    _returned_files = obj._returned_files;
  }
  return (*this);
}

/**
 *  Move constructor.
 *
 *  @param[in] obj  The object to move.
 */
object::object(object&& obj) noexcept
  : _name(std::move(obj._name)),
    _type(std::move(obj._type)),
    _macros(std::move(obj._macros)),
    _files(std::move(obj._files)),
    _returned_files(std::move(obj._returned_files)) {}

/**
 *  Move assignment operator.
 *
 *  @param[in] obj  The object to move.
 *
 *  @return         A reference to this object.
 */
object& object::operator=(object&& obj) noexcept {
  if (this != &obj) {
    _name = std::move(obj._name);
    _type = std::move(obj._type);
    _macros = std::move(obj._macros);
    _files = std::move(obj._files);
    _returned_files = std::move(obj._returned_files);
  }
  return (*this);
}

/**
 *  Destructor.
 */
object::~object() noexcept {

}

/**
 *  Set the name of this object.
 *
 *  @param[in] name
 */
void object::set_name(std::string name) {
  _name = std::move(name);
  set_macro("name", _name);
}

/**
 *  Get the name of this object.
 *
 *  @return  Name of the object.
 */
std::string const& object::get_name() const noexcept {
  return (_name);
}

/**
 *  Get the type of this object.
 *
 *  @return  Type of this object.
 */
std::string const& object::get_type() const noexcept {
  return (_type);
}

/**
 *  Check for macro existence.
 *
 *  @param[in] name  Name of the macro.
 *
 *  @return          True if the macro exists.
 */
bool object::macro_exists(std::string const& name) const noexcept {
  return (_macros.find(name) != _macros.end());
}

/**
 *  Get the content of a macro.
 *
 *  @param[in] name  Name of the macro.
 *
 *  @return          The content of the macro, or an empty string.
 */
std::string object::macro_content(std::string const& name) const noexcept {
  std::map<std::string, std::string>::const_iterator found
    = _macros.find(name);
   return (found != _macros.end() ? found->second : std::string());
}

/**
 *  Set a macro.
 *
 *  @param[in] name     The name of the macro.
 *  @param[in] content  The content of the macro.
 */
void object::set_macro(std::string name, std::string content) {
  _macros[std::move(name)] = std::move(content);
}

/**
 *  Inherit all the macros and files of another object.
 *
 *  @param[in] obj  The other object.
 */
void object::inherit_macros(object const& obj) {
  for (auto const& macro : obj._macros) {
    set_macro(macro.first, macro.second);
    set_macro(std::string(obj.get_type()) + "." + macro.first, macro.second);
  }
  for (auto const& file : obj._files)
    add_file(
      file.get_local_filename(),
      file.get_remote_filename(),
      file.resolve_macro());
}

/**
 *  Resolve the macros in a string.
 *
 *  @param[in] str  The string.
 *
 *  @return         The string with macros resolved.
 */
std::string object::resolve_macros(std::string str) const {
  size_t index = 0;
  for (size_t first_of = str.find_first_of('$', index);
       first_of != std::string::npos;
       first_of = str.find_first_of('$', index)) {
    size_t second_of = str.find_first_of('$', first_of + 1);
    if (second_of == std::string::npos)
      throw (exceptions::basic()
             << "couldn't find closing '$' in '" << str << "'");
    second_of -= first_of;
    std::string substr = str.substr(first_of + 1, second_of - 1);
    std::string replaced = substr.empty() ? "$" : macro_content(substr);
    str.replace(first_of, second_of + 1, replaced);
    index = first_of + 1;
  }
  return (std::move(str));
}

/**
 *  Add a file that should be copied.
 *
 *  @param[in] local_filename   The local filename of the file to be copied.
 *  @param[in] remote_filename  The remote filename of the file to be copied.
 *  @param[in] resolve_macro    Should the file's macro be resolved?
 */
void object::add_file(
               std::string local_filename,
               std::string remote_filename,
               bool resolve_macro) {
  _files.emplace_back(
           std::move(local_filename),
           std::move(remote_filename),
           resolve_macro);
}

/**
 *  Add a file that should be copied back from the server.
 *
 *  @param[in] local_filename   The local filename of the file.
 *  @param[in] remote_filename  The remote filename of the file.
 */
void object::add_returned_file(
               std::string local_filename,
               std::string remote_filename) {
  _returned_files.emplace_back(
                    std::move(local_filename),
                    std::move(remote_filename),
                    false);
}

/**
 *  Get the files that should be copied.
 *
 *  @return  The files that should be copied.
 */
std::vector<file> const& object::get_files() const noexcept {
  return (_files);
}

/**
 *  Get the files that should be copied.
 *
 *  @return  The files that should be copied.
 */
std::vector<file>& object::get_files_mut() noexcept {
  return (_files);
}

/**
 *  Get the files that should be copied back.
 *
 *  @return  The files that should be copied back.
 */
std::vector<file> const& object::get_returned_files() const noexcept {
  return (_returned_files);
}
