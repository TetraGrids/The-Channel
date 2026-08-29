add_contract(ra.authex ra.authex ${CMAKE_CURRENT_SOURCE_DIR}/src/ra.authex.cpp)

target_include_directories(ra.authex PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(ra.authex
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
