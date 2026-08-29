# Install script for directory: /Users/fresh/Dropbox/htdocs/The-Channel/unittests/test-contracts

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
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/unittests/test-contracts/asserter/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/unittests/test-contracts/deferred_test/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/unittests/test-contracts/get_sender_test/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/unittests/test-contracts/get_table_test/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/unittests/test-contracts/get_table_seckey_test/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/unittests/test-contracts/integration_test/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/unittests/test-contracts/no_auth_table/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/unittests/test-contracts/noop/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/unittests/test-contracts/payloadless/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/unittests/test-contracts/proxy/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/unittests/test-contracts/ram_restrictions_test/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/unittests/test-contracts/reject_all/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/unittests/test-contracts/restrict_action_test/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/unittests/test-contracts/snapshot_test/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/unittests/test-contracts/sync_caller/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/unittests/test-contracts/sync_callee/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/unittests/test-contracts/sync_callee1/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/unittests/test-contracts/test_api/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/unittests/test-contracts/test_api_db/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/unittests/test-contracts/test_api_multi_index/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/unittests/test-contracts/test_ram_limit/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/unittests/test-contracts/action_results/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/unittests/test-contracts/wasm_config_bios/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/unittests/test-contracts/params_test/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/unittests/test-contracts/crypto_primitives_test/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/unittests/test-contracts/bls_primitives_test/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/unittests/test-contracts/get_block_num_test/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/unittests/test-contracts/nested_container_multi_index/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/unittests/test-contracts/savanna/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/unittests/test-contracts/db_find_secondary_test/cmake_install.cmake")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/Users/fresh/Dropbox/htdocs/The-Channel/build-arm64/unittests/test-contracts/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
