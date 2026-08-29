add_contract(ra.bpay ra.bpay ${CMAKE_CURRENT_SOURCE_DIR}/src/ra.bpay.cpp)

target_include_directories(ra.bpay PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include
   ${CMAKE_CURRENT_SOURCE_DIR}/../ra.system/include
   ${CMAKE_CURRENT_SOURCE_DIR}/../ra.token/include)

set_target_properties(ra.bpay
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ricardian/ra.bpay.contracts.md.in ${CMAKE_CURRENT_BINARY_DIR}/ricardian/ra.bpay.contracts.md @ONLY )

target_compile_options( ra.bpay PUBLIC -R${CMAKE_CURRENT_SOURCE_DIR}/ricardian -R${CMAKE_CURRENT_BINARY_DIR}/ricardian )
