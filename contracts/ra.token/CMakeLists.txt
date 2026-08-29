add_contract(ra.token ra.token ${CMAKE_CURRENT_SOURCE_DIR}/src/ra.token.cpp)

target_include_directories(ra.token
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(ra.token
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ricardian/ra.token.contracts.md.in ${CMAKE_CURRENT_BINARY_DIR}/ricardian/ra.token.contracts.md @ONLY )

target_compile_options( ra.token PUBLIC -R${CMAKE_CURRENT_SOURCE_DIR}/ricardian -R${CMAKE_CURRENT_BINARY_DIR}/ricardian )
