if(NOT EOSIO_ROOT)
   list(APPEND CMAKE_MODULE_PATH /Users/fresh/Dropbox/htdocs/The-Channel/build-verify/lib/cmake/eosio)
else()
   # legacy behavior for anyone configuring with -DEOSIO_ROOT=/foo or such
   list(APPEND CMAKE_MODULE_PATH ${EOSIO_ROOT}/lib/cmake/eosio)
   list(APPEND CMAKE_MODULE_PATH ${EOSIO_ROOT}/lib64/cmake/eosio)
endif()
include(EosioTester)
include(EosioCheckVersion)
