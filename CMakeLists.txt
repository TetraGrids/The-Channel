cmake_minimum_required( VERSION 3.21...4.0 )

project( the-channel )
include(CTest)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

include( GNUInstallDirs )
include( MASSigning )
include( AddTestHelpers )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_EXTENSIONS ON )
set( CXX_STANDARD_REQUIRED ON)

# When CMake runs under Rosetta on Apple Silicon, CMAKE_SYSTEM_PROCESSOR may report x86_64
# even when CMAKE_OSX_ARCHITECTURES targets arm64, causing x86_64 assembly to be selected
# for arm64 compilation. Align the processor with the requested OSX architecture.
if(APPLE AND CMAKE_OSX_ARCHITECTURES)
   list(GET CMAKE_OSX_ARCHITECTURES 0 _osx_arch0)
   if(_osx_arch0 MATCHES "arm64|aarch64" AND NOT CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
      message(STATUS "CMAKE_OSX_ARCHITECTURES=${_osx_arch0}; overriding CMAKE_SYSTEM_PROCESSOR from ${CMAKE_SYSTEM_PROCESSOR} to arm64")
      set(CMAKE_SYSTEM_PROCESSOR arm64)
   endif()
   unset(_osx_arch0)
endif()

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)
set(VERSION_SUFFIX dev1)

if(VERSION_SUFFIX)
    set(VERSION_FULL "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}-${VERSION_SUFFIX}")
else()
    set(VERSION_FULL "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
endif()


# Options
option(ENABLE_WERROR "Enable `-Werror` compilation flag." Off)
option(ENABLE_WEXTRA "Enable `-Wextra` compilation flag." Off)


set( CLI_CLIENT_EXECUTABLE_NAME chan )
set( NODE_EXECUTABLE_NAME channeld )
set( KEY_STORE_EXECUTABLE_NAME keyd )
set( CHANNEL_UTIL_EXECUTABLE_NAME channel-util )

# http://stackoverflow.com/a/18369825
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 11.4)
        message(FATAL_ERROR "GCC version must be at least 11.4.0!")
    endif()
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
endif()

if ("${CMAKE_GENERATOR}" STREQUAL "Ninja")
   add_compile_options(-fdiagnostics-color=always)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")
set(BUILD_DOXYGEN FALSE CACHE BOOL "Build doxygen documentation on every make")
set(ENABLE_MULTIVERSION_PROTOCOL_TEST FALSE CACHE BOOL "Enable channeld multiversion protocol test")

option(ENABLE_OC "Enable eosvm-oc on supported platforms" ON)

# WASM runtimes to enable. Each runtime in this list will have:
#  * definition EOSIO_<RUNTIME>_RUNTIME_ENABLED defined in public libchain interface
#  * ctest entries with --runtime
if(ENABLE_OC AND CMAKE_SIZEOF_VOID_P EQUAL 8 AND NOT WIN32)
   if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux" AND "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
      list(APPEND EOSIO_WASM_RUNTIMES eos-vm-oc)
      # EOS VM OC requires LLVM, but move the check up here to a central location so that the EosioTester.cmakes
      # can be created with the exact version found
      find_package(LLVM REQUIRED CONFIG)
      if(LLVM_VERSION_MAJOR VERSION_LESS 7 OR LLVM_VERSION_MAJOR VERSION_GREATER_EQUAL 12)
	      message(FATAL_ERROR "The Channel requires an LLVM version 7 through 11")
      endif()
   endif()
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8 AND NOT WIN32)
   if(CMAKE_SYSTEM_PROCESSOR STREQUAL x86_64 OR CMAKE_SYSTEM_PROCESSOR STREQUAL amd64)
      list(APPEND EOSIO_WASM_RUNTIMES eos-vm eos-vm-jit)
   else()
      list(APPEND EOSIO_WASM_RUNTIMES eos-vm)
   endif()
endif()

if(UNIX)
  if(APPLE)
    set(whole_archive_flag "-force_load")
    set(no_whole_archive_flag "")
  else()
    set(whole_archive_flag "--whole-archive")
    set(no_whole_archive_flag "--no-whole-archive")
  endif()
else()
  set(whole_archive_flag "--whole-archive")
  set(no_whole_archive_flag "--no-whole-archive")
endif()

if( APPLE AND UNIX )
# Apple Specific Options Here
    message( STATUS "Configuring The Channel on macOS" )
    set( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wall -Wno-deprecated-declarations" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-deprecated-declarations" )
else()
    # Linux Specific Options Here
    message( STATUS "Configuring The Channel on Linux" )
    set( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wall" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall" )
    if ( FULL_STATIC_BUILD )
      set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
    endif ( FULL_STATIC_BUILD )

    if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
        if( CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 4.0.0 OR CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.0.0 )
            set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-partial-specialization" )
        endif()
    endif()

    set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -latomic")
