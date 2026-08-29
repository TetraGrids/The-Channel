add_contract(ra.bios ra.bios ${CMAKE_CURRENT_SOURCE_DIR}/src/ra.bios.cpp)

target_include_directories(ra.bios
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(ra.bios
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ricardian/ra.bios.contracts.md.in ${CMAKE_CURRENT_BINARY_DIR}/ricardian/ra.bios.contracts.md @ONLY )

target_compile_options( ra.bios PUBLIC -R${CMAKE_CURRENT_SOURCE_DIR}/ricardian -R${CMAKE_CURRENT_BINARY_DIR}/ricardian )
