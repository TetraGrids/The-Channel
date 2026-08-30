#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/time.hpp>
#include <eosio/singleton.hpp>

namespace eosio {

   using std::string;

   // === flex.token Contract Interface === //
   class [[eosio::contract("flex.token")]] flex_token : public contract {
      public:
         using contract::contract;

         ACTION forge(const name& issuer, const asset& maximum_supply);
         ACTION mint(const name& to, const asset& quantity, const string& memo);
         ACTION smelt(const name& username, const asset& quantity, const string& memo);
         ACTION transfer(const name& from, const name& to, const asset& quantity, const string& memo);
         ACTION open(const name& owner, const symbol& symbol, const name& ram_payer);
         ACTION close(const name& owner, const symbol& symbol);
         ACTION reflect();
         ACTION setconfig(const symbol& sym, uint64_t start_key, uint32_t limit, uint16_t reflection_rate, uint16_t burn_rate, uint16_t project_rate, const name& project_account);
         ACTION renounce(const name& account, const bool& ban_status);
         ACTION addpool(const uint64_t& id, const symbol& token_symbol, const name& token_contract, const string& pool_ids);
         ACTION interestoken(const name& owner, const string& token_symbol);
         ACTION inheritance(const name& flexer, const name& tree, const uint16_t& rate);
         ACTION inheritmemo(const name& flexer, const string& custom_memo);
         
         [[eosio::on_notify("*::transfer")]]
         void handle_transfer(name from, name to, asset quantity, string memo);

         static asset get_supply(const name& token_contract_account, const symbol_code& sym_code) {
            stats statstable(token_contract_account, sym_code.raw());
            const auto& st = statstable.get(sym_code.raw(), "invalid supply symbol code 🤷");
            return st.supply;
         }

         static asset get_balance(const name& token_contract_account, const name& owner, const symbol_code& sym_code) {
            accounts accountstable(token_contract_account, owner.value);
            const auto& ac = accountstable.get(sym_code.raw(), "no balance with specified symbol 🤷");
            return ac.balance;
         }

         using forge_action = eosio::action_wrapper<"forge"_n, &flex_token::forge>;
         using mint_action = eosio::action_wrapper<"mint"_n, &flex_token::mint>;
         using smelt_action = eosio::action_wrapper<"smelt"_n, &flex_token::smelt>;
         using transfer_action = eosio::action_wrapper<"transfer"_n, &flex_token::transfer>;
         using open_action = eosio::action_wrapper<"open"_n, &flex_token::open>;
         using close_action = eosio::action_wrapper<"close"_n, &flex_token::close>;
         using reflect_action = eosio::action_wrapper<"reflect"_n, &flex_token::reflect>;
         using setconfig_action = eosio::action_wrapper<"setconfig"_n, &flex_token::setconfig>;
         using renounce_action = eosio::action_wrapper<"renounce"_n, &flex_token::renounce>;
         using addpool_action = eosio::action_wrapper<"addpool"_n, &flex_token::addpool>;
         using interestoken_action = eosio::action_wrapper<"interestoken"_n, &flex_token::interestoken>;
         using inheritance_action = eosio::action_wrapper<"inheritance"_n, &flex_token::inheritance>;
         using inheritmemo_action = eosio::action_wrapper<"inheritmemo"_n, &flex_token::inheritmemo>;

      private:
         TABLE account {
            asset    balance;
            uint64_t primary_key()const { return balance.symbol.code().raw(); }
         };

         TABLE currency_stats {
            asset    supply;
            asset    max_supply;
            name     issuer;
            asset    reflection_pool;
            asset    burn_pool;
            asset    project_pool;
            uint64_t primary_key()const { return supply.symbol.code().raw(); }
            EOSLIB_SERIALIZE(currency_stats, (supply)(max_supply)(issuer)(reflection_pool)(burn_pool)(project_pool))
         };

         TABLE flexer {
            name     owner;
            asset    balance;
            bool     is_banned = false;    // Default to not banned
            uint64_t flextoken = 0;    // Default to 0, will be used as ID for flex pool
            name     tree;
            uint16_t tree_rate = 10000;
            string   custom_memo;
            uint64_t primary_key()const { return owner.value; }
         };

         TABLE settings {
            symbol    token_symbol;
            uint64_t  start_key = 0;
            uint32_t  limit = 100;
            uint16_t  reflection_rate = 100;  // 1% = 100, 100% = 10000
            uint16_t  burn_rate = 0;          // Default 0%
            uint16_t  project_rate = 100;     // 1% = 100, 100% = 10000
            name      project_account;
            
            EOSLIB_SERIALIZE(settings, (token_symbol)(start_key)(limit)(reflection_rate)(burn_rate)(project_rate)(project_account))
         };

         TABLE flexpool {
            uint64_t id;                // Primary key
            symbol   token_symbol;      // Symbol of the output token
            name     token_contract;    // Contract of the output token
            string   pool_ids;         // Comma-separated pool IDs for Alcor
            
            uint64_t primary_key()const { return id; }
            uint64_t by_symbol()const { return token_symbol.raw(); }
         };

         typedef eosio::multi_index<"flexpools"_n, flexpool,
            indexed_by<"bysymbol"_n, const_mem_fun<flexpool, uint64_t, &flexpool::by_symbol>>
         > flexpools;

         typedef eosio::singleton<"settings"_n, settings> distribution_singleton;

         using accounts = eosio::multi_index<"accounts"_n, account>;
         using stats = eosio::multi_index<"stat"_n, currency_stats>;
         using flexers = eosio::multi_index<"flexers"_n, flexer>;

         void sub_balance(const name& owner, const asset& value);
         void add_balance(const name& owner, const asset& value, const name& ram_payer);
         void update_flex_balance(const name& owner, const asset& value);
         void set_distribution_config(const symbol& sym, uint64_t start, uint32_t lim, uint16_t reflection_rate, uint16_t burn_rate, uint16_t project_rate, const name& project_account);
   };
} 