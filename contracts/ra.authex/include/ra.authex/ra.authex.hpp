#pragma once

#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>
#include <eosio/system.hpp>

#include <string>

namespace eosio {

   using std::string;

   /**
    * External-key registry. A Channel account attests that it controls a key
    * or address on another chain (EVM, SVM, ...). This does not grant signing
    * authority on The Channel; EM/ED keys sign Channel transactions directly
    * once they are on an account permission.
    */
   class [[eosio::contract("ra.authex")]] authex : public contract {
      public:
         using contract::contract;

         struct [[eosio::table]] link {
            uint64_t    id = 0;
            name        account;
            name        chain;
            std::string address;
            time_point  created;

            uint64_t primary_key() const { return id; }
            uint64_t by_account() const { return account.value; }
         };

         struct [[eosio::table("state")]] state_row {
            uint64_t next_id = 1;
         };

         using links_table = multi_index< "links"_n, link,
                               indexed_by< "byaccount"_n, const_mem_fun<link, uint64_t, &link::by_account> > >;
         using state_singleton = singleton< "state"_n, state_row >;

         [[eosio::action]]
         void createlink( const name& account, const name& chain, const std::string& address );

         [[eosio::action]]
         void unlink( uint64_t id );

         [[eosio::action]]
         void adminlink( const name& account, const name& chain, const std::string& address );

      private:
         void add_link( const name& account, const name& chain, const std::string& address, const name& ram_payer );
   };

} // namespace eosio
