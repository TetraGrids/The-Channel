#include <fc/crypto/signature.hpp>
#include <fc/crypto/common.hpp>
#include <fc/crypto/key_serdes.hpp>
#include <fc/exception/exception.hpp>

namespace fc { namespace crypto {
   struct hash_visitor : public fc::visitor<size_t> {
      template<typename SigType>
      size_t operator()(const SigType& sig) const {
         static_assert(sizeof(sig._data.data) == 65, "sig size is expected to be 65");
         //signatures are two bignums: r & s. Just add up least significant digits of the two
         return *(size_t*)&sig._data.data[32-sizeof(size_t)] + *(size_t*)&sig._data.data[64-sizeof(size_t)];
      }

      size_t operator()(const em::signature_shim& sig) const {
         static_assert(sizeof(sig._data) == 65, "sig size is expected to be 65");
         return *(size_t*)&sig._data[32-sizeof(size_t)] + *(size_t*)&sig._data[64-sizeof(size_t)];
      }

      size_t operator()(const ed::signature_shim& sig) const {
         return sig.get_hash();
      }

      size_t operator()(const webauthn::signature& sig) const {
         return sig.get_hash();
      }
   };

   template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
   template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

   signature signature::from_string(const std::string& str, sig_type type) {
      switch (type) {
      case sig_type::k1:
      case sig_type::r1:
      case sig_type::wa: {
         signature s(parse_unknown_signature_str(str));
         FC_ASSERT( s.type() == type, "Parsed type does not match specified type for ${str}", ("str", str) );
         return s;
      }
      case sig_type::em: {
         auto [base_prefix, type_prefix, data_str] = parse_base_prefixes(str);
         const auto& sig = base_prefix.empty() ? std::string_view(str) : data_str;
         FC_ASSERT(type_prefix.empty() || type_prefix == sig_prefix(sig_type::em), "Invalid signature prefixes: ${str}", ("str", str));
         return from_native_string_to_signature<chain_key_type_ethereum>(sig);
      }
      case sig_type::ed: {
         auto [base_prefix, type_prefix, data_str] = parse_base_prefixes(str);
         const auto& sig = base_prefix.empty() ? std::string_view(str) : data_str;
         FC_ASSERT(type_prefix.empty() || type_prefix == sig_prefix(sig_type::ed), "Invalid signature prefixes: ${str}", ("str", str));
         return from_native_string_to_signature<chain_key_type_solana>(sig);
      }
      case sig_type::unknown: {
         auto [base_prefix, type_prefix, data_str] = parse_base_prefixes(str);
         FC_ASSERT(base_prefix == config::signature_base_prefix, "Invalid prefix to parse signature type: ${str}", ("str", str));
         if (type_prefix == sig_prefix(sig_type::em)) {
            return from_native_string_to_signature<chain_key_type_ethereum>(data_str);
         } else if (type_prefix == sig_prefix(sig_type::ed)) {
            return from_native_string_to_signature<chain_key_type_solana>(data_str);
         }
         return signature(parse_unknown_signature_str(str));
      }

      default:
         FC_ASSERT(false, "Unknown signature type");
      };
   }

   signature::signature(const std::string& base58str)
      :_storage(parse_unknown_signature_str(base58str))
   {}

   size_t signature::variable_size() const {
      return std::visit(overloaded {
         [&](const auto& k1r1) {
            return static_cast<size_t>(0);
         },
         [&](const webauthn::signature& wa) {
            return static_cast<size_t>(wa.variable_size());
         }
      }, _storage);
   }

   bool signature::is_canonical() const {
      return std::visit(overloaded {
         [&](const ecc::signature_shim& s) {
            return ecc::public_key::is_canonical(s._data);
         },
         [&](const r1::signature_shim& s) {
            return r1::public_key::is_canonical(s._data);
         },
         [&](const webauthn::signature& wa) {
            return true;
         },
         [&](const em::signature_shim& s) {
            // EM signatures enforce low-s canonical form at signing time
            return true;
         },
         [&](const ed::signature_shim& s) {
            return true;
         }
      }, _storage);
   }

   std::string signature::to_string(const fc::yield_function_t& yield) const
   {
      switch (type()) {
      case sig_type::k1:
      case sig_type::r1:
      case sig_type::wa: {
         auto data_str = std::visit(base58str_visitor<storage_type, config::signature_prefix>(yield), _storage);
         yield();
         return std::string(config::signature_base_prefix) + "_" + data_str;
      }
      case sig_type::em: {
         return std::string(config::signature_base_prefix) + "_" + sig_prefix(sig_type::em) + "_"
                + get<em::signature_shim>().to_string();
      }
      case sig_type::ed: {
         return std::string(config::signature_base_prefix) + "_" + sig_prefix(sig_type::ed) + "_"
                + get<ed::signature_shim>().to_string(yield);
      }
      case sig_type::unknown:
         break;
      }

      FC_ASSERT(false, "signature unknown sig type");
   }

   std::ostream& operator<<(std::ostream& s, const signature& k) {
      s << "signature(" << k.to_string() << ')';
      return s;
   }

   bool operator == ( const signature& p1, const signature& p2) {
      return eq_comparator<signature::storage_type>::apply(p1._storage, p2._storage);
   }

   bool operator != ( const signature& p1, const signature& p2) {
      return !eq_comparator<signature::storage_type>::apply(p1._storage, p2._storage);
   }

   bool operator < ( const signature& p1, const signature& p2)
   {
      return less_comparator<signature::storage_type>::apply(p1._storage, p2._storage);
   }

   size_t hash_value(const signature& b) {
       return std::visit(hash_visitor(), b._storage);
   }
} } // fc::crypto

namespace fc
{
   void to_variant(const fc::crypto::signature& var, fc::variant& vo, const fc::yield_function_t& yield)
   {
      vo = var.to_string(yield);
   }

   void from_variant(const fc::variant& var, fc::crypto::signature& vo)
   {
      vo = fc::crypto::signature::from_string(var.as_string());
   }
} // fc
