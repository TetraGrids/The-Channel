# AWS SSM-backed signature-provider plugin: supplies the `SSM:<param-ref>`
# scheme (private key fetched once from SSM Parameter Store at startup, then
# signs locally). Built only when the AWS C++ SDK is installed.
find_package(aws-cpp-sdk-ssm CONFIG QUIET)

if(NOT aws-cpp-sdk-ssm_FOUND)
   message(STATUS "aws-cpp-sdk-ssm not found: signature_provider_ssm_plugin disabled")
   return()
endif()

if(NOT TARGET signature_provider_aws)
   message(STATUS "signature_provider_aws unavailable: signature_provider_ssm_plugin disabled")
   return()
endif()

file(GLOB HEADERS "include/eosio/signature_provider_ssm_plugin/*.hpp")
add_library( signature_provider_ssm_plugin
             src/signature_provider_ssm_plugin.cpp
             src/ssm_signature_provider.cpp
             ${HEADERS} )

target_link_libraries( signature_provider_ssm_plugin
                       signature_provider_manager_plugin signature_provider_aws
                       aws-cpp-sdk-ssm )
target_include_directories( signature_provider_ssm_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )
