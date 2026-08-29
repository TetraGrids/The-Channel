add_contract(ra.fees ra.fees ${CMAKE_CURRENT_SOURCE_DIR}/src/ra.fees.cpp)

target_include_directories(ra.fees PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include
   ${CMAKE_CURRENT_SOURCE_DIR}/../ra.system/include)

set_target_properties(ra.fees
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

target_compile_options( ra.fees PUBLIC )
