#include <fc/crypto/public_key.hpp>
#include <fc/crypto/common.hpp>
#include <fc/crypto/key_serdes.hpp>
#include <fc/exception/exception.hpp>

namespace fc::crypto {

   public_key public_key::recover( const signature& c, const sha256& digest ) {
      return public_key(std::visit([&](const auto& s) { return public_key::storage_type(s.recover(digest)); }, c._storage));
   }

   public_key::public_key(const signature& c, const sha256& digest)
      : public_key(recover(c, digest)) {}

   public_key public_key::from_string(const std::string& str, key_type type) {
      switch (type) {
      case key_type::k1:
      case key_type::r1:
      case key_type::wa: {
         public_key k(parse_unknown_public_key_str(str));
         FC_ASSERT( k.type() == type, "Parsed type does not match specified type for ${str}", ("str", str));
         return k;
      }
      case key_type::em: {
         auto [base_prefix, type_prefix, data_str] = parse_base_prefixes(str);
         const auto& key = base_prefix.empty() ? std::string_view(str) : data_str;
         FC_ASSERT(type_prefix.empty() || type_prefix == key_prefix(key_type::em), "Invalid public key prefixes: ${str}", ("str", str));
         return from_native_string_to_public_key<chain_key_type_ethereum>(key);
      }
      case key_type::ed: {
         auto [base_prefix, type_prefix, data_str] = parse_base_prefixes(str);
         const auto& key = base_prefix.empty() ? std::string_view(str) : data_str;
         FC_ASSERT(type_prefix.empty() || type_prefix == key_prefix(key_type::ed), "Invalid public key prefixes: ${str}", ("str", str));
         return from_native_string_to_public_key<chain_key_type_solana>(key);
      }
      case key_type::unknown: {
         if (is_legacy_public_key_str(str))
            return public_key(parse_unknown_public_key_str(str));

         auto [base_prefix, type_prefix, data_str] = parse_base_prefixes(str);
         FC_ASSERT(base_prefix == config::public_key_base_prefix, "Invalid prefix to parse key type: ${str}", ("str", str));
         if (type_prefix == key_prefix(key_type::em)) {
            return from_native_string_to_public_key<chain_key_type_ethereum>(data_str);
         } else if (type_prefix == key_prefix(key_type::ed)) {
            return from_native_string_to_public_key<chain_key_type_solana>(data_str);
         }
         return public_key(parse_unknown_public_key_str(str));
      }

      default:
         FC_ASSERT(false, "Unknown key type");
      };
   }

   public_key::public_key(const std::string& base58str)
   :_storage(parse_unknown_public_key_str(base58str))
   {}

   bool public_key::valid()const
   {
      return std::visit([](const auto& key) { return key.valid(); }, _storage);
   }

   // output in pre The Channel-2.0 format: RA, PUB_R1, PUB_WA (em/ed have no legacy form)
   std::string public_key::to_legacy_string(const fc::yield_function_t& yield) const
   {
      auto which = _storage.index();
      if (which >= static_cast<size_t>(key_type::em)) {
         return to_string(yield);
      }

      auto data_str = std::visit(base58str_visitor<storage_type, config::public_key_prefix, 0>(yield), _storage);

      if (which == 0) {
         return std::string(config::public_key_legacy_prefix) + data_str;
      } else {
         return std::string(config::public_key_base_prefix) + "_" + data_str;
      }
   }

   // The Channel 2.0+ native form: PUB_K1, PUB_R1, PUB_WA, PUB_EM (hex), PUB_ED (base58)
   std::string public_key::to_string(const fc::yield_function_t& yield, bool include_prefix) const
   {
      switch (type()) {
      case key_type::k1:
      case key_type::r1:
      case key_type::wa: {
         auto data_str = std::visit(base58str_visitor<storage_type, config::public_key_prefix>(yield), _storage);
         return std::string(config::public_key_base_prefix) + "_" + data_str;
      }
      case key_type::em: {
         std::string prefix = include_prefix
                                 ? std::string(config::public_key_base_prefix) + "_" + key_prefix(key_type::em) + "_"
                                 : "";
         return prefix + get<em::public_key_shim>().to_string(false); // keep 0x for consistency with PVT_EM_/SIG_EM_
      }
      case key_type::ed: {
         std::string prefix = include_prefix
                                 ? std::string(config::public_key_base_prefix) + "_" + key_prefix(key_type::ed) + "_"
                                 : "";
         return prefix + get<ed::public_key_shim>().to_string(yield);
      }
      case key_type::unknown:
         break;
      }

      FC_ASSERT(false, "public_key unknown key type");
   }

   std::ostream& operator<<(std::ostream& s, const public_key& k) {
      s << "public_key(" << k.to_string({}) << ')';
      return s;
   }

   bool operator==( const public_key& p1, const public_key& p2) {
      return eq_comparator<public_key::storage_type>::apply(p1._storage, p2._storage);
   }

   bool operator!=( const public_key& p1, const public_key& p2) {
      return !(p1 == p2);
   }

   bool operator<( const public_key& p1, const public_key& p2)
   {
      return less_comparator<public_key::storage_type>::apply(p1._storage, p2._storage);
   }
} // fc::crypto

namespace fc
{
   using namespace std;
   void to_variant(const fc::crypto::public_key& var, fc::variant& vo, const fc::yield_function_t& yield)
   {
      vo = var.to_string(yield);
   }

   void from_variant(const fc::variant& var, fc::crypto::public_key& vo)
   {
      vo = fc::crypto::public_key::from_string(var.as_string());
   }
} // fc