endif()

if(ENABLE_WERROR)
  set( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Werror" )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror" )
endif()
if(ENABLE_WEXTRA)
  set( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wextra" )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra" )
endif()


if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 14.0)
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wthread-safety" )
endif()

option(EOSIO_ENABLE_DEVELOPER_OPTIONS "enable developer options for The Channel" OFF)

# based on http://www.delorie.com/gnu/docs/gdb/gdb_70.html
# uncomment this line to tell GDB about macros (slows compile times)
# set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -gdwarf-2 -g3" )

set(ENABLE_COVERAGE_TESTING FALSE CACHE BOOL "Build The Channel for code coverage analysis")

if(ENABLE_COVERAGE_TESTING)
  SET(CMAKE_C_FLAGS   "--coverage ${CMAKE_C_FLAGS}")
  SET(CMAKE_CXX_FLAGS "--coverage ${CMAKE_CXX_FLAGS}")
  find_program( LCOV_PATH lcov )
  find_program( LLVMCOV_PATH  llvm-cov )
  find_program( GENHTML_PATH NAMES genhtml)
endif()

include(utils)

if ("${CORE_SYMBOL_NAME}" STREQUAL "")
  set( CORE_SYMBOL_NAME "RA" )
endif()
string(TOUPPER ${CORE_SYMBOL_NAME} CORE_SYMBOL_NAME)

string(LENGTH ${CORE_SYMBOL_NAME} CORE_SYMBOL_NAME_LENGTH)
if (CORE_SYMBOL_NAME_LENGTH GREATER 7)
  message(FATAL_ERROR "CORE_SYMBOL_NAME length must be between 1 and 7 characters")
endif()

message( STATUS "Using '${CORE_SYMBOL_NAME}' as CORE symbol name" )

if ("${CHANNEL_ROOT_KEY}" STREQUAL "")
   set(CHANNEL_ROOT_KEY "PUB_K1_6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5BoDq63")
endif()

message( STATUS "Using '${CHANNEL_ROOT_KEY}' as public key for 'ra' account" )

option(ENABLE_TCMALLOC "use tcmalloc (requires gperftools)" OFF)
if( ENABLE_TCMALLOC )
    find_package( Gperftools REQUIRED )
    message( STATUS "Compiling The Channel with TCMalloc")
    #if doing this by the book, simply link_libraries( ${GPERFTOOLS_TCMALLOC} ) here. That will
    #give the performance benefits of tcmalloc but since it won't be linked last
    #the heap profiler & checker may not be accurate. This here is rather undocumented behavior
    #to stuff a library toward the end of the link list
    set(CMAKE_C_STANDARD_LIBRARIES "${CMAKE_C_STANDARD_LIBRARIES} ${GPERFTOOLS_TCMALLOC}")
    set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} ${GPERFTOOLS_TCMALLOC}")
endif()

# channel includes a bundled BoringSSL which conflicts with OpenSSL. Make sure any other bundled libraries (such as boost)
# do not attempt to use an external OpenSSL in any manner
set(CMAKE_DISABLE_FIND_PACKAGE_OpenSSL On)

# many tests require handling of signals themselves and even when they don't we'd like for them to generate a core dump, this
# is effectively --catch_system_errors=no broadly across all tests
add_compile_definitions(BOOST_TEST_DEFAULTS_TO_CORE_DUMP)

# Unless this is set, some boost unordered headers include `arm_neon.h` when building on ARM processors,
# which includes a definition for `float16_t` conflicting with the one in `libraries/softfloat`.
add_compile_definitions(BOOST_UNORDERED_DISABLE_NEON)

add_subdirectory( libraries )
add_subdirectory( plugins )
add_subdirectory( programs )
add_subdirectory( scripts )
add_subdirectory( unittests )
add_subdirectory( tests )
add_subdirectory( tools )
add_subdirectory( benchmark )

# Core system contracts (ra.*) are built with CDT as an external project.
# Enable with -DBUILD_CONTRACTS=ON (requires CDT 3.x/4.x).
option(BUILD_CONTRACTS "Build the ra.* core system contracts (requires CDT)" OFF)
if(BUILD_CONTRACTS)
  include(ExternalProject)
  find_package(cdt)
  ExternalProject_Add(
    contracts_project
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/contracts
    BINARY_DIR ${CMAKE_BINARY_DIR}/contracts
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release
               -DCMAKE_TOOLCHAIN_FILE=${CDT_ROOT}/lib/cmake/cdt/CDTWasmToolchain.cmake
               -DSYSTEM_CONFIGURABLE_WASM_LIMITS=ON
               -DSYSTEM_BLOCKCHAIN_PARAMETERS=ON
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    TEST_COMMAND ""
    INSTALL_COMMAND ""
    BUILD_ALWAYS 1)
