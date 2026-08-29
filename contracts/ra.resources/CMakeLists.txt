add_contract(ra.resources ra.resources ${CMAKE_CURRENT_SOURCE_DIR}/src/ra.resources.cpp)

target_include_directories(ra.resources PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include
   ${CMAKE_CURRENT_SOURCE_DIR}/../ra.system/include
   ${CMAKE_CURRENT_SOURCE_DIR}/../ra.token/include)

set_target_properties(ra.resources
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
