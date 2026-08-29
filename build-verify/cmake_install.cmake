# Install script for directory: /Users/fresh/Dropbox/htdocs/The-Channel

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/Users/fresh/Dropbox/htdocs/The-Channel/install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-verify/libraries/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-verify/plugins/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-verify/programs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-verify/scripts/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-verify/unittests/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-verify/tests/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-verify/tools/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-verify/benchmark/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "dev")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/fresh/Dropbox/htdocs/The-Channel/install/include/eosio.version.hpp")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/fresh/Dropbox/htdocs/The-Channel/install/include" TYPE FILE FILES "/Users/fresh/Dropbox/htdocs/The-Channel/build-verify/eosio.version.hpp")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "dev")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/fresh/Dropbox/htdocs/The-Channel/install/lib/cmake/eosio/eosio-config.cmake")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/fresh/Dropbox/htdocs/The-Channel/install/lib/cmake/eosio" TYPE FILE FILES "/Users/fresh/Dropbox/htdocs/The-Channel/build-verify/modules/eosio-config.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "dev")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/fresh/Dropbox/htdocs/The-Channel/install/lib/cmake/eosio/EosioTester.cmake")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/fresh/Dropbox/htdocs/The-Channel/install/lib/cmake/eosio" TYPE FILE FILES "/Users/fresh/Dropbox/htdocs/The-Channel/build-verify/modules/EosioTester.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "dev")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/fresh/Dropbox/htdocs/The-Channel/install/lib/cmake/eosio/EosioCheckVersion.cmake")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/fresh/Dropbox/htdocs/The-Channel/install/lib/cmake/eosio" TYPE FILE FILES "/Users/fresh/Dropbox/htdocs/The-Channel/CMakeModules/EosioCheckVersion.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "dev")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/fresh/Dropbox/htdocs/The-Channel/install/lib/cmake/leap/leap-config.cmake")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/fresh/Dropbox/htdocs/The-Channel/install/lib/cmake/leap" TYPE FILE FILES "/Users/fresh/Dropbox/htdocs/The-Channel/build-verify/modules/leap-config.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "dev")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/fresh/Dropbox/htdocs/The-Channel/install/lib/cmake/leap/EosioTester.cmake")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/fresh/Dropbox/htdocs/The-Channel/install/lib/cmake/leap" TYPE FILE FILES "/Users/fresh/Dropbox/htdocs/The-Channel/build-verify/modules/EosioTester.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "dev")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/fresh/Dropbox/htdocs/The-Channel/install/lib/cmake/leap/EosioCheckVersion.cmake")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/fresh/Dropbox/htdocs/The-Channel/install/lib/cmake/leap" TYPE FILE FILES "/Users/fresh/Dropbox/htdocs/The-Channel/CMakeModules/EosioCheckVersion.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "dev")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/fresh/Dropbox/htdocs/The-Channel/install/lib/cmake/channel/channel-config.cmake")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/fresh/Dropbox/htdocs/The-Channel/install/lib/cmake/channel" TYPE FILE FILES "/Users/fresh/Dropbox/htdocs/The-Channel/build-verify/modules/channel-config.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "dev")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/fresh/Dropbox/htdocs/The-Channel/install/lib/cmake/channel/EosioTester.cmake")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/fresh/Dropbox/htdocs/The-Channel/install/lib/cmake/channel" TYPE FILE FILES "/Users/fresh/Dropbox/htdocs/The-Channel/build-verify/modules/EosioTester.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "dev")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/fresh/Dropbox/htdocs/The-Channel/install/lib/cmake/channel/EosioCheckVersion.cmake")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/fresh/Dropbox/htdocs/The-Channel/install/lib/cmake/channel" TYPE FILE FILES "/Users/fresh/Dropbox/htdocs/The-Channel/CMakeModules/EosioCheckVersion.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "base" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/fresh/Dropbox/htdocs/The-Channel/install/share/licenses//channel")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/fresh/Dropbox/htdocs/The-Channel/install/share/licenses/" TYPE DIRECTORY FILES "/Users/fresh/Dropbox/htdocs/The-Channel/build-verify/licenses/channel")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "dev")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/fresh/Dropbox/htdocs/The-Channel/install/share/channel_testing/libraries/testing/contracts")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/fresh/Dropbox/htdocs/The-Channel/install/share/channel_testing/libraries/testing" TYPE DIRECTORY FILES "/Users/fresh/Dropbox/htdocs/The-Channel/build-verify/libraries/testing/contracts" REGEX "/cmakefiles$" EXCLUDE REGEX "/[^/]*\\.cmake$" EXCLUDE REGEX "/makefile$" EXCLUDE)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "dev")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/fresh/Dropbox/htdocs/The-Channel/install/share/channel_testing/unittests/contracts")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/fresh/Dropbox/htdocs/The-Channel/install/share/channel_testing/unittests" TYPE DIRECTORY FILES "/Users/fresh/Dropbox/htdocs/The-Channel/build-verify/unittests/contracts" REGEX "/cmakefiles$" EXCLUDE REGEX "/[^/]*\\.cmake$" EXCLUDE REGEX "/makefile$" EXCLUDE)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "dev")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/fresh/Dropbox/htdocs/The-Channel/install/share/channel_testing/tests/TestHarness")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/fresh/Dropbox/htdocs/The-Channel/install/share/channel_testing/tests" TYPE DIRECTORY FILES "/Users/fresh/Dropbox/htdocs/The-Channel/build-verify/tests/TestHarness" FILES_MATCHING REGEX "/[^/]*\\.py$" REGEX "/[^/]*\\.json$" REGEX "/\\_\\_pycache\\_\\_$" EXCLUDE REGEX "/cmakefiles$" EXCLUDE)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "dev")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/fresh/Dropbox/htdocs/The-Channel/install/lib/python3/dist-packages/TestHarness")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/fresh/Dropbox/htdocs/The-Channel/install/lib/python3/dist-packages" TYPE FILE FILES "/Users/fresh/Dropbox/htdocs/The-Channel/build-verify/lib/python3/dist-packages/TestHarness")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "dev")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/fresh/Dropbox/htdocs/The-Channel/install/share/channel_testing/bin")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/fresh/Dropbox/htdocs/The-Channel/install/share/channel_testing" TYPE FILE FILES "/Users/fresh/Dropbox/htdocs/The-Channel/build-verify/share/channel_testing/bin")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "base" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/fresh/Dropbox/htdocs/The-Channel/install/share/bash-completion/completions/channel-util")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/fresh/Dropbox/htdocs/The-Channel/install/share/bash-completion/completions" TYPE FILE FILES "/Users/fresh/Dropbox/htdocs/The-Channel/libraries/cli11/bash-completion/completions/channel-util")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "base" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/fresh/Dropbox/htdocs/The-Channel/install/share/bash-completion/completions/chan")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/fresh/Dropbox/htdocs/The-Channel/install/share/bash-completion/completions" TYPE FILE FILES "/Users/fresh/Dropbox/htdocs/The-Channel/libraries/cli11/bash-completion/completions/chan")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "dev")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/fresh/Dropbox/htdocs/The-Channel/install/share/spring_boost/")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/fresh/Dropbox/htdocs/The-Channel/install/share/spring_boost" TYPE DIRECTORY FILES "/Users/fresh/Dropbox/htdocs/The-Channel/libraries/boost/" REGEX "/\\.git\\/[^/]*$" EXCLUDE REGEX "/example\\/[^/]*$" EXCLUDE REGEX "/bench\\/[^/]*$" EXCLUDE REGEX "/doc\\/[^/]*$" EXCLUDE REGEX "/libs\\/[^/]*\\/test$" EXCLUDE REGEX "/tools\\/[^/]*\\/test$" EXCLUDE)
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/Users/fresh/Dropbox/htdocs/The-Channel/build-verify/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
if(CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_COMPONENT MATCHES "^[a-zA-Z0-9_.+-]+$")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
  else()
    string(MD5 CMAKE_INST_COMP_HASH "${CMAKE_INSTALL_COMPONENT}")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INST_COMP_HASH}.txt")
    unset(CMAKE_INST_COMP_HASH)
  endif()
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/Users/fresh/Dropbox/htdocs/The-Channel/build-verify/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
