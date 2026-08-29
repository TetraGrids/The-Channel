file(GLOB HEADERS "include/eosio/signature_provider_manager_plugin/*.hpp")
add_library( signature_provider_manager_plugin
             src/signature_provider_manager_plugin.cpp
             ${HEADERS} )

target_link_libraries( signature_provider_manager_plugin chain_plugin http_client_plugin appbase fc )
target_include_directories( signature_provider_manager_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )
