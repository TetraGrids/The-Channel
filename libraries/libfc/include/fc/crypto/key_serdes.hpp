#pragma once

#include <fc/crypto/common.hpp>
#include <fc/crypto/chain_types.hpp>
#include <fc/crypto/elliptic_em.hpp>
#include <fc/crypto/elliptic_ed.hpp>
#include <fc/crypto/ethereum/ethereum_utils.hpp>
#include <fc/crypto/private_key.hpp>
#include <fc/crypto/public_key.hpp>
#include <fc/crypto/signature.hpp>
#include <fc/exception/exception.hpp>
#include <fc/utility.hpp>

namespace fc::crypto {

// Parses the base and type prefixes from a key string.
// Returns a tuple of the base prefix, type prefix, and the remaining data string.
// For example, "PVT_K1_abc123" -> ("PVT", "K1", "abc123"), "PVT_EM_0x1234abcd" -> ("PVT", "EM", "0x1234abcd")
// If the string does not contain any underscores, returns ("", "", original_string).
inline std::tuple<std::string_view, std::string_view, std::string_view> parse_base_prefixes(std::string_view str) {
   const auto pivot = str.find('_');
   if (pivot == std::string::npos)
      return {std::string_view{}, std::string_view{}, str};

   const auto pivot2 = str.find('_', pivot + 1);
   if (pivot2 == std::string::npos)
      return {std::string_view{}, std::string_view{}, str};

   return {str.substr(0, pivot), str.substr(pivot + 1, pivot2 - pivot - 1), str.substr(pivot2 + 1)};
}

// Converts a key to a WIF string.
template <typename Data>
std::string to_wif(const Data& secret, const yield_function_t& yield) {
   const size_t size_of_data_to_hash = sizeof(typename Data::data_type) + 1;
   const size_t size_of_hash_bytes = 4;
   char data[size_of_data_to_hash + size_of_hash_bytes];
   data[0] = (char)0x80; // this is the Bitcoin MainNet code
   memcpy(&data[1], (const char*)&secret.serialize(), sizeof(typename Data::data_type));
   sha256 digest = sha256::hash(data, size_of_data_to_hash);
   digest = sha256::hash(digest);
   memcpy(data + size_of_data_to_hash, (char*)&digest, size_of_hash_bytes);
   return to_base58(data, sizeof(data), yield);
}

// Converts a WIF string back to a key.
template <typename Data>
Data from_wif(const std::string& wif_key) {
   auto wif_bytes = from_base58(wif_key);
   FC_ASSERT(wif_bytes.size() >= 5);
   auto key_bytes = std::vector<char>(wif_bytes.begin() + 1, wif_bytes.end() - 4);
   fc::sha256 check = fc::sha256::hash(wif_bytes.data(), wif_bytes.size() - 4);
   fc::sha256 check2 = fc::sha256::hash(check);

   FC_ASSERT(memcmp((char*)&check, wif_bytes.data() + wif_bytes.size() - 4, 4) == 0 ||
                memcmp((char*)&check2, wif_bytes.data() + wif_bytes.size() - 4, 4) == 0);

   return Data(fc::variant(key_bytes).as<typename Data::data_type>());
}

inline bool is_legacy_public_key_str(std::string_view str) {
   constexpr auto legacy_prefix = config::public_key_legacy_prefix;
   return prefix_matches(legacy_prefix, str) && str.find('_') == std::string::npos;
}

// Parses a The Channel public key string (legacy "RA..." or "PUB_<TYPE>_...") into storage.
inline public_key::storage_type parse_unknown_public_key_str(std::string_view str) {
   if (is_legacy_public_key_str(str)) {
      auto sub_str = str.substr(const_strlen(config::public_key_legacy_prefix));
      using default_type = std::variant_alternative_t<0, public_key::storage_type>;
      using data_type = default_type::data_type;
      using wrapper = checksummed_data<data_type>;
      auto bin = fc::from_base58(std::string(sub_str));
      FC_ASSERT(bin.size() == sizeof(data_type) + sizeof(uint32_t), "Legacy public key has invalid size");
      auto wrapped = fc::raw::unpack<wrapper>(bin);
      FC_ASSERT(wrapper::calculate_checksum(wrapped.data) == wrapped.check, "Legacy public key checksum mismatch");
      return public_key::storage_type(default_type(wrapped.data));
   }

   constexpr auto prefix = config::public_key_base_prefix;
   const auto pivot = str.find('_');
   FC_ASSERT(pivot != std::string::npos, "No delimiter in string, cannot determine type: ${str}", ("str", std::string(str)));

   const auto prefix_str = str.substr(0, pivot);
   FC_ASSERT(prefix == prefix_str, "Public Key has invalid prefix: ${str}", ("str", std::string(str))("prefix_str", std::string(prefix_str)));

   auto data_str = str.substr(pivot + 1);
   FC_ASSERT(!data_str.empty(), "Public Key has no data: ${str}", ("str", std::string(str)));
   return base58_str_parser<public_key::storage_type, config::public_key_prefix>::apply(std::string(data_str));
}

// Parses a The Channel private key string (WIF or "PVT_<TYPE>_...") into storage.
inline private_key::storage_type parse_unknown_private_key_str(std::string_view str) {
   if (str.find('_') == std::string::npos) {
      // wif import
      return private_key::storage_type(from_wif<ecc::private_key_shim>(std::string(str)));
   }

   constexpr auto prefix = config::private_key_base_prefix;
   const auto pivot = str.find('_');
   const auto prefix_str = str.substr(0, pivot);
   FC_ASSERT(prefix == prefix_str, "Private Key has invalid prefix: ${str}", ("str", std::string(str))("prefix_str", std::string(prefix_str)));

   auto data_str = str.substr(pivot + 1);
   FC_ASSERT(!data_str.empty(), "Private Key has no data: ${str}", ("str", std::string(str)));
   return base58_str_parser<private_key::storage_type, config::private_key_prefix>::apply(std::string(data_str));
}

// Parses a The Channel signature string ("SIG_<TYPE>_...") into storage.
inline signature::storage_type parse_unknown_signature_str(std::string_view str) {
   constexpr auto prefix = config::signature_base_prefix;
   const auto pivot = str.find('_');
   FC_ASSERT(pivot != std::string::npos, "No delimiter in string, cannot determine type: ${str}", ("str", std::string(str)));

   const auto prefix_str = str.substr(0, pivot);
   FC_ASSERT(prefix == prefix_str, "Signature has invalid prefix: ${str}", ("str", std::string(str))("prefix_str", std::string(prefix_str)));

   auto data_str = str.substr(pivot + 1);
   FC_ASSERT(!data_str.empty(), "Signature has no data: ${str}", ("str", std::string(str)));
   return base58_str_parser<signature::storage_type, config::signature_prefix>::apply(std::string(data_str));
}

// Converts a native string representation of a key to a public key.
template <chain_key_type_t ChainKeyType>
public_key from_native_string_to_public_key(std::string_view str) {
   FC_ASSERT(!str.empty(), "Public key string cannot be empty");
   if constexpr (ChainKeyType == chain_key_type_ethereum) {
      return public_key(public_key::storage_type(em::public_key_shim(em::public_key::from_string(std::string(str)).serialize())));
   } else if constexpr (ChainKeyType == chain_key_type_solana) {
      return public_key(public_key::storage_type(ed::public_key_shim::from_base58_string(std::string(str))));
   } else {
      FC_ASSERT(false, "Unsupported chain key type for public key string parsing");
   }
}

// Converts a native string representation of a key to a private key.
template <chain_key_type_t ChainKeyType>
private_key from_native_string_to_private_key(std::string_view str) {
   FC_ASSERT(!str.empty(), "Private key string cannot be empty");
   if constexpr (ChainKeyType == chain_key_type_ethereum) {
      return private_key(private_key::storage_type(em::private_key_shim(em::private_key::from_native_string(std::string(str)).get_secret())));
   } else if constexpr (ChainKeyType == chain_key_type_solana) {
      return private_key(private_key::storage_type(ed::private_key_shim::from_base58_string(std::string(str))));
   } else {
      FC_ASSERT(false, "Unsupported chain key type for private key string parsing");
   }
}

// Converts a native string representation of a signature to a signature object.
template <chain_key_type_t ChainKeyType>
signature from_native_string_to_signature(std::string_view str) {
   FC_ASSERT(!str.empty(), "Signature string cannot be empty");
   if constexpr (ChainKeyType == chain_key_type_ethereum) {
      return signature(signature::storage_type(em::signature_shim(ethereum::to_em_signature(std::string(str)))));
   } else if constexpr (ChainKeyType == chain_key_type_solana) {
      return signature(signature::storage_type(ed::signature_shim::from_base58_string(std::string(str))));
   } else {
      FC_ASSERT(false, "Unsupported chain key type for signature string parsing");
   }
}

} // namespace fc::crypto
