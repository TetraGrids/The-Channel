#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <ra.system/ra.system.hpp>
#include <ra.token/ra.token.hpp>

#include <string>

namespace eosio {

   using std::string;

   /**
    * XPR-style CPU/NET subscription rental. Prefund the stake pool by
    * transferring RA to this contract with memo `fund`. Users pay a plan
    * fee; the contract delegates from that pool. REX is not used.
    */
   class [[eosio::contract("ra.resources")]] resources : public contract {
      public:
         using contract::contract;

         static constexpr name system_account{"ra"_n};
         static constexpr name token_account{"ra.token"_n};
         static constexpr name fees_account{"ra.fees"_n};

         struct [[eosio::table]] plan {
            uint64_t       id = 0;
            asset          cpu_weight;
            asset          net_weight;
            uint32_t       duration_sec = 0;
            asset          price;
            std::string    label;

            uint64_t primary_key() const { return id; }
         };

         struct [[eosio::table]] subscription {
            name           account;
            uint64_t       plan_id = 0;
            time_point_sec expires;
            asset          cpu_weight;
            asset          net_weight;

            uint64_t primary_key() const { return account.value; }
            uint64_t by_expiry() const { return expires.utc_seconds; }
         };

         struct [[eosio::table("state")]] state_row {
            asset available;
            asset delegated;
            uint64_t next_plan_id = 1;
         };

         using plans_table = multi_index< "plans"_n, plan >;
         using subs_table  = multi_index< "subs"_n, subscription,
                               indexed_by< "byexpiry"_n, const_mem_fun<subscription, uint64_t, &subscription::by_expiry> > >;
         using state_singleton = singleton< "state"_n, state_row >;

         [[eosio::action]]
         void addplan( const asset& cpu_weight, const asset& net_weight,
                       uint32_t duration_sec, const asset& price, const std::string& label );

         [[eosio::action]]
         void rmplan( uint64_t plan_id );

         [[eosio::action]]
         void buyplan( const name& account, uint64_t plan_id );

         [[eosio::action]]
         void process( uint32_t max );

         [[eosio::action]]
         void claimrefund();

         [[eosio::on_notify("ra.token::transfer")]]
         void on_transfer( const name& from, const name& to, const asset& quantity, const string& memo );

      private:
         void require_admin() const;
         symbol core_symbol() const;
         state_row get_state() const;
         void set_state( const state_row& st );
   };

} // namespace eosio
