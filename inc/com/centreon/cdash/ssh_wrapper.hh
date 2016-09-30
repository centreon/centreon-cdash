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

#ifndef CCC_SSH_WRAPPER_HH
#  define CCC_SSH_WRAPPER_HH

#  include <string>
#  include "com/centreon/process.hh"
#  include "com/centreon/cdash/namespace.hh"

CCC_BEGIN()

class               ssh_wrapper {
  public:
                    ssh_wrapper(
                      std::string host,
                      unsigned short port,
                      std::string user);
                    ssh_wrapper(ssh_wrapper const&);
    ssh_wrapper&    operator=(ssh_wrapper const&);
                    ~ssh_wrapper();

    void            copy_file(
                      process& proc,
                      std::string const& local_filename,
                      std::string const& remote_filename,
                      std::string const& identity_file_path,
                      unsigned int timeout);
    void            copy_file_back(
                      process& proc,
                      std::string const& local_filename,
                      std::string const& remote_filename,
                      std::string const& identity_file_path,
                      unsigned int timeout);
    void            execute(
                      process& proc,
                      std::string const& command,
                      std::string const& identity_file_path,
                      unsigned int timeout);

  private:
                    ssh_wrapper() = delete;

    std::string     _host;
    unsigned short  _port;
    std::string     _user;
};

CCC_END()

#endif // !CCC_SSH_WRAPPER_HH
