cmake_minimum_required( VERSION 3.5 )
message(STATUS "Setting up Eosio Tester 1.0.0-dev1 at /Users/fresh/Dropbox/htdocs/The-Channel/install/lib")

SET(CMAKE_INSTALL_RPATH "${HOME}/lib;${HOME}/lib64")

set(CMAKE_CXX_COMPILER /usr/bin/c++)
set(CMAKE_C_COMPILER   /usr/bin/cc)

set(EOSIO_VERSION "1.0.0-dev1")

enable_testing()

if (LLVM_DIR STREQUAL "" OR NOT LLVM_DIR)
   set(LLVM_DIR )
endif()

if(NOT "" STREQUAL "")
   find_package(LLVM  EXACT REQUIRED CONFIG)
   llvm_map_components_to_libnames(LLVM_LIBS support core passes mcjit native DebugInfoDWARF orcjit)
   link_directories(${LLVM_LIBRARY_DIR})
endif()

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_EXTENSIONS ON )
set( CXX_STANDARD_REQUIRED ON )

#adds -pthread. Ubuntu eosio.contracts build breaks without this flag specified
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads)

if ( APPLE )
   set( CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS} -Wall -Wno-deprecated-declarations" )
else ( APPLE )
   set( CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS} -Wall")
endif ( APPLE )

add_compile_definitions(BOOST_TEST_DEFAULTS_TO_CORE_DUMP)
add_compile_definitions(BOOST_UNORDERED_DISABLE_NEON)
add_compile_definitions(XXH_INLINE_ALL=1)

set( Boost_USE_MULTITHREADED      ON )
set( Boost_USE_STATIC_LIBS ON CACHE STRING "ON or OFF" )
set( BOOST_EXCLUDE_LIBRARIES "mysql" )

add_subdirectory( /Users/fresh/Dropbox/htdocs/The-Channel/install/share/spring_boost ${PROJECT_BINARY_DIR}/libraries/boost EXCLUDE_FROM_ALL)

find_library(libtester eosio_testing /Users/fresh/Dropbox/htdocs/The-Channel/install/lib NO_DEFAULT_PATH)
find_library(libchain eosio_chain /Users/fresh/Dropbox/htdocs/The-Channel/install/lib NO_DEFAULT_PATH)
find_library(libfc fc /Users/fresh/Dropbox/htdocs/The-Channel/install/lib NO_DEFAULT_PATH)
find_library(libsecp256k1 secp256k1 /Users/fresh/Dropbox/htdocs/The-Channel/install/lib NO_DEFAULT_PATH)
find_library(libbn256 bn256 /Users/fresh/Dropbox/htdocs/The-Channel/install/lib NO_DEFAULT_PATH)
find_library(libbls12-381 bls12-381 /Users/fresh/Dropbox/htdocs/The-Channel/install/lib NO_DEFAULT_PATH)

find_library(libwasm WASM /Users/fresh/Dropbox/htdocs/The-Channel/install/lib NO_DEFAULT_PATH)
find_library(libwast WAST /Users/fresh/Dropbox/htdocs/The-Channel/install/lib NO_DEFAULT_PATH)
find_library(libir IR     /Users/fresh/Dropbox/htdocs/The-Channel/install/lib NO_DEFAULT_PATH)
find_library(liblogging Logging /Users/fresh/Dropbox/htdocs/The-Channel/install/lib NO_DEFAULT_PATH)
find_library(libsoftfloat softfloat /Users/fresh/Dropbox/htdocs/The-Channel/install/lib NO_DEFAULT_PATH)
find_library(libbscrypto bscrypto /Users/fresh/Dropbox/htdocs/The-Channel/install/lib NO_DEFAULT_PATH)
find_library(libdecrepit decrepit /Users/fresh/Dropbox/htdocs/The-Channel/install/lib NO_DEFAULT_PATH)
find_library(libchainbase chainbase /Users/fresh/Dropbox/htdocs/The-Channel/install/lib NO_DEFAULT_PATH)
find_library(libbuiltins builtins /Users/fresh/Dropbox/htdocs/The-Channel/install/lib NO_DEFAULT_PATH)

#Ubuntu build requires rt library to be specified explicitly
if(UNIX AND NOT APPLE)
  find_library(LIBRT rt)
endif()

set(EOSIO_WASM_RUNTIMES eos-vm;eos-vm-jit)
if("eos-vm-oc" IN_LIST EOSIO_WASM_RUNTIMES)
   set(WRAP_MAIN "-Wl,-wrap=main")
endif()

add_library(EosioChain INTERFACE)

target_link_libraries(EosioChain INTERFACE
   ${libchain}
   ${libfc}
   ${libwast}
   ${libwasm}
   ${libir}
   ${libsoftfloat}
   ${libbscrypto}
   ${libdecrepit}
   ${liblogging}
   ${libchainbase}
   ${libbuiltins}
   ${libsecp256k1}
   ${libbn256}
   ${libbls12-381}
   /usr/local/lib/libgmp.dylib

   Boost::date_time
   Boost::filesystem
   Boost::system
   Boost::chrono
   Boost::multi_index
   Boost::multiprecision
   Boost::interprocess
   Boost::asio
   Boost::beast
   Boost::crc
   Boost::signals2
   Boost::iostreams
   "-lz" # Needed by Boost iostreams

   ${LLVM_LIBS}

   ${PLATFORM_SPECIFIC_LIBS}

   ${WRAP_MAIN}
   Threads::Threads
)

target_include_directories(EosioChain INTERFACE
                              
                              /Users/fresh/Dropbox/htdocs/The-Channel/install
                              /Users/fresh/Dropbox/htdocs/The-Channel/install/include
                              /Users/fresh/Dropbox/htdocs/The-Channel/install/include/wasm-jit
                              /Users/fresh/Dropbox/htdocs/The-Channel/install/include/springboringssl
                              /Users/fresh/Dropbox/htdocs/The-Channel/install/include/springxxhash
                              /Users/fresh/Dropbox/htdocs/The-Channel/install/include/softfloat )

#adds -lrt and -latomic. Ubuntu eosio.contracts build breaks without this
if(UNIX AND NOT APPLE)
   target_link_libraries(EosioChain INTERFACE ${LIBRT} -latomic)
endif()

add_library(EosioTester INTERFACE)

target_link_libraries(EosioTester INTERFACE
   ${libtester}
   Boost::unit_test_framework
   EosioChain
)

macro(add_eosio_test_executable test_name)
   add_executable( ${test_name} ${ARGN} )
   target_link_libraries( ${test_name}
       EosioTester
      )

endmacro()

macro(add_eosio_test test_name)
   add_eosio_test_executable( ${test_name} ${ARGN} )
   #This will generate a test with the default runtime
   add_test(NAME ${test_name} COMMAND ${test_name} --report_level=detailed --color_output)

   #Manually run unit_test for all supported runtimes
   #To run unit_test with all log from blockchain displayed, put --verbose after --, i.e. unit_test -- --verbose
endmacro()
