#pragma once
#include <fc/crypto/elliptic.hpp>
#include <fc/crypto/elliptic_r1.hpp>
#include <fc/crypto/elliptic_webauthn.hpp>
#include <fc/crypto/elliptic_em.hpp>
#include <fc/crypto/elliptic_ed.hpp>
#include <fc/crypto/signature.hpp>
#include <fc/reflect/reflect.hpp>
#include <fc/reflect/variant.hpp>
#include <fc/static_variant.hpp>

namespace fc {

namespace crypto {
   namespace config {
      constexpr const char* public_key_legacy_prefix = "RA";
      constexpr const char* public_key_base_prefix = "PUB";
      constexpr const char* public_key_prefix[] = {
         "K1",
         "R1",
         "WA",
         "EM",
         "ED"
      };
   };

   class public_key
   {
      public:
         using storage_type = std::variant<ecc::public_key_shim, r1::public_key_shim, webauthn::public_key,
                                           em::public_key_shim, ed::public_key_shim>;
         enum class key_type : uint8_t {
            k1 = fc::get_index<storage_type, ecc::public_key_shim>(),
            r1 = fc::get_index<storage_type, r1::public_key_shim>(),
            wa = fc::get_index<storage_type, webauthn::public_key>(),
            em = fc::get_index<storage_type, em::public_key_shim>(),
            ed = fc::get_index<storage_type, ed::public_key_shim>(),
            unknown
         };
         static_assert(std::variant_size_v<storage_type> == static_cast<uint8_t>(key_type::unknown), "Missing public_key key_type");
         static_assert(std::size(config::public_key_prefix) == static_cast<size_t>(key_type::unknown), "Missing public_key prefix");

         constexpr static const char* key_prefix(key_type t) { return config::public_key_prefix[static_cast<size_t>(t)]; };

         public_key() = default;
         public_key( public_key&& ) = default;
         public_key( const public_key& ) = default;
         public_key& operator= (const public_key& ) = default;

         /// Recover public key from a The Channel transaction signature
         static public_key recover( const signature& c, const sha256& digest );

         /// Convenience constructor equivalent to recover(c, digest)
         public_key( const signature& c, const sha256& digest );

         public_key( storage_type&& other_storage )
            :_storage(std::move(other_storage))
         {}

         bool valid()const;

         size_t which()const { return _storage.index(); }
         key_type type()const { return static_cast<key_type>(which()); }

         // serialize to/from string; if type is unknown, infer the key type from the string
         explicit public_key(const std::string& base58str);
         static public_key from_string(const std::string& str, key_type type = key_type::unknown);

         std::string to_legacy_string(const fc::yield_function_t& yield) const;

         /**
          * The Channel native string form.
          * For k1/r1/wa this is the base58check PUB_<TYPE>_ form; for em the
          * hex form (PUB_EM_ prefix included when include_prefix is true);
          * for ed the Solana-native base58 form (PUB_ED_ prefix included when
          * include_prefix is true).
          */
         std::string to_string(const fc::yield_function_t& yield, bool include_prefix = true) const;

         template<typename... Args>
         bool contains() const { return (std::holds_alternative<Args>(_storage) || ...); }

         template<typename... Args>
         bool contains_type(Args... types) const {
            static_assert((std::is_same_v<Args, key_type> && ...), "Args must be of type public_key::key_type");
            auto current_index = _storage.index();
            return ((current_index == static_cast<size_t>(types)) || ...);
         }

         template<typename T>
         const T& get() const { return std::get<T>(_storage); }

         const storage_type& storage() const { return _storage; }

         storage_type _storage;

      private:
         friend std::ostream& operator<<(std::ostream& s, const public_key& k);
         friend bool operator==( const public_key& p1, const public_key& p2);
         friend bool operator!=( const public_key& p1, const public_key& p2);
         friend bool operator<( const public_key& p1, const public_key& p2);
         friend struct reflector<public_key>;
         friend class private_key;
   }; // public_key

} }  // fc::crypto

namespace fc {
   void to_variant(const crypto::public_key& var, variant& vo, const fc::yield_function_t& yield = fc::yield_function_t());

   void from_variant(const variant& var, crypto::public_key& vo);
} // namespace fc

FC_REFLECT(fc::crypto::public_key, (_storage) )
FC_REFLECT_ENUM(fc::crypto::public_key::key_type, (k1)(r1)(wa)(em)(ed)(unknown))
