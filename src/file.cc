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

#include <cstdio>
#include <cstring>
#include <sstream>
#include <fstream>
#include "com/centreon/exceptions/basic.hh"
#include "com/centreon/cdash/file.hh"
#include "com/centreon/io/file_entry.hh"
#include "com/centreon/cdash/log/log.hh"

using namespace com::centreon;
using namespace com::centreon::cdash;

static char const* tmp_file_template = "/tmp/resolved_macro_fileXXXXXX";

/**
 *  Default constructor.
 *
 *  @param[in] local_filename   The local filename.
 *  @param[in] remote_filename  The remote filename.
 *  @param[in] resolve_macro    Should the file's macros be resolved?
 */
file::file(
  std::string local_filename,
  std::string remote_filename,
  bool resolve_macro)
  : _local_filename(std::move(local_filename)),
    _remote_filename(std::move(remote_filename)),
    _resolve_macro(resolve_macro) {
  // Check the we can access this file.
  io::file_entry _(local_filename);
}

/**
 *  Default constructor.
 */
file::file() :
        _resolve_macro(false) {
}

/**
 *  Copy constructor.
 *
 *  @param[in] other  The object to copy.
 */
file::file(file const& other)
  : _local_filename(other._local_filename),
    _remote_filename(other._remote_filename),
    _resolve_macro(other._resolve_macro),
    _temporary_file(other._temporary_file) {
}

/**
 *  Move constructor.
 *
 *  @param[in] other  The object to move.
 */
file::file(file&& other) noexcept
  : _local_filename(other._local_filename),
    _remote_filename(other._remote_filename),
    _resolve_macro(other._resolve_macro),
    _temporary_file(other._temporary_file) {

}

/**
 *  Assignment operator.
 *
 *  @param[in] other  The object to copy.
 *
 *  @return           Reference to this.
 */
file& file::operator=(file const& other) {
  if (this != &other) {
    _local_filename = other._local_filename;
    _remote_filename = other._remote_filename;
    _resolve_macro = other._resolve_macro;
    _temporary_file = other._temporary_file;
  }
  return (*this);
}

/**
 *  Move assignment operator.
 *
 *  @param[in] other  The object to move.
 *
 *  @return           Reference to this.
 */
file& file::operator=(file&& other) noexcept {
  if (this != &other) {
    _local_filename = std::move(other._local_filename);
    _remote_filename = std::move(other._remote_filename);
    _resolve_macro = other._resolve_macro;
    _temporary_file = std::move(other._temporary_file);
  }
  return (*this);
}

/**
 *  Destructor.
 */
file::~file() noexcept {
  if (!_temporary_file.empty())
    std::remove(_temporary_file.c_str());
}

/**
 *  Get the local filename.
 *
 *  @return  The local filename.
 */
std::string const& file::get_local_filename() const  noexcept {
  return (_local_filename);
}

/**
 *  Get the remote filename.
 *
 *  @return  The remote filename.
 */
std::string const& file::get_remote_filename() const noexcept {
  return (_remote_filename);
}

/**
 *  Should the content of the file be resolved?
 *
 *  @return  True or false.
 */
bool file::resolve_macro() const noexcept {
  return (_resolve_macro);
}

/**
 *  Get the content of a file.
 *
 *  @return  The content of the file.
 */
std::string file::get_file_content() const {
  std::ifstream ifs;
  std::stringstream ss;

  try {
  ifs.exceptions(~std::ofstream::goodbit);
  ifs.open(_local_filename);
  ss << ifs.rdbuf();
  } catch (std::exception const& e) {
    throw (exceptions::basic()
           << "couldn't access the file '" << _local_filename << "'");
  }

  return (std::move(ss.str()));
}

/**
 *  @brief Set a resolved file content to be put into a temporary file.
 *
 */
void file::set_resolved_file_content(std::string content) {
  // Create temporary file
  std::vector<char> buf;
  buf.resize(strlen(tmp_file_template) + 1);
  ::strcpy(buf.data(), tmp_file_template);
  int fd = ::mkstemp(buf.data());
  if (fd == -1) {
    char const* error = ::strerror(errno);
    throw (exceptions::basic()
           << "can't create temporary file for '"
           << _local_filename << "': " << error);
  }

  _temporary_file = buf.data();

  LOG()
    << "resolving content of file '"
    << _local_filename << "' into '"
    << _temporary_file << "'";

  // Copy content to temporary
  std::ofstream ofs;
  std::stringstream ss;

  try {
  ofs.exceptions(~std::ifstream::goodbit);
  ofs.open(_temporary_file);

  ss << content;
  ofs << ss.rdbuf();
  ofs.flush();
  } catch (std::exception const& e) {
    throw (exceptions::basic()
           << "couldn't create the temporary file '"
           << _temporary_file << "'");
  }
}

/**
 *  Get the temporary file containing the resolved file content, if it exists.
 *
 *  @return  The temporary file containing the resolved file content,
 *           if it exists.
 */
std::string const& file::get_temporary_file() const noexcept {
  return (_temporary_file);
}
