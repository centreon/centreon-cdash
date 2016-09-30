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

#include "com/centreon/cdash/ssh_wrapper.hh"
#include "com/centreon/exceptions/basic.hh"
#include "com/centreon/misc/command_line_writer.hh"
#include "com/centreon/process.hh"

using namespace com::centreon;
using namespace com::centreon::cdash;

/**
 *  Constructor.
 *
 *  @param[in] host      Host to use.
 *  @param[in] port      Port to use.
 *  @param[in] user      User to use.
 */
ssh_wrapper::ssh_wrapper(
               std::string host,
               unsigned short port,
               std::string user)
  : _host(host),
    _port(port),
    _user(user) {

}

/**
 *  Copy constructor.
 *
 *  @param[in] other  The object to copy.
 */
ssh_wrapper::ssh_wrapper(ssh_wrapper const& other)
  : _host(other._host),
    _port(other._port),
    _user(other._user) {
}

/**
 *  Assignment operator.
 *
 *  @param[in] other  The object to copy.
 *
 *  @return           A reference to this object.
 */
ssh_wrapper& ssh_wrapper::operator=(ssh_wrapper const& other) {
  if (this != &other) {
    _host = other._host;
    _port = other._port;
    _user = other._user;
  }
  return (*this);
}

/**
 *  Destructor.
 */
ssh_wrapper::~ssh_wrapper() {

}

/**
 *  Copy a file to the distant server.
 *
 *  @param[in] process          Process used to copy the files.
 *  @param[in] local_filename   The local filename of the file to copy.
 *  @param[in] remote_filename  The remote filename of the file to copy.
 *  @param[in] identity_fp      The path of the identity file.
 *  @param[in] timeout          The timeout used to establish the connection.
 */
void ssh_wrapper::copy_file(
                    process& proc,
                    std::string const& local_filename,
                    std::string const& remote_filename,
                    std::string const& identity_fp,
                    unsigned int timeout) {
  misc::command_line_writer writer("scp -oStrictHostKeyChecking=no");
  writer.add_arg("-P", _port);
  writer.add_arg_condition("-i", identity_fp, !identity_fp.empty());
  writer.add_arg(
    "-o",
    std::string("ConnectTimeout=") + std::to_string(timeout));

  std::string command = writer.get_command();
  command.append(" ").append(local_filename);
  command.append(" ").append(_user).append("@")
         .append(_host).append(":")
         .append(remote_filename);

  proc.exec(command);
}

/**
 *  Copy a file back from the distant server.
 *
 *  @param[in] process          Process used to copy the files.
 *  @param[in] local_filename   The local filename of the file to copy.
 *  @param[in] remote_filename  The remote filename of the file to copy.
 *  @param[in] identity_fp      The path of the identity file.
 *  @param[in] timeout          The timeout used to establish the connection.
 */
void ssh_wrapper::copy_file_back(
                    process& proc,
                    std::string const& local_filename,
                    std::string const& remote_filename,
                    std::string const& identity_fp,
                    unsigned int timeout) {
  misc::command_line_writer writer("scp -oStrictHostKeyChecking=no");
  writer.add_arg("-P", _port);
  writer.add_arg_condition("-i", identity_fp, !identity_fp.empty());
  writer.add_arg(
    "-o",
    std::string("ConnectTimeout=") + std::to_string(timeout));

  std::string command = writer.get_command();
  command.append(" ").append(_user).append("@")
         .append(_host).append(":")
         .append(remote_filename);
  command.append(" ").append(local_filename);

  proc.exec(command);
}

/**
 *  Execute a command.
 *
 *  @param[in] process     Process used to execute the remote command.
 *  @param[in] remote_cmd  The command to execute.
 *  @param[in] identity_fp The path of the identity file.
 *  @param[in] timeout     The timeout used to establish the connection.
 */
void ssh_wrapper::execute(
                    process& proc,
                    std::string const& remote_cmd,
                    std::string const& identity_fp,
                    unsigned int timeout) {
  misc::command_line_writer writer("ssh -oStrictHostKeyChecking=no");
  writer.add_arg("-p", _port);
  writer.add_arg_condition("-i", identity_fp, !identity_fp.empty());
  writer.add_arg(
    "-o",
    std::string("ConnectTimeout=") + std::to_string(timeout));

  std::string command = writer.get_command();
  command.append(" ").append(_user).append("@")
         .append(_host).append(" ")
         .append(remote_cmd);

  proc.exec(command);
}
