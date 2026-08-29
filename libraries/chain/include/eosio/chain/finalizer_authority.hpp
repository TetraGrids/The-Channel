#pragma once

#include <fc/crypto/bls_public_key.hpp>
#include <compare>
#include <string>

namespace eosio::chain {

   struct finalizer_authority {

      std::string  description;
      uint64_t     weight = 0; // weight that this finalizer's vote has for meeting fthreshold
      fc::crypto::blslib::bls_public_key  public_key;

      // libc++ before 15 lacks std::string's operator<=>, so spell the comparison out
      std::strong_ordering operator<=>(const finalizer_authority& rhs) const {
         if (int c = description.compare(rhs.description); c != 0)
            return c < 0 ? std::strong_ordering::less : std::strong_ordering::greater;
         if (auto c = weight <=> rhs.weight; c != 0) return c;
         return public_key <=> rhs.public_key;
      }
      bool operator==(const finalizer_authority& rhs) const {
         return description == rhs.description && weight == rhs.weight && public_key == rhs.public_key;
      }
   };

   using finalizer_authority_ptr = std::shared_ptr<const finalizer_authority>;

   // This is used by SHiP and Deepmind which require public keys in string format.
   struct finalizer_authority_with_string_key {
      std::string  description;
      uint64_t     weight = 0;
      std::string  public_key;

      finalizer_authority_with_string_key() = default;
      explicit finalizer_authority_with_string_key(const finalizer_authority& input)
         : description(input.description)
         , weight(input.weight)
         , public_key(input.public_key.to_string()) {}
   };
} /// eosio::chain

FC_REFLECT( eosio::chain::finalizer_authority, (description)(weight)(public_key) )
FC_REFLECT( eosio::chain::finalizer_authority_with_string_key, (description)(weight)(public_key) )
