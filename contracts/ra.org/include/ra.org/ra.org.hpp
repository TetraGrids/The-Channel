#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>
#include <eosio/system.hpp>

#include <string>

namespace eosio {

   using std::string;

   /**
    * Org backpay. People accrue a monthly buck amount for a role, judged by
    * self, manager, and peer review. Incoming buck tokens on this account
    * trickle out in proportion to what the org owes each person versus
    * everyone. The buck token lives in config and is not hardcoded.
    */
   class [[eosio::contract("ra.org")]] org : public contract {
      public:
         using contract::contract;

         static constexpr uint16_t GUARANTEED_BPS = 100;   // 1%
         static constexpr uint16_t REVIEW_BPS     = 3300;  // 33% each of self / manager / peer
         static constexpr uint8_t  RATING_MAX      = 10;

         struct [[eosio::table("config")]] config_row {
            name     buck_contract;          // token contract that pays backpay
            symbol   buck;                  // buck symbol (precision + code)
            uint16_t open_days  = 3;        // reviews open this many days before month end
            uint16_t grace_days = 3;       // reviews close this many days after month end
            uint64_t next_role_id = 1;
            uint64_t next_seat_id = 1;
         };

         struct [[eosio::table("stat")]] stat_row {
            asset owed;                     // total outstanding backpay
            asset paid;                    // lifetime trickle paid
         };

         struct [[eosio::table]] admin {
            name account;
            uint64_t primary_key() const { return account.value; }
         };

         struct [[eosio::table]] role {
            uint64_t id = 0;
            string   kpi_name;
            name     kpi_type;             // number, text, ...
            asset    base_salary;           // per person, in bucks
            asset    bucket_salary;         // split among people with this role
            uint16_t boost = 0;            // percent of base added first
            uint64_t review_role = 0;      // 0 = same-role peers
            uint64_t primary_key() const { return id; }
         };

         struct [[eosio::table]] seat {
            uint64_t   id = 0;
            uint64_t   role_id = 0;
            name       worker;
            time_point granted;
            uint64_t primary_key() const { return id; }
            uint64_t by_role() const { return role_id; }
            uint64_t by_worker() const { return worker.value; }
         };

         struct [[eosio::table]] review {
            uint64_t id = 0;
            uint64_t role_id = 0;
            name     worker;
            name     reviewer;
            name     kind;                 // self | manager | peer
            uint8_t  rating = 0;            // 0-10
            string   proof;                // proof of work link
            uint64_t primary_key() const { return id; }
            uint128_t by_target() const { return ( uint128_t( role_id ) << 64 ) | worker.value; }
         };

         struct [[eosio::table]] account {
            name  owner;
            asset owed;
            uint64_t primary_key() const { return owner.value; }
         };

         struct [[eosio::table]] period {
            uint32_t yyyymm = 0;
            uint64_t primary_key() const { return yyyymm; }
         };

         using config_singleton = singleton< "config"_n, config_row >;
         using stat_singleton   = singleton< "stat"_n, stat_row >;
         using admins_table     = multi_index< "admins"_n, admin >;
         using roles_table      = multi_index< "roles"_n, role >;
         using seats_table      = multi_index< "seats"_n, seat,
                                   indexed_by< "byrole"_n, const_mem_fun<seat, uint64_t, &seat::by_role> >,
                                   indexed_by< "byworker"_n, const_mem_fun<seat, uint64_t, &seat::by_worker> > >;
         using reviews_table    = multi_index< "reviews"_n, review,
                                   indexed_by< "bytarget"_n, const_mem_fun<review, uint128_t, &review::by_target> > >;
         using accounts_table   = multi_index< "accounts"_n, account >;
         using periods_table    = multi_index< "periods"_n, period >;

         [[eosio::action]]
         void setconfig( const name& buck_contract, const symbol& buck );

         [[eosio::action]]
         void addadmin( const name& account );

         [[eosio::action]]
         void rmadmin( const name& account );

         [[eosio::action]]
         void addrole( const string& kpi_name, const name& kpi_type,
                        const asset& base_salary, const asset& bucket_salary,
                        uint16_t boost, uint64_t review_role );

         [[eosio::action]]
         void editrole( uint64_t role_id, const string& kpi_name, const name& kpi_type,
                         const asset& base_salary, const asset& bucket_salary,
                         uint16_t boost, uint64_t review_role );

         [[eosio::action]]
         void grantrole( uint64_t role_id, const name& worker );

         [[eosio::action]]
         void revokerole( uint64_t role_id, const name& worker );

         [[eosio::action]]
         void review( const name& reviewer, uint32_t period, uint64_t role_id,
                       const name& worker, uint8_t rating, const string& proof );

         [[eosio::action]]
         void closeperiod( uint32_t period );

         [[eosio::on_notify("*::transfer")]]
         void on_transfer( const name& from, const name& to, const asset& quantity, const string& memo );

         static asset get_balance( const name& org_contract, const name& owner, const symbol_code& sym_code )
         {
            config_singleton cfg( org_contract, org_contract.value );
            check( cfg.exists(), "org: config not found" );
            const auto c = cfg.get();
            check( c.buck.code() == sym_code, "org: buck symbol mismatch" );
            accounts_table accounts( org_contract, org_contract.value );
            auto itr = accounts.find( owner.value );
            if ( itr == accounts.end() ) {
               return asset{ 0, c.buck };
            }
            return itr->owed;
         }

         using setconfig_action  = action_wrapper< "setconfig"_n,  &org::setconfig >;
         using addadmin_action    = action_wrapper< "addadmin"_n,    &org::addadmin >;
         using rmadmin_action     = action_wrapper< "rmadmin"_n,     &org::rmadmin >;
         using addrole_action    = action_wrapper< "addrole"_n,    &org::addrole >;
         using editrole_action   = action_wrapper< "editrole"_n,   &org::editrole >;
         using grantrole_action  = action_wrapper< "grantrole"_n,  &org::grantrole >;
         using revokerole_action = action_wrapper< "revokerole"_n, &org::revokerole >;
         using review_action     = action_wrapper< "review"_n,     &org::review >;
         using closeperiod_action = action_wrapper< "closeperiod"_n, &org::closeperiod >;

      private:
         config_row get_config() const;
         void set_cfg( const config_row& c );
         stat_row get_stat() const;
         void set_st( const stat_row& s );

         void require_admin() const;
         bool is_admin( const name& account ) const;
         bool has_seat( uint64_t role_id, const name& worker ) const;
         uint32_t count_seats( uint64_t role_id ) const;
         uint32_t count_peers( uint64_t peer_role, const name& worker ) const;
         void check_period( uint32_t period ) const;
         void check_salary( const asset& quantity, const symbol& buck ) const;
         void write_role( role& row, const string& kpi_name, const name& kpi_type,
                           const asset& base_salary, const asset& bucket_salary,
                           uint16_t boost, uint64_t review_role, const symbol& buck ) const;
         void add_owed( const name& owner, const asset& quantity );
         name infer_kind( const name& reviewer, const name& worker, uint64_t role_id, uint64_t review_role ) const;
   };

} // namespace eosio
