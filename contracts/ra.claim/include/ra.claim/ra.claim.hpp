#pragma once

#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>
#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>
#include <eosio/system.hpp>
#include <ra.token/ra.token.hpp>

#include <string>

namespace eosio {

   using std::string;

   /**
    * Simple deposit-there / claim-here unlock. A relayer records an external
    * deposit; the recipient withdraws RA from this contract's treasury.
    * This is not Wire OPP (no outposts, batch operators, or msgch).
    */
   class [[eosio::contract("ra.claim")]] claimc : public contract {
      public:
         using contract::contract;

         static constexpr name token_account{"ra.token"_n};

         struct [[eosio::table]] relayer {
            name account;
            uint64_t primary_key() const { return account.value; }
         };

         struct [[eosio::table]] deposit {
            uint64_t    id = 0;
            checksum256 ext_txid;
            name        chain;
            name        recipient;
            asset       quantity;
            bool        claimed = false;
            time_point  created;

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

         using relayers_table = multi_index< "relayers"_n, relayer >;
         using deposits_table = multi_index< "deposits"_n, deposit,
                               indexed_by< "bytxid"_n, const_mem_fun<deposit, uint64_t, &deposit::by_txid> > >;
         using state_singleton = singleton< "state"_n, state_row >;

         [[eosio::action]]
         void addrelayer( const name& account );

         [[eosio::action]]
         void rmrelayer( const name& account );

         [[eosio::action]]
         void credit( const name& chain, const checksum256& ext_txid, const name& recipient, const asset& quantity );

         [[eosio::action]]
         void claim( uint64_t id );

      private:
         void require_relayer() const;
   };

} // namespace eosio
