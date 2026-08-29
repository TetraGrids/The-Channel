add_contract(ra.org ra.org ${CMAKE_CURRENT_SOURCE_DIR}/src/ra.org.cpp)

target_include_directories(ra.org PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(ra.org
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
