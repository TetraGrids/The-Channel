add_contract(ra.boot ra.boot ${CMAKE_CURRENT_SOURCE_DIR}/src/ra.boot.cpp)

target_include_directories(ra.boot
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(ra.boot
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ricardian/ra.boot.contracts.md.in ${CMAKE_CURRENT_BINARY_DIR}/ricardian/ra.boot.contracts.md @ONLY )

target_compile_options( ra.boot PUBLIC -R${CMAKE_CURRENT_SOURCE_DIR}/ricardian -R${CMAKE_CURRENT_BINARY_DIR}/ricardian )
