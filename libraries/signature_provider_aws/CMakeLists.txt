# Shared AWS SDK glue for the AWS-backed signature-provider plugins
# (signature_provider_kms_plugin / signature_provider_ssm_plugin).
# Requires the AWS C++ SDK; when it is not installed the AWS provider
# plugins are silently left out of the build.
find_package(aws-cpp-sdk-core CONFIG QUIET)

if(NOT aws-cpp-sdk-core_FOUND)
   message(STATUS "aws-cpp-sdk-core not found: AWS signature-provider support (KMS/SSM) disabled")
   return()
endif()

add_library(signature_provider_aws
            src/aws_common.cpp)

target_include_directories(signature_provider_aws PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(signature_provider_aws PUBLIC eosio_chain fc aws-cpp-sdk-core)
