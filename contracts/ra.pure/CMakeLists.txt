add_contract(flex.token flex.token ${CMAKE_CURRENT_SOURCE_DIR}/flex.token.cpp)

target_include_directories(flex.token PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR}/alcor-exchange)

set_target_properties(flex.token
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
