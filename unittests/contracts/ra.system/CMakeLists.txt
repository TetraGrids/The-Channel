if( EOSIO_COMPILE_TEST_CONTRACTS )
   add_contract( ra.system ra.system ra.system.cpp peer_keys.cpp )
   target_include_directories(ra.system
           PUBLIC
           ${CMAKE_CURRENT_SOURCE_DIR}/include
           ${CMAKE_CURRENT_SOURCE_DIR}/../ra.token)
else()
   configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ra.system.wasm ${CMAKE_CURRENT_BINARY_DIR}/ra.system.wasm COPYONLY )
   configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ra.system.abi  ${CMAKE_CURRENT_BINARY_DIR}/ra.system.abi  COPYONLY )
endif()
