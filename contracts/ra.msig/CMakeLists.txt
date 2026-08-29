add_contract(ra.msig ra.msig ${CMAKE_CURRENT_SOURCE_DIR}/src/ra.msig.cpp)

target_include_directories(ra.msig
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(ra.msig
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ricardian/ra.msig.contracts.md.in ${CMAKE_CURRENT_BINARY_DIR}/ricardian/ra.msig.contracts.md @ONLY )

target_compile_options( ra.msig PUBLIC -R${CMAKE_CURRENT_SOURCE_DIR}/ricardian -R${CMAKE_CURRENT_BINARY_DIR}/ricardian )
