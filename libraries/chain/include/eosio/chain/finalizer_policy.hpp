#pragma once

#include <eosio/chain/types.hpp>
#include <compare>
#include <eosio/chain/finalizer_authority.hpp>
#include <fc/container/ordered_diff.hpp>

namespace eosio::chain {

   using finalizers_differ = fc::ordered_diff<finalizer_authority, uint16_t>;
   // Verify finalizers_differ::size_type can represent all index values in the
   // diff between two policies that could each hold up to max_finalizers entries.
   static_assert(std::numeric_limits<finalizers_differ::size_type>::max() >= config::max_finalizers - 1);
   using finalizers_diff_t = finalizers_differ::diff_result;

   struct finalizer_policy_diff {
      uint32_t                         generation = 0; ///< sequentially incrementing version number
      uint64_t                         threshold = 0;  ///< vote weight threshold to finalize blocks
      finalizers_diff_t                finalizers_diff;
   };

   struct finalizer_policy {
      uint32_t                         generation = 0; ///< sequentially incrementing version number
      uint64_t                         threshold = 0;  ///< vote weight threshold to finalize blocks
      std::vector<finalizer_authority> finalizers;     ///< Instant Finality voter set

      finalizer_policy_diff create_diff(const finalizer_policy& target) const {
         return {.generation = target.generation,
                 .threshold = target.threshold,
                 .finalizers_diff = finalizers_differ::diff(finalizers, target.finalizers)};
      }

      template <typename X>
      requires std::same_as<std::decay_t<X>, finalizer_policy_diff>
      [[nodiscard]] finalizer_policy apply_diff(X&& diff) const {
         finalizer_policy result;
         result.generation = diff.generation;
         result.threshold = diff.threshold;
         auto copy = finalizers;
         result.finalizers = finalizers_differ::apply_diff(std::move(copy), std::forward<X>(diff).finalizers_diff);
         return result;
      }

      // max accumulated weak weight before becoming weak_final
      uint64_t max_weak_sum_before_weak_final() const {
         uint64_t sum = std::accumulate( finalizers.begin(), finalizers.end(), 0,
            [](uint64_t acc, const finalizer_authority& f) {
               return acc + f.weight;
            }
         );

         return (sum - threshold);
      }

      // libc++ before 15 lacks std::vector's operator<=>, so spell the comparison out
      // (lexicographical over generation, threshold, then finalizers)
      std::strong_ordering operator<=>(const finalizer_policy& rhs) const {
         if (auto c = generation <=> rhs.generation; c != 0) return c;
         if (auto c = threshold <=> rhs.threshold; c != 0) return c;
         if (finalizers.size() != rhs.finalizers.size())
            return finalizers.size() < rhs.finalizers.size() ? std::strong_ordering::less : std::strong_ordering::greater;
         for (size_t i = 0; i < finalizers.size(); ++i) {
            const auto& a = finalizers[i];
            const auto& b = rhs.finalizers[i];
            if (int c = a.description.compare(b.description); c != 0)
               return c < 0 ? std::strong_ordering::less : std::strong_ordering::greater;
            if (auto c = a.weight <=> b.weight; c != 0) return c;
            if (auto c = a.public_key <=> b.public_key; c != 0) return c;
         }
         return std::strong_ordering::equal;
      }
      bool operator==(const finalizer_policy& rhs) const {
         return generation == rhs.generation && threshold == rhs.threshold && finalizers == rhs.finalizers;
      }
   };

   // This is used by SHiP and Deepmind which require public keys in string format.
   struct finalizer_policy_with_string_key {
      uint32_t                                         generation = 0;
      uint64_t                                         threshold = 0;
      std::vector<finalizer_authority_with_string_key> finalizers;

      finalizer_policy_with_string_key() = default;
      explicit finalizer_policy_with_string_key(const finalizer_policy& input)
         : generation(input.generation)
         , threshold(input.threshold)
      {
         finalizers.resize(input.finalizers.size());
         std::transform(
            input.finalizers.begin(),
            input.finalizers.end(),
            finalizers.begin(),
            [](const finalizer_authority& fin_authority) {
               return finalizer_authority_with_string_key(fin_authority); });
      }
   };

   using finalizer_policy_ptr = std::shared_ptr<finalizer_policy>;
   using finalizer_policy_diff_ptr = std::shared_ptr<finalizer_policy_diff>;

} /// eosio::chain

FC_REFLECT( eosio::chain::finalizer_policy, (generation)(threshold)(finalizers) )
FC_REFLECT( eosio::chain::finalizers_diff_t, (remove_indexes)(insert_indexes) )
FC_REFLECT( eosio::chain::finalizer_policy_diff, (generation)(threshold)(finalizers_diff) )
FC_REFLECT( eosio::chain::finalizer_policy_with_string_key, (generation)(threshold)(finalizers) )
