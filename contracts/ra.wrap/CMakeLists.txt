add_contract(ra.wrap ra.wrap ${CMAKE_CURRENT_SOURCE_DIR}/src/ra.wrap.cpp)

target_include_directories(ra.wrap
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(ra.wrap
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ricardian/ra.wrap.contracts.md.in ${CMAKE_CURRENT_BINARY_DIR}/ricardian/ra.wrap.contracts.md @ONLY )

target_compile_options( ra.wrap PUBLIC -R${CMAKE_CURRENT_SOURCE_DIR}/ricardian -R${CMAKE_CURRENT_BINARY_DIR}/ricardian )
