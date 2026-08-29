#include <ra.resources/ra.resources.hpp>
#include <eosio/time.hpp>

namespace eosio {

   void resources::require_admin() const {
      check( has_auth( get_self() ) || has_auth( system_account ), "missing authority of ra.resources or ra" );
   }

   symbol resources::core_symbol() const {
      return rasystem::system_contract::get_core_symbol();
   }

   resources::state_row resources::get_state() const {
      state_singleton sing{ get_self(), get_self().value };
      auto st = sing.get_or_default();
      const auto core = core_symbol();
      if ( st.available.symbol != core ) st.available = asset{ 0, core };
      if ( st.delegated.symbol != core ) st.delegated = asset{ 0, core };
      if ( st.next_plan_id == 0 ) st.next_plan_id = 1;
      return st;
   }

   void resources::set_state( const state_row& st ) {
      state_singleton sing{ get_self(), get_self().value };
      sing.set( st, get_self() );
   }

   void resources::addplan( const asset& cpu_weight, const asset& net_weight,
                            uint32_t duration_sec, const asset& price, const std::string& label )
   {
      require_admin();
      const auto core = core_symbol();
      check( cpu_weight.symbol == core && net_weight.symbol == core && price.symbol == core, "weights and price must be core token" );
      check( cpu_weight.amount >= 0 && net_weight.amount >= 0, "weights must be non-negative" );
      check( cpu_weight.amount + net_weight.amount > 0, "plan must delegate a positive amount" );
      check( price.amount > 0, "price must be positive" );
      check( duration_sec > 0, "duration must be positive" );
      check( label.size() <= 64, "label has more than 64 bytes" );

      auto st = get_state();
      plans_table plans{ get_self(), get_self().value };
      plans.emplace( get_self(), [&]( auto& row ) {
         row.id           = st.next_plan_id++;
         row.cpu_weight   = cpu_weight;
         row.net_weight   = net_weight;
         row.duration_sec = duration_sec;
         row.price        = price;
         row.label        = label;
      });
      set_state( st );
   }

   void resources::rmplan( uint64_t plan_id )
   {
      require_admin();
      plans_table plans{ get_self(), get_self().value };
      auto itr = plans.require_find( plan_id, "plan does not exist" );
      plans.erase( itr );
   }

   void resources::buyplan( const name& account, uint64_t plan_id )
   {
      require_auth( account );
      check( is_account( account ), "account does not exist" );

      plans_table plans{ get_self(), get_self().value };
      const auto& plan = plans.get( plan_id, "plan does not exist" );

      subs_table subs{ get_self(), get_self().value };
      check( subs.find( account.value ) == subs.end(), "account already has an active plan; wait for expiry" );

      const asset stake = plan.cpu_weight + plan.net_weight;
      auto st = get_state();
      check( st.available >= stake, "resource pool has insufficient unstaked tokens" );

      token::transfer_action pay{ token_account, { { account, "active"_n } } };
      pay.send( account, fees_account, plan.price, "resource plan fee" );

      rasystem::system_contract::delegatebw_action del{ system_account, { { get_self(), "active"_n } } };
      del.send( get_self(), account, plan.net_weight, plan.cpu_weight, false );

      st.available -= stake;
      st.delegated += stake;
      set_state( st );

      const auto now = current_time_point();
      subs.emplace( get_self(), [&]( auto& row ) {
         row.account    = account;
         row.plan_id    = plan_id;
         row.expires    = time_point_sec{ now + seconds( plan.duration_sec ) };
         row.cpu_weight = plan.cpu_weight;
         row.net_weight = plan.net_weight;
      });
   }

   void resources::process( uint32_t max )
   {
      check( max > 0 && max <= 100, "max must be in 1..100" );
      subs_table subs{ get_self(), get_self().value };
      auto idx = subs.get_index< "byexpiry"_n >();
      const auto now = time_point_sec{ current_time_point() };

      uint32_t n = 0;
      auto st = get_state();
      for ( auto itr = idx.begin(); n < max && itr != idx.end() && itr->expires <= now; ) {
         rasystem::system_contract::undelegatebw_action undel{ system_account, { { get_self(), "active"_n } } };
         undel.send( get_self(), itr->account, itr->net_weight, itr->cpu_weight );
         st.delegated -= ( itr->cpu_weight + itr->net_weight );
         itr = idx.erase( itr );
         ++n;
      }
      if ( n > 0 ) {
         set_state( st );
      }
   }

   void resources::claimrefund()
   {
      rasystem::system_contract::refund_action refund{ system_account, { { get_self(), "active"_n } } };
      refund.send( get_self() );
   }

   void resources::on_transfer( const name& from, const name& to, const asset& quantity, const string& memo )
   {
      if ( to != get_self() || from == get_self() ) {
         return;
      }
      if ( quantity.symbol != core_symbol() ) {
         return;
      }
      check( quantity.amount > 0, "quantity must be positive" );

      if ( memo == "fund" ) {
         auto st = get_state();
         st.available += quantity;
         set_state( st );
         return;
      }
      if ( memo == "unstake" ) {
         check( from == "ra.stake"_n, "unstake memo is only accepted from ra.stake" );
         auto st = get_state();
         st.available += quantity;
         set_state( st );
         return;
      }
      check( false, "ra.resources only accepts transfers with memo 'fund'" );
   }

} // namespace eosio
