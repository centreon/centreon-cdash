##
## Copyright 2015-2016 Centreon
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##    http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.
##

# Global options.
cmake_minimum_required(VERSION 2.8)
project("Centreon CDash" C CXX)
set(CMAKE_CXX_FLAGS "-std=c++0x")

# Set directories.
set(PROJECT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/..")
include_directories("${PROJECT_SOURCE_DIR}/inc")
set(INC_DIR "${PROJECT_SOURCE_DIR}/inc/com/centreon/cdash")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(CONF_DIR "${PROJECT_SOURCE_DIR}/conf")

# Version.
set(CENTREON_CDASH_MAJOR 1)
set(CENTREON_CDASH_MINOR 0)
set(CENTREON_CDASH_PATCH 0)
set(CENTREON_CDASH_VERSION "${CENTREON_CDASH_MAJOR}.${CENTREON_CDASH_MINOR}.${CENTREON_CDASH_PATCH}")
message(STATUS "Generating version header file.")
configure_file(
  "${INC_DIR}/version.hh.in"
  "${INC_DIR}/version.hh"
  "@ONLY"
)

# Try to find Centreon-Clib include dirs.
if (WITH_CENTREON_CLIB_INCLUDE_DIR)
  find_file(
    CLIB_HEADER_FOUND
    "com/centreon/clib/version.hh"
    PATHS "${WITH_CENTREON_CLIB_INCLUDE_DIR}"
    NO_DEFAULT_PATH)
  if (NOT CLIB_HEADER_FOUND)
    message(FATAL_ERROR "Could not find Centreon Clib's headers in ${WITH_CENTREON_CLIB_INCLUDE_DIR}.")
  endif ()
  include_directories("${WITH_CENTREON_CLIB_INCLUDE_DIR}")
  set(CLIB_INCLUDE_DIR "${WITH_CENTREON_CLIB_INCLUDE_DIR}")
else ()
  find_path(
    CLIB_INCLUDE_DIR
    "com/centreon/clib/version.hh"
    PATH_SUFFIXES
    "centreon-clib")
  if (NOT CLIB_INCLUDE_DIR)
    message(FATAL_ERROR "Could not find Centreon Clib's headers (try WITH_CENTREON_CLIB_INCLUDE_DIR).")
  endif ()
endif ()
include_directories(${CLIB_INCLUDE_DIR})

# Try to find Centreon Clib's library.
if (WITH_CENTREON_CLIB_LIBRARIES)
  set(CLIB_LIBRARIES "${WITH_CENTREON_CLIB_LIBRARIES}")
elseif (WITH_CENTREON_CLIB_LIBRARY_DIR)
  find_library(
    CLIB_LIBRARIES
    "centreon_clib"
    PATHS "${WITH_CENTREON_CLIB_LIBRARY_DIR}"
    NO_DEFAULT_PATH)
  if (NOT CLIB_LIBRARIES)
    message(FATAL_ERROR "Could not find Centreon Clib's library in ${WITH_CENTREON_CLIB_LIBRARY_DIR}.")
  endif ()
else ()
  find_library(CLIB_LIBRARIES "centreon_clib")
  if (NOT CLIB_LIBRARIES)
    message(FATAL_ERROR "Could not find Centreon Clib's library (try WITH_CENTREON_CLIB_LIBRARY_DIR or WITH_CENTREON_CLIB_LIBRARIES).")
  endif ()
endif ()

# Find libxml2
find_package (LibXml2)
if (LIBXML2_FOUND)
  include_directories(${LIBXML2_INCLUDE_DIR})
else ()
  message(FATAL_ERROR "Could not find LibXml2 library.")
endif (LIBXML2_FOUND)

# Find pthreads.
if (WIN32)
  include(FindThreads)
  if (NOT CMAKE_USE_WIN32_THREADS_INIT)
    message(FATAL_ERROR "Could not find Win32 threads.")
  endif ()
else ()
  set(CMAKE_THREAD_PREFER_PTHREAD)
  include(FindThreads)
  if (NOT CMAKE_USE_PTHREADS_INIT)
    message(FATAL_ERROR "Could not find pthreads library.")
  endif ()
endif ()
if (CMAKE_SYSTEM_NAME STREQUAL "OpenBSD" AND CMAKE_COMPILER_IS_GNUCXX)
  set(LIB_THREAD "-pthread -lpthread")
else ()
  set(LIB_THREAD "${CMAKE_THREAD_LIBS_INIT}")
endif ()

# Set general path.
set(PREFIX "${CMAKE_INSTALL_PREFIX}/centreon-cdash")
if (WITH_PREFIX)
  set(PREFIX "${WITH_PREFIX}")
  set(CMAKE_INSTALL_PREFIX "${PREFIX}")
endif ()

# Set binary path.
set(PREFIX_BIN "${CMAKE_INSTALL_PREFIX}/bin")
if (WITH_PREFIX_BIN)
  set(PREFIX_BIN "${WITH_PREFIX_BIN}")
endif ()

# Set configuration path.
set(PREFIX_CONF "${CMAKE_INSTALL_PREFIX}/etc")
if (WITH_PREFIX_CONF)
  set(PREFIX_CONF "${WITH_PREFIX_CONF}")
endif ()

# Generate configuration file.
message(STATUS "Generating configuration file.")
configure_file(
  "${CONF_DIR}/cdash.xml.in"
  "${CONF_DIR}/cdash.xml"
  "@ONLY"
)

# Core library.
set(LIB_NAME "centreon_cdash_core")
add_library(
  "${LIB_NAME}" STATIC
  # Sources.
  "${SRC_DIR}/args_parser.cc"
  "${SRC_DIR}/file.cc"
  "${SRC_DIR}/file_parser.cc"
  "${SRC_DIR}/log/engine.cc"
  "${SRC_DIR}/log/error.cc"
  "${SRC_DIR}/log/log.cc"
  "${SRC_DIR}/object.cc"
  "${SRC_DIR}/sequence.cc"
  "${SRC_DIR}/ssh_wrapper.cc"
  "${SRC_DIR}/task.cc"
  "${SRC_DIR}/task_manager.cc"
  "${SRC_DIR}/task_process.cc"
  "${SRC_DIR}/xml_tree_parser.cc"
  "${SRC_DIR}/xml/library.cc"
  "${SRC_DIR}/xml/tree.cc"
  "${SRC_DIR}/xml/node.cc"
  "${SRC_DIR}/xml/node_iterator.cc"
  "${SRC_DIR}/xml/property.cc"
  # Headers.
  "${INC_DIR}/namespace.hh"
  "${INC_DIR}/version.hh"
  "${INC_DIR}/args_parser.hh"
  "${INC_DIR}/file.hh"
  "${INC_DIR}/file_parser.hh"
  "${INC_DIR}/log/engine.hh"
  "${INC_DIR}/log/error.hh"
  "${INC_DIR}/log/log.hh"
  "${INC_DIR}/object.hh"
  "${INC_DIR}/sequence.hh"
  "${INC_DIR}/ssh_wrapper.hh"
  "${INC_DIR}/task.hh"
  "${INC_DIR}/task_manager.hh"
  "${INC_DIR}/task_process.hh"
  "${INC_DIR}/xml_tree_parser.hh"
  "${INC_DIR}/xml/library.hh"
  "${INC_DIR}/xml/tree.hh"
  "${INC_DIR}/xml/node.hh"
  "${INC_DIR}/xml/node_iterator.hh"
  "${INC_DIR}/xml/property.hh"
)
target_link_libraries(
  "${LIB_NAME}"
  ${LIB_THREAD}
  ${CLIB_LIBRARIES}
  ${LIBXML2_LIBRARIES}
)

# Executable.
add_executable(
  "centreon_cdash"
  "${SRC_DIR}/main.cc"
)
target_link_libraries(
  "centreon_cdash"
  ${LIB_THREAD}
  "${LIB_NAME}"
  "rt"
)
# Install executable library.
install(
  TARGETS "centreon_cdash"
  DESTINATION "${PREFIX_BIN}"
  COMPONENT "runtime"
)

# Unit test.
option(WITH_TESTING "Build unit tests." OFF)
set(UNIT_TEST "No")
if (WITH_TESTING)
  set(UNIT_TEST "Yes")
  set(CTEST_TESTING_TIMEOUT 20)
  include(CTest)
  add_subdirectory("test")
endif ()

# Print summary.
message(STATUS "")
message(STATUS "Configuration Summary")
message(STATUS "---------------------")
message(STATUS "")
message(STATUS "  Project")
message(STATUS "    - Name                      ${PROJECT_NAME}")
message(STATUS "    - Version                   ${CENTREON_CDASH_VERSION}")
message(STATUS "")
message(STATUS "  System")
message(STATUS "    - Name                      ${CMAKE_SYSTEM_NAME}")
message(STATUS "    - Version                   ${CMAKE_SYSTEM_VERSION}")
message(STATUS "    - Processor                 ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "")
message(STATUS "  Build")
message(STATUS "    - Compiler                  ${CMAKE_CXX_COMPILER} (${CMAKE_CXX_COMPILER_ID})")
message(STATUS "    - Extra compilation flags   ${CMAKE_CXX_FLAGS}")
message(STATUS "    - Build unit tests          ${UNIT_TEST}")
message(STATUS "")
message(STATUS "  Install")
message(STATUS "    - Prefix                    ${PREFIX}")
message(STATUS "    - Binary prefix             ${PREFIX_BIN}")
message(STATUS "    - Configuration prefix      ${PREFIX_CONF}")
message(STATUS "    - var directory             ${PREFIX_VAR}")
