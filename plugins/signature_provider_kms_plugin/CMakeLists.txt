# AWS KMS-backed signature-provider plugin: supplies the `KMS:<key-ref>`
# scheme (remote Sign call per signature; the key never leaves AWS KMS;
# secp256k1/ethereum keys only). Built only when the AWS C++ SDK is installed.
find_package(aws-cpp-sdk-kms CONFIG QUIET)

if(NOT aws-cpp-sdk-kms_FOUND)
   message(STATUS "aws-cpp-sdk-kms not found: signature_provider_kms_plugin disabled")
   return()
endif()

if(NOT TARGET signature_provider_aws)
   message(STATUS "signature_provider_aws unavailable: signature_provider_kms_plugin disabled")
   return()
endif()

file(GLOB HEADERS "include/eosio/signature_provider_kms_plugin/*.hpp")
add_library( signature_provider_kms_plugin
             src/signature_provider_kms_plugin.cpp
             src/kms_signature_provider.cpp
             ${HEADERS} )

target_link_libraries( signature_provider_kms_plugin
                       signature_provider_manager_plugin signature_provider_aws
                       secp256k1 aws-cpp-sdk-kms )
target_include_directories( signature_provider_kms_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )
