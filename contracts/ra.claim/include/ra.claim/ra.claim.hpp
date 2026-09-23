#pragma once

#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>
#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>
#include <eosio/system.hpp>
#include <ra.token/ra.token.hpp>

#include <string>
#include <vector>

namespace eosio {

   using std::string;
   using std::vector;

   /**
    * Simple deposit-there / claim-here unlock. Relayer `credit` records a
    * deposit; `claim` pays with Channel account auth; `claimsig` pays with a
    * linked EM/ED key (see FUTURE.md). This is not Wire OPP.
    */
   class [[eosio::contract("ra.claim")]] claimc : public contract {
      public:
         using contract::contract;

         static constexpr name token_account{"ra.token"_n};
         static constexpr name authex_account{"ra.authex"_n};

         struct [[eosio::table]] relayer {
            name account;
            uint64_t primary_key() const { return account.value; }
         };

         struct [[eosio::table]] deposit {
            uint64_t     id = 0;
            checksum256  ext_txid;
            name         chain;
            name         recipient;
            asset        quantity;
            bool         claimed = false;
            time_point   created;
            std::string  pubkey;    // Channel string form; empty unless creditopen
            vector<char> packedkey; // fc-packed public key; empty unless creditopen

            uint64_t primary_key() const { return id; }
            uint64_t by_txid() const {
               const auto bytes = ext_txid.extract_as_byte_array();
               uint64_t v = 0;
               for ( size_t i = 0; i < 8; ++i ) {
                  v = ( v << 8 ) | bytes[i];
               }
               return v;
            }
         };

         struct [[eosio::table("state")]] state_row {
            uint64_t next_id = 1;
         };

         /// RAM taken out of a lazy claim. `ram_bytes` 0 skips `buyrambytes`
         /// (tester chains bill unlimited RAM). `ram_fee` is withheld from the payout.
         struct [[eosio::table("cfg")]] claim_cfg {
            uint32_t ram_bytes = 0;
            asset    ram_fee;
         };

         using relayers_table = multi_index< "relayers"_n, relayer >;
         using deposits_table = multi_index< "deposits"_n, deposit,
                               indexed_by< "bytxid"_n, const_mem_fun<deposit, uint64_t, &deposit::by_txid> > >;
         using state_singleton = singleton< "state"_n, state_row >;
         using cfg_singleton = singleton< "cfg"_n, claim_cfg >;

         [[eosio::action]]
         void addrelayer( const name& account );

         [[eosio::action]]
         void rmrelayer( const name& account );

         [[eosio::action]]
         void credit( const name& chain, const checksum256& ext_txid, const name& recipient, const asset& quantity );

         [[eosio::action]]
         void claim( uint64_t id );

         /// Credit a deposit for a Channel name that does not exist yet.
         /// `pubkey` is the Channel string (`PUB_EM_…` / `PUB_ED_…`).
         /// `packedkey` is `fc::raw::pack` of that same public key.
         [[eosio::action]]
         void creditopen( const name& chain, const checksum256& ext_txid, const name& recipient,
                          const asset& quantity, const std::string& pubkey, const vector<char>& packedkey );

         /// Pay a credited deposit using recover_key of
         /// `"<ext_txid>|<recipient>|<quantity>|<chain>|<id>|claim auth"`.
         /// No Channel account signature; first authorizer pays CPU.
         [[eosio::action]]
         void claimsig( uint64_t id, const vector<char>& sig );

         /// Create `recipient` with the credited key and pay the deposit minus `ram_fee`.
         /// Message: `"<pubkey>|<ext_txid>|<recipient>|<quantity>|<chain>|<id>|claimopen auth"`.
         [[eosio::action]]
         void claimopen( uint64_t id, const vector<char>& sig );

         [[eosio::action]]
         void cfgclaim( uint32_t ram_bytes, const asset& ram_fee );

      private:
         void require_relayer() const;
         void pay_claim( deposits_table& deposits, uint64_t id, const asset& payout );
         bool linked_key_matches( const name& account, const name& chain, const vector<char>& packed ) const;
         void emplace_deposit( const name& chain, const checksum256& ext_txid, const name& recipient,
                               const asset& quantity, const std::string& pubkey, const vector<char>& packedkey );
   };

} // namespace eosio
