# Install script for directory: /Users/fresh/Dropbox/htdocs/The-Channel/libraries/libfc

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
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/libraries/libfc/secp256k1/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/libraries/libfc/libraries/bn256/src/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/libraries/libfc/libraries/bls12-381/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/libraries/libfc/libraries/boringssl/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/libraries/libfc/libraries/xxhash/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/libraries/libfc/test/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "dev")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/fresh/Dropbox/htdocs/The-Channel/install/lib/libfc.a")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/fresh/Dropbox/htdocs/The-Channel/install/lib" TYPE STATIC_LIBRARY FILES "/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/libraries/libfc/libfc.a")
  if(EXISTS "$ENV{DESTDIR}/Users/fresh/Dropbox/htdocs/The-Channel/install/lib/libfc.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/Users/fresh/Dropbox/htdocs/The-Channel/install/lib/libfc.a")
    execute_process(COMMAND "/usr/bin/ranlib" "$ENV{DESTDIR}/Users/fresh/Dropbox/htdocs/The-Channel/install/lib/libfc.a")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "dev")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/fresh/Dropbox/htdocs/The-Channel/install/include/fc")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/fresh/Dropbox/htdocs/The-Channel/install/include" TYPE DIRECTORY FILES "/Users/fresh/Dropbox/htdocs/The-Channel/libraries/libfc/include/fc")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "dev")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/fresh/Dropbox/htdocs/The-Channel/install/lib/libbls12-381.a")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/fresh/Dropbox/htdocs/The-Channel/install/lib" TYPE STATIC_LIBRARY FILES "/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/libraries/libfc/libraries/bls12-381/libbls12-381.a")
  if(EXISTS "$ENV{DESTDIR}/Users/fresh/Dropbox/htdocs/The-Channel/install/lib/libbls12-381.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/Users/fresh/Dropbox/htdocs/The-Channel/install/lib/libbls12-381.a")
    execute_process(COMMAND "/usr/bin/ranlib" "$ENV{DESTDIR}/Users/fresh/Dropbox/htdocs/The-Channel/install/lib/libbls12-381.a")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "dev")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/fresh/Dropbox/htdocs/The-Channel/install/include/bls12-381/arithmetic.hpp;/Users/fresh/Dropbox/htdocs/The-Channel/install/include/bls12-381/bls12-381.hpp;/Users/fresh/Dropbox/htdocs/The-Channel/install/include/bls12-381/fp.hpp;/Users/fresh/Dropbox/htdocs/The-Channel/install/include/bls12-381/g.hpp;/Users/fresh/Dropbox/htdocs/The-Channel/install/include/bls12-381/pairing.hpp;/Users/fresh/Dropbox/htdocs/The-Channel/install/include/bls12-381/scalar.hpp;/Users/fresh/Dropbox/htdocs/The-Channel/install/include/bls12-381/signatures.hpp")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/fresh/Dropbox/htdocs/The-Channel/install/include/bls12-381" TYPE FILE FILES
    "/Users/fresh/Dropbox/htdocs/The-Channel/libraries/libfc/libraries/bls12-381/include/bls12-381/arithmetic.hpp"
    "/Users/fresh/Dropbox/htdocs/The-Channel/libraries/libfc/libraries/bls12-381/include/bls12-381/bls12-381.hpp"
    "/Users/fresh/Dropbox/htdocs/The-Channel/libraries/libfc/libraries/bls12-381/include/bls12-381/fp.hpp"
    "/Users/fresh/Dropbox/htdocs/The-Channel/libraries/libfc/libraries/bls12-381/include/bls12-381/g.hpp"
    "/Users/fresh/Dropbox/htdocs/The-Channel/libraries/libfc/libraries/bls12-381/include/bls12-381/pairing.hpp"
    "/Users/fresh/Dropbox/htdocs/The-Channel/libraries/libfc/libraries/bls12-381/include/bls12-381/scalar.hpp"
    "/Users/fresh/Dropbox/htdocs/The-Channel/libraries/libfc/libraries/bls12-381/include/bls12-381/signatures.hpp"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/libraries/libfc/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
