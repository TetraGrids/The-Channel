add_contract(ra.claim ra.claim ${CMAKE_CURRENT_SOURCE_DIR}/src/ra.claim.cpp)

target_include_directories(ra.claim PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include
   ${CMAKE_CURRENT_SOURCE_DIR}/../ra.token/include)

set_target_properties(ra.claim
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
