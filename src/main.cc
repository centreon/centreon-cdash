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

#include <utility>
#include <csignal>
#include <ctime>
#include <string>
#include <iostream>
#include "com/centreon/cdash/args_parser.hh"
#include "com/centreon/cdash/file_parser.hh"
#include "com/centreon/cdash/xml_tree_parser.hh"
#include "com/centreon/cdash/task_manager.hh"
#include "com/centreon/cdash/object.hh"
#include "com/centreon/cdash/task.hh"
#include "com/centreon/cdash/sequence.hh"
#include "com/centreon/exceptions/basic.hh"
#include "com/centreon/cdash/xml/library.hh"
#include "com/centreon/aws/ec2/command.hh"
#include "com/centreon/process_manager.hh"
#include "com/centreon/cdash/log/log.hh"

using namespace com::centreon;
using namespace com::centreon::io;
using namespace com::centreon::cdash;
using namespace com::centreon::cdash::log;

/**
 *  Signal handler
 *
 *  @param sig
 */
static void signal_handler(int sig) {
  if (sig == SIGTERM || sig == SIGINT) {
    std::cout << "termination asked, exiting..." << std::endl;
    cdash::task_manager::should_exit = true;
  }
}

/**
 *  Build C++ projects.
 *
 *  @param[in] argc  Argument count.
 *  @param[in] argv  Argument values.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main(int argc, char* argv[]) {

  // Add signal manager.
  {
    struct sigaction sig;
    sig.sa_handler = signal_handler;
    ::sigemptyset(&sig.sa_mask);
    ::sigfillset(&sig.sa_mask);
    sig.sa_flags = 0;
    if (::sigaction(SIGTERM, &sig, nullptr) < 0
        || ::sigaction(SIGINT, &sig, nullptr) < 0) {
      std::cerr << "can't set signal handlers" << std::endl;
      return (-1);
    }
  }

  // Parse the arguments.
  args_parser parser;
  try {
  parser.parse(argc - 1, argv + 1);
  } catch (std::exception const& e) {
    std::cerr << "can't parse the arguments: " << e.what() << std::endl;
    return (-1);
  }

  // Print help if needed.
  if (parser.get_argument('h').is_set()
        || parser.get_parameters().size() == 0) {
    parser.print_help();
    return (0);
  }

  // Initialize the process manager.
  process_manager::load();

  std::vector<std::vector<object>> sequence_objects;
  std::string profile;

  {
    // Initialize xml library.
    xml::library _;

    // Parse files.
    for (auto const& parameter : parser.get_parameters()) {
      try {
        file_parser fp(parameter);
        xml_tree_parser xtp(fp.parse());
        xtp.parse(sequence_objects, profile);
      } catch (std::exception const& e) {
        std::cerr << "couldn't parse configuration: " << e.what() << std::endl;
        return (-1);
      }
    }
  }

  // Create sequences.
  std::vector<sequence> sequences;
  try {
    for (auto& objects : sequence_objects) {
      sequence seq;
      for (auto& object : objects)
        seq.add_task(task(object));
      sequences.emplace_back(std::move(seq));
    }
  } catch (std::exception const& e) {
    std::cerr << "couldn't create tasks: " << e.what() << std::endl;
    return (-1);
  }

  std::cout << "resolved " << sequences.size()
            << " sequence(s) of tasks" << std::endl;

  // Create task manager.
  try {
  cdash::task_manager manager(profile);
  manager.run(std::move(sequences));
  } catch (std::exception const& e) {
    std::cerr << "error in execution: " << e.what() << std::endl;
  }

  // Deinitialize the process manager.
  process_manager::unload();

  return (0);
}
