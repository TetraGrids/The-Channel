#pragma once

#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>
#include <eosio/system.hpp>

#include <string>
#include <vector>

namespace eosio {

   using std::string;
   using std::vector;

   /**
    * External-key registry. A Channel account attests that it controls a key
    * or address on another chain (EVM, SVM, ...). This does not grant signing
    * authority on The Channel; EM/ED keys sign Channel transactions directly
    * once they are on an account permission.
    *
    * Unsigned `createlink` is a self-attested address string. `linksig` also
    * stores the recovered packed pubkey so `ra.claim` can verify user-signed
    * claims. See contracts/ra.claim/FUTURE.md.
    */
   class [[eosio::contract("ra.authex")]] authex : public contract {
      public:
         using contract::contract;

         struct [[eosio::table]] link {
            uint64_t      id = 0;
            name          account;
            name          chain;
            std::string   address;
            time_point    created;
            vector<char>  packedkey; // fc-packed Channel pubkey; empty for unsigned v1 links

            uint64_t primary_key() const { return id; }
            uint64_t by_account() const { return account.value; }
         };

         struct [[eosio::table]] nonce {
            name     account;
            uint64_t next_nonce = 0;
            uint64_t primary_key() const { return account.value; }
         };

         struct [[eosio::table("state")]] state_row {
            uint64_t next_id = 1;
         };

         using links_table = multi_index< "links"_n, link,
                               indexed_by< "byaccount"_n, const_mem_fun<link, uint64_t, &link::by_account> > >;
         using nonces_table = multi_index< "nonces"_n, nonce >;
         using state_singleton = singleton< "state"_n, state_row >;

         [[eosio::action]]
         void createlink( const name& account, const name& chain, const std::string& address );

         [[eosio::action]]
         void unlink( uint64_t id );

         [[eosio::action]]
         void adminlink( const name& account, const name& chain, const std::string& address );

         /// Channel account auth plus recover_key of
         /// `"<pubkey>|<account>|<chain>|<nonce>|createlink auth"`.
         [[eosio::action]]
         void linksig( const name& account, const name& chain, const std::string& address,
                       const std::string& pubkey, uint64_t nonce, const vector<char>& sig );

         /// `ra.claim` records the key it just installed on a new account.
         [[eosio::action]]
         void bridgelink( const name& account, const name& chain, const std::string& address,
                          const vector<char>& packedkey );

      private:
         void add_link( const name& account, const name& chain, const std::string& address,
                        const vector<char>& packedkey, const name& ram_payer );
         uint64_t expected_nonce( const name& account ) const;
         void bump_nonce( const name& account, const name& ram_payer );
   };

} // namespace eosio
