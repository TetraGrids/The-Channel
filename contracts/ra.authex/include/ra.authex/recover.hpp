#pragma once

#include <eosio/crypto.hpp>
#include <eosio/eosio.hpp>

#include <string>
#include <vector>

// CDT 4.1.1's eosio::signature has no EM/ED alternatives. Call the host
// recover_key with fc-packed signature bytes (variant index + payload).
extern "C" {
   __attribute__((eosio_wasm_import))
   int32_t recover_key( const void* digest, const char* sig, uint32_t siglen, char* pub, uint32_t publen );
}

namespace eosio {

inline checksum256 sha256_msg( const std::string& msg ) {
   return sha256( msg.data(), static_cast<uint32_t>( msg.size() ) );
}

inline std::vector<char> recover_packed_key( const checksum256& digest, const std::vector<char>& sig ) {
   check( sig.size() >= 8 && sig.size() <= 256, "invalid signature size" );
   std::vector<char> pub( 128, 0 );
   const auto bytes = digest.extract_as_byte_array();
   const int32_t n = ::recover_key( bytes.data(), sig.data(), static_cast<uint32_t>( sig.size() ),
                                    pub.data(), static_cast<uint32_t>( pub.size() ) );
   check( n > 0 && static_cast<uint32_t>( n ) <= pub.size(), "recover_key failed" );
   pub.resize( static_cast<uint32_t>( n ) );
   return pub;
}

} // namespace eosio