endif()

option(DISABLE_WASM_SPEC_TESTS "disable building of wasm spec unit tests" OFF)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/eosio.version.in ${CMAKE_CURRENT_BINARY_DIR}/eosio.version.hpp)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/eosio.version.hpp DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR} COMPONENT dev EXCLUDE_FROM_ALL)

set(EOS_ROOT_DIR "${CMAKE_BINARY_DIR}/lib")
# legacy eosio CMake files
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/eosio-config.cmake.in ${CMAKE_BINARY_DIR}/lib/cmake/eosio/eosio-config.cmake @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/EosioTesterBuild.cmake.in ${CMAKE_BINARY_DIR}/lib/cmake/eosio/EosioTester.cmake @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/EosioCheckVersion.cmake ${CMAKE_BINARY_DIR}/lib/cmake/eosio/EosioCheckVersion.cmake COPYONLY)
# legacy leap CMake files
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/leap-config.cmake.in ${CMAKE_BINARY_DIR}/lib/cmake/leap/leap-config.cmake @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/EosioTesterBuild.cmake.in ${CMAKE_BINARY_DIR}/lib/cmake/leap/EosioTester.cmake @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/EosioCheckVersion.cmake ${CMAKE_BINARY_DIR}/lib/cmake/leap/EosioCheckVersion.cmake COPYONLY)
# channel CMake files
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/channel-config.cmake.in ${CMAKE_BINARY_DIR}/lib/cmake/channel/channel-config.cmake @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/EosioTesterBuild.cmake.in ${CMAKE_BINARY_DIR}/lib/cmake/channel/EosioTester.cmake @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/EosioCheckVersion.cmake ${CMAKE_BINARY_DIR}/lib/cmake/channel/EosioCheckVersion.cmake COPYONLY)

set(EOS_ROOT_DIR "${CMAKE_INSTALL_FULL_LIBDIR}")
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/EosioTester.cmake.in ${CMAKE_BINARY_DIR}/modules/EosioTester.cmake @ONLY)
# legacy eosio CMake files
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/eosio-config.cmake.in ${CMAKE_BINARY_DIR}/modules/eosio-config.cmake @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/modules/eosio-config.cmake DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/eosio COMPONENT dev EXCLUDE_FROM_ALL)
install(FILES ${CMAKE_BINARY_DIR}/modules/EosioTester.cmake DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/eosio COMPONENT dev EXCLUDE_FROM_ALL)
install(FILES ${CMAKE_SOURCE_DIR}/CMakeModules/EosioCheckVersion.cmake DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/eosio COMPONENT dev EXCLUDE_FROM_ALL)
# legacy leap CMake files
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/leap-config.cmake.in ${CMAKE_BINARY_DIR}/modules/leap-config.cmake @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/modules/leap-config.cmake DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/leap COMPONENT dev EXCLUDE_FROM_ALL)
install(FILES ${CMAKE_BINARY_DIR}/modules/EosioTester.cmake DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/leap COMPONENT dev EXCLUDE_FROM_ALL)
install(FILES ${CMAKE_SOURCE_DIR}/CMakeModules/EosioCheckVersion.cmake DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/leap COMPONENT dev EXCLUDE_FROM_ALL)
# channel CMake files
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/channel-config.cmake.in ${CMAKE_BINARY_DIR}/modules/channel-config.cmake @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/modules/channel-config.cmake DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/channel COMPONENT dev EXCLUDE_FROM_ALL)
install(FILES ${CMAKE_BINARY_DIR}/modules/EosioTester.cmake DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/channel COMPONENT dev EXCLUDE_FROM_ALL)
install(FILES ${CMAKE_SOURCE_DIR}/CMakeModules/EosioCheckVersion.cmake DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/channel COMPONENT dev EXCLUDE_FROM_ALL)

