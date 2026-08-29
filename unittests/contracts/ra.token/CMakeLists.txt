if( EOSIO_COMPILE_TEST_CONTRACTS )
   add_contract( ra.token ra.token ra.token.cpp )
else()
   configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ra.token.wasm ${CMAKE_CURRENT_BINARY_DIR}/ra.token.wasm COPYONLY )
   configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ra.token.abi  ${CMAKE_CURRENT_BINARY_DIR}/ra.token.abi  COPYONLY )
endif()
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ra.token.bad.abi  ${CMAKE_CURRENT_BINARY_DIR}/ra.token.bad.abi  COPYONLY )