configure_file(LICENSE                                                     licenses/channel/LICENSE           COPYONLY)
configure_file(libraries/softfloat/COPYING.txt                             licenses/channel/LICENSE.softfloat COPYONLY)
configure_file(libraries/wasm-jit/LICENSE                                  licenses/channel/LICENSE.wavm      COPYONLY)
configure_file(libraries/libfc/secp256k1/secp256k1/COPYING                 licenses/channel/LICENSE.secp256k1 COPYONLY)
configure_file(libraries/libfc/include/fc/crypto/webauthn_json/license.txt licenses/channel/LICENSE.rapidjson COPYONLY)
configure_file(libraries/eos-vm/LICENSE                                    licenses/channel/LICENSE.eos-vm    COPYONLY)
configure_file(libraries/prometheus/prometheus-cpp/LICENSE                 licenses/channel/LICENSE.prom      COPYONLY)
configure_file(programs/chan/LICENSE.CLI11                                licenses/channel/LICENSE.CLI11     COPYONLY)
configure_file(libraries/libfc/libraries/bls12-381/LICENSE                 licenses/channel/LICENSE.bls12-381 COPYONLY)
configure_file(libraries/libfc/libraries/boringssl/bssl/LICENSE            licenses/channel/LICENSE.boringssl COPYONLY)
configure_file(libraries/libfc/libraries/xxhash/xxHash/LICENSE             licenses/channel/LICENSE.xxHash    COPYONLY)

install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/licenses/channel" DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/licenses/" COMPONENT base)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libraries/testing/contracts DESTINATION ${CMAKE_INSTALL_FULL_DATAROOTDIR}/channel_testing/libraries/testing COMPONENT dev EXCLUDE_FROM_ALL
        PATTERN "CMakeFiles" EXCLUDE
        PATTERN "*.cmake" EXCLUDE
        PATTERN "Makefile" EXCLUDE)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unittests/contracts DESTINATION ${CMAKE_INSTALL_FULL_DATAROOTDIR}/channel_testing/unittests COMPONENT dev EXCLUDE_FROM_ALL
        PATTERN "CMakeFiles" EXCLUDE
        PATTERN "*.cmake" EXCLUDE
        PATTERN "Makefile" EXCLUDE)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests/TestHarness DESTINATION ${CMAKE_INSTALL_FULL_DATAROOTDIR}/channel_testing/tests COMPONENT dev EXCLUDE_FROM_ALL
        FILES_MATCHING
        PATTERN "*.py"
        PATTERN "*.json"
        PATTERN "__pycache__" EXCLUDE
        PATTERN "CMakeFiles" EXCLUDE)

add_custom_target(link_target ALL
   COMMAND ${CMAKE_COMMAND} -E make_directory lib/python3/dist-packages
   COMMAND ${CMAKE_COMMAND} -E create_symlink ../../../share/channel_testing/tests/TestHarness lib/python3/dist-packages/TestHarness
   COMMAND ${CMAKE_COMMAND} -E make_directory share/channel_testing
   COMMAND ${CMAKE_COMMAND} -E create_symlink ../../bin share/channel_testing/bin)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib/python3/dist-packages/TestHarness DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/python3/dist-packages COMPONENT dev EXCLUDE_FROM_ALL)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/share/channel_testing/bin DESTINATION ${CMAKE_INSTALL_FULL_DATAROOTDIR}/channel_testing COMPONENT dev EXCLUDE_FROM_ALL)

configure_file(${CMAKE_SOURCE_DIR}/libraries/cli11/bash-completion/completions/channel-util
               ${CMAKE_BINARY_DIR}/programs/channel-util/bash-completion/completions/channel-util COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/libraries/cli11/bash-completion/completions/chan
               ${CMAKE_BINARY_DIR}/programs/chan/bash-completion/completions/chan COPYONLY)

install(FILES libraries/cli11/bash-completion/completions/channel-util DESTINATION ${CMAKE_INSTALL_FULL_DATAROOTDIR}/bash-completion/completions COMPONENT base)
install(FILES libraries/cli11/bash-completion/completions/chan DESTINATION ${CMAKE_INSTALL_FULL_DATAROOTDIR}/bash-completion/completions COMPONENT base)

# Add the boost submodule we used to build to our install package, so headers can be found for libtester
install(DIRECTORY "${CMAKE_SOURCE_DIR}/libraries/boost/"
        DESTINATION ${CMAKE_INSTALL_FULL_DATAROOTDIR}/spring_boost
        COMPONENT dev EXCLUDE_FROM_ALL
        PATTERN ".git/*" EXCLUDE
        PATTERN "example/*" EXCLUDE
        PATTERN "bench/*" EXCLUDE
        PATTERN "doc/*" EXCLUDE
        PATTERN "libs/*/test" EXCLUDE
        PATTERN "tools/*/test" EXCLUDE
        )

add_custom_target(dev-install
  COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}"
  COMMAND "${CMAKE_COMMAND}" --install "${CMAKE_BINARY_DIR}"
  COMMAND "${CMAKE_COMMAND}" --install "${CMAKE_BINARY_DIR}" --component dev
  USES_TERMINAL
)

include(doxygen)

option(ENABLE_CHANNEL_DEV_DEB "Enable building the channel-dev .deb package" OFF)

include(package.cmake)
include(CPack)
