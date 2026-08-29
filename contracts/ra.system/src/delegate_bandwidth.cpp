#include <eosio/datastream.hpp>
#include <eosio/eosio.hpp>
#include <eosio/multi_index.hpp>
#include <eosio/privileged.hpp>
#include <eosio/serialize.hpp>
#include <eosio/transaction.hpp>

#include <limits>

#include <ra.system/ra.system.hpp>
#include <ra.token/ra.token.hpp>

namespace rasystem {

   using eosio::asset;
   using eosio::const_mem_fun;
   using eosio::current_time_point;
   using eosio::indexed_by;
   using eosio::permission_level;
   using eosio::seconds;
   using eosio::time_point_sec;
   using eosio::token;

   eosio_global_stateram system_contract::ram_config() const {
      auto cfg = _globalram.get_or_default();
      if ( cfg.ram_price_per_byte.amount <= 0 ) {
         cfg.ram_price_per_byte = asset{ default_ram_price_amt, core_symbol() };
      } else if ( cfg.ram_price_per_byte.symbol != core_symbol() ) {
         cfg.ram_price_per_byte.symbol = core_symbol();
      }
      if ( cfg.max_per_user_bytes == 0 ) {
         cfg.max_per_user_bytes = ram_cap_max_bytes;
      }
      return cfg;
   }

   void system_contract::ensure_usersram( const name& owner ) {
      auto itr = _usersram.find( owner.value );
      if ( itr != _usersram.end() ) {
         return;
      }
      _usersram.emplace( owner, [&]( auto& row ) {
         row.owner     = owner;
         row.ram_bytes = 0;
         row.quantity  = asset{ 0, core_symbol() };
         row.ramlimit  = 0;
         row.created   = current_time_point();
      });
   }

   int64_t system_contract::purchased_ram_cap( const name& owner, const eosio_global_stateram& cfg ) const {
      auto itr = _usersram.find( owner.value );
      int64_t years = 0;
      int64_t extra = 0;
      if ( itr != _usersram.end() ) {
         extra = itr->ramlimit;
         const auto created_sec = itr->created.sec_since_epoch();
         if ( created_sec > 0 ) {
            const int64_t now_sec = current_time_point().sec_since_epoch();
            if ( now_sec > created_sec ) {
               years = ( now_sec - created_sec ) / ram_cap_year_sec;
            }
         }
      }
      int128_t cap = int128_t( ram_cap_base_bytes ) + int128_t( ram_cap_per_year_bytes ) * years;
      const int64_t ceiling = static_cast<int64_t>( cfg.max_per_user_bytes );
      if ( cap > ceiling ) {
         cap = ceiling;
      }
      cap += extra;
      check( cap <= std::numeric_limits<int64_t>::max(), "ram cap overflow" );
      return static_cast<int64_t>( cap );
   }

   void system_contract::assert_under_ram_cap( const name& owner, int64_t additional_bytes, const eosio_global_stateram& cfg ) const {
      auto itr = _usersram.find( owner.value );
      const int64_t current = ( itr == _usersram.end() ) ? 0 : itr->ram_bytes;
      check( additional_bytes <= std::numeric_limits<int64_t>::max() - current, "ram bytes overflow" );
      check( current + additional_bytes <= purchased_ram_cap( owner, cfg ), "purchased ram exceeds the per-account cap" );
   }

   void system_contract::credit_purchased_ram( const name& owner, int64_t bytes, const asset& cost ) {
      ensure_usersram( owner );
      auto itr = _usersram.find( owner.value );
      _usersram.modify( itr, same_payer, [&]( auto& row ) {
         row.ram_bytes += bytes;
         row.quantity  += cost;
      });
   }

   asset system_contract::debit_purchased_ram( const name& owner, int64_t bytes ) {
      auto itr = _usersram.find( owner.value );
      check( itr != _usersram.end(), "no purchased ram to sell" );
      check( bytes <= itr->ram_bytes, "insufficient purchased ram" );

      const int128_t cost_i = ( int128_t( itr->quantity.amount ) * bytes ) / itr->ram_bytes;
      check( cost_i <= std::numeric_limits<int64_t>::max(), "ram cost overflow" );
      asset refund{ static_cast<int64_t>( cost_i ), itr->quantity.symbol };

      _usersram.modify( itr, same_payer, [&]( auto& row ) {
         row.ram_bytes -= bytes;
         row.quantity  -= refund;
      });
      // Keep the row so `created` (age clock) is never reset by a full sell.
      return refund;
   }

   void system_contract::transfer_purchased_ram( const name& from, const name& to, int64_t bytes ) {
      const asset moved = debit_purchased_ram( from, bytes );
      if ( to != null_account ) {
         credit_purchased_ram( to, bytes, moved );
      }
   }

   /**
    *  This action will buy an exact amount of ram and bill the payer the current fixed price plus fee.
    */
   action_return_buyram system_contract::buyrambytes( const name& payer, const name& receiver, uint32_t bytes ) {
      check( bytes > 0, "must purchase a positive amount" );
      const auto cfg = ram_config();
      check( cfg.ram_fee_percent < ram_fee_precision, "invalid ram fee percent" );

      const int128_t net_cost = int128_t( bytes ) * cfg.ram_price_per_byte.amount;
      check( net_cost > 0, "ram price is too low for this purchase" );
      const int128_t denom = ram_fee_precision - cfg.ram_fee_percent;
      const int128_t gross = ( net_cost * ram_fee_precision + denom - 1 ) / denom;
      check( gross <= std::numeric_limits<int64_t>::max(), "ram cost overflow" );
      return buyram( payer, receiver, asset{ static_cast<int64_t>( gross ), core_symbol() } );
   }

   /**
    * Buy self ram action, ram can only be purchased to itself.
    */
   action_return_buyram system_contract::buyramself( const name& account, const asset& quant ) {
      return buyram( account, account, quant );
   }

   /**
    *  When buying ram the payer transfers quant to ra.ram (net of fee) and only
    *  the receiver may reclaim those tokens via sellram at average cost basis.
    *  RAM is priced at a governance-set fixed rate; rammarket.base is the free-byte ledger.
    */
   action_return_buyram system_contract::buyram( const name& payer, const name& receiver, const asset& quant )
   {
      require_auth( payer );
      update_ram_supply();
      require_recipient(payer);
      require_recipient(receiver);

      check( quant.symbol == core_symbol(), "must buy ram with core token" );
      check( quant.amount > 0, "must purchase a positive amount" );

      const auto cfg = ram_config();
      check( cfg.ram_price_per_byte.amount > 0, "ram price must be positive" );
      check( cfg.ram_fee_percent < ram_fee_precision, "invalid ram fee percent" );

      asset fee = quant;
      fee.amount = static_cast<int64_t>( ( int128_t( quant.amount ) * cfg.ram_fee_percent + ram_fee_precision - 1 ) / ram_fee_precision );
      check( fee.amount < quant.amount, "ram fee consumes the entire payment" );

      asset quant_after_fee = quant;
      quant_after_fee.amount -= fee.amount;

      const int64_t bytes_out = static_cast<int64_t>( int128_t( quant_after_fee.amount ) / cfg.ram_price_per_byte.amount );
      check( bytes_out > 0, "must reserve a positive amount" );

      const auto& market = _rammarket.get( ramcore_symbol.raw(), "ram market does not exist" );
      check( market.base.balance.amount >= bytes_out, "insufficient available ram" );
      assert_under_ram_cap( receiver, bytes_out, cfg );

      {
         token::transfer_action transfer_act{ token_account, { {payer, active_permission}, {ram_account, active_permission} } };
         transfer_act.send( payer, ram_account, quant_after_fee, "buy ram" );
      }
      if ( fee.amount > 0 ) {
         token::transfer_action transfer_act{ token_account, { {payer, active_permission} } };
         transfer_act.send( payer, ramfee_account, fee, "ram fee" );
         channel_to_system_fees( ramfee_account, fee );
      }

      _rammarket.modify( market, same_payer, [&]( auto& es ) {
         es.base.balance.amount -= bytes_out;
      });

      _gstate.total_ram_bytes_reserved += uint64_t(bytes_out);
      _gstate.total_ram_stake          += quant_after_fee.amount;

      credit_purchased_ram( receiver, bytes_out, quant_after_fee );
      const int64_t ram_bytes = add_ram( receiver, bytes_out );

      system_contract::logbuyram_action logbuyram_act{ get_self(), { {get_self(), active_permission} } };
      system_contract::logsystemfee_action logsystemfee_act{ get_self(), { {get_self(), active_permission} } };

      logbuyram_act.send( payer, receiver, quant, bytes_out, ram_bytes, fee );
      logsystemfee_act.send( ram_account, fee, "buy ram" );

      return action_return_buyram{ payer, receiver, quant, bytes_out, ram_bytes, fee };
   }

   void system_contract::logbuyram( const name& payer, const name& receiver, const asset& quantity, int64_t bytes, int64_t ram_bytes, const asset& fee ) {
      require_auth( get_self() );
      require_recipient(payer);
      require_recipient(receiver);
   }

  /**
    *  Sell purchased RAM at the seller's average cost basis. Tokens return from ra.ram;
    *  the free-byte ledger is credited so the same bytes can be bought again.
    */
   action_return_sellram system_contract::sellram( const name& account, int64_t bytes ) {
      require_auth( account );
      update_ram_supply();
      require_recipient(account);
      check( bytes > 0, "must sell a positive amount" );

      const asset tokens_out = debit_purchased_ram( account, bytes );
      check( tokens_out.amount > 0, "token amount received from selling ram is too low" );

      const int64_t ram_bytes = reduce_ram(account, bytes);

      auto itr = _rammarket.find(ramcore_symbol.raw());
      check( itr != _rammarket.end(), "ram market does not exist" );
      _rammarket.modify( itr, same_payer, [&]( auto& es ) {
         es.base.balance.amount += bytes;
      });

      _gstate.total_ram_bytes_reserved -= static_cast<decltype(_gstate.total_ram_bytes_reserved)>(bytes);
      _gstate.total_ram_stake          -= tokens_out.amount;
      check( _gstate.total_ram_stake >= 0, "error, attempt to unstake more tokens than previously staked" );

      {
         token::transfer_action transfer_act{ token_account, { {ram_account, active_permission}, {account, active_permission} } };
         transfer_act.send( ram_account, account, tokens_out, "sell ram" );
      }

      const asset fee{ 0, core_symbol() };

      system_contract::logsellram_action logsellram_act{ get_self(), { {get_self(), active_permission} } };
      logsellram_act.send( account, tokens_out, bytes, ram_bytes, fee );

      return action_return_sellram{ account, tokens_out, bytes, ram_bytes, fee };
   }

   action_return_buyram system_contract::buyrambsys( const name& payer, const name& receiver, uint32_t bytes ) {
      require_auth( get_self() );
      check( bytes > 0, "must purchase a positive amount" );
      auto itr = _rammarket.find( ramcore_symbol.raw() );
      check( itr != _rammarket.end(), "ram market does not exist" );
      const int64_t ram_reserve = itr->base.balance.amount;
      const int64_t tok_reserve = itr->quote.balance.amount;
      check( tok_reserve > 0, "system ram market quote reserve is empty" );
      const int64_t cost = exchange_state::get_bancor_input( ram_reserve, tok_reserve, bytes );
      const int64_t cost_plus_fee = static_cast<int64_t>( cost / double(0.995) );
      check( cost_plus_fee > 0, "bancor ram cost is zero" );
      return buyramsys( payer, receiver, asset{ cost_plus_fee, core_symbol() } );
   }

   action_return_buyram system_contract::buyramsys( const name& payer, const name& receiver, const asset& quant )
   {
      require_auth( get_self() );
      if ( payer != get_self() ) {
         require_auth( payer );
      }
      update_ram_supply();
      require_recipient( payer );
      require_recipient( receiver );

      check( quant.symbol == core_symbol(), "must buy ram with core token" );
      check( quant.amount > 0, "must purchase a positive amount" );

      asset fee = quant;
      fee.amount = ( fee.amount + 199 ) / 200; // 0.5% fee, same as the original Bancor market
      asset quant_after_fee = quant;
      quant_after_fee.amount -= fee.amount;
      check( quant_after_fee.amount > 0, "ram fee consumes the entire payment" );

      {
         token::transfer_action transfer_act{ token_account, { {payer, active_permission}, {ram_account, active_permission} } };
         transfer_act.send( payer, ram_account, quant_after_fee, "buy ram sys" );
      }
      if ( fee.amount > 0 ) {
         token::transfer_action transfer_act{ token_account, { {payer, active_permission} } };
         transfer_act.send( payer, ramfee_account, fee, "ram fee" );
         channel_to_system_fees( ramfee_account, fee );
      }

      int64_t bytes_out = 0;
      const auto& market = _rammarket.get( ramcore_symbol.raw(), "ram market does not exist" );
      _rammarket.modify( market, same_payer, [&]( auto& es ) {
         bytes_out = es.direct_convert( quant_after_fee, ram_symbol ).amount;
      });
      check( bytes_out > 0, "must reserve a positive amount" );

      _gstate.total_ram_bytes_reserved += uint64_t( bytes_out );
      _gstate.total_ram_stake          += quant_after_fee.amount;

      const int64_t ram_bytes = add_ram( receiver, bytes_out );

      system_contract::logbuyram_action logbuyram_act{ get_self(), { {get_self(), active_permission} } };
      logbuyram_act.send( payer, receiver, quant, bytes_out, ram_bytes, fee );

      return action_return_buyram{ payer, receiver, quant, bytes_out, ram_bytes, fee };
   }

   action_return_sellram system_contract::sellramsys( const name& account, int64_t bytes ) {
      require_auth( get_self() );
      update_ram_supply();
      require_recipient( account );
      check( bytes > 0, "must sell a positive amount" );

      user_resources_table userres( get_self(), account.value );
      auto res_itr = userres.find( account.value );
      check( res_itr != userres.end(), "no resource row" );

      const int64_t purchased = [&]() {
         auto ur = _usersram.find( account.value );
         return ur == _usersram.end() ? int64_t(0) : ur->ram_bytes;
      }();
      check( res_itr->ram_bytes - bytes >= purchased, "cannot sell fixed-price ram via sellramsys" );

      const int64_t ram_bytes = reduce_ram( account, bytes );

      asset tokens_out;
      auto itr = _rammarket.find( ramcore_symbol.raw() );
      check( itr != _rammarket.end(), "ram market does not exist" );
      _rammarket.modify( itr, same_payer, [&]( auto& es ) {
         tokens_out = es.direct_convert( asset( bytes, ram_symbol ), core_symbol() );
      });
      check( tokens_out.amount > 1, "token amount received from selling ram is too low" );

      _gstate.total_ram_bytes_reserved -= static_cast<uint64_t>( bytes );
      _gstate.total_ram_stake          -= tokens_out.amount;
      check( _gstate.total_ram_stake >= 0, "error, attempt to unstake more tokens than previously staked" );

      const int64_t fee_amt = ( tokens_out.amount + 199 ) / 200;
      asset fee{ fee_amt, core_symbol() };
      asset net = tokens_out;
      net.amount -= fee.amount;

      {
         token::transfer_action transfer_act{ token_account, { {ram_account, active_permission} } };
         transfer_act.send( ram_account, account, net, "sell ram sys" );
      }
      if ( fee.amount > 0 ) {
         token::transfer_action transfer_act{ token_account, { {ram_account, active_permission} } };
         transfer_act.send( ram_account, ramfee_account, fee, "sell ram fee" );
         channel_to_system_fees( ramfee_account, fee );
      }

      system_contract::logsellram_action logsellram_act{ get_self(), { {get_self(), active_permission} } };
      logsellram_act.send( account, tokens_out, bytes, ram_bytes, fee );

      return action_return_sellram{ account, tokens_out, bytes, ram_bytes, fee };
   }

   void system_contract::logsellram( const name& account, const asset& quantity, int64_t bytes, int64_t ram_bytes, const asset& fee ) {
      require_auth( get_self() );
      require_recipient(account);
   }

   /**
    * This action will transfer RAM bytes from one account to another.
    */
   action_return_ramtransfer system_contract::ramtransfer( const name& from, const name& to, int64_t bytes, const std::string& memo ) {
      require_auth( from );
      update_ram_supply();
      check( memo.size() <= 256, "memo has more than 256 bytes" );
      check( bytes > 0, "must transfer a positive amount" );
      if ( to != null_account ) {
         assert_under_ram_cap( to, bytes, ram_config() );
      }
      transfer_purchased_ram( from, to, bytes );
      const int64_t from_ram_bytes = reduce_ram( from, bytes );
      const int64_t to_ram_bytes = ( to == null_account ) ? 0 : add_ram( to, bytes );
      if ( to == null_account ) {
         auto itr = _rammarket.find( ramcore_symbol.raw() );
         check( itr != _rammarket.end(), "ram market does not exist" );
         _rammarket.modify( itr, same_payer, [&]( auto& es ) {
            es.base.balance.amount += bytes;
         });
         _gstate.total_ram_bytes_reserved -= static_cast<uint64_t>( bytes );
      }
      require_recipient( from );
      require_recipient( to );

      return action_return_ramtransfer{ from, to, bytes, from_ram_bytes, to_ram_bytes };
   }

   /**
    * This action will burn RAM bytes from owner account.
    */
   action_return_ramtransfer system_contract::ramburn( const name& owner, int64_t bytes, const std::string& memo ) {
      require_auth( owner );
      return ramtransfer( owner, null_account, bytes, memo );
   }

   /**
    * This action will buy and then burn the purchased RAM bytes.
    */
   action_return_buyram system_contract::buyramburn( const name& payer, const asset& quantity, const std::string& memo ) {
      require_auth( payer );
      check( quantity.symbol == core_symbol(), "quantity must be core token" );
      check( quantity.amount > 0, "quantity must be positive" );

      const auto return_buyram = buyram( payer, payer, quantity );
      ramburn( payer, return_buyram.bytes_purchased, memo );

      return return_buyram;
   }

   [[eosio::action]]
   void system_contract::logramchange( const name& owner, int64_t bytes, int64_t ram_bytes )
   {
      require_auth( get_self() );
      require_recipient( owner );
   }

   void system_contract::setramoption( const std::optional<asset>& ram_price_per_byte,
                                      const std::optional<uint64_t>& max_per_user_bytes,
                                      const std::optional<uint16_t>& ram_fee_percent )
   {
      require_auth( get_self() );
      auto cfg = ram_config();
      if ( ram_price_per_byte ) {
         check( ram_price_per_byte->symbol == core_symbol(), "ram price must be core token" );
         check( ram_price_per_byte->amount > 0, "ram price must be positive" );
         cfg.ram_price_per_byte = *ram_price_per_byte;
      }
      if ( max_per_user_bytes ) {
         check( *max_per_user_bytes >= ram_cap_base_bytes, "max per user bytes must be at least 4 MiB" );
         cfg.max_per_user_bytes = *max_per_user_bytes;
      }
      if ( ram_fee_percent ) {
         check( *ram_fee_percent < ram_fee_precision, "ram fee percent must be less than 10000" );
         cfg.ram_fee_percent = *ram_fee_percent;
      }
      _globalram.set( cfg, get_self() );
   }

   void system_contract::ramlimitset( const name& account, int64_t ramlimit )
   {
      require_auth( get_self() );
      check( is_account( account ), "account does not exist" );
      check( ramlimit >= 0, "ram limit must be non-negative" );
      ensure_usersram( account );
      auto itr = _usersram.find( account.value );
      _usersram.modify( itr, same_payer, [&]( auto& row ) {
         row.ramlimit = ramlimit;
      });
   }

   int64_t system_contract::reduce_ram( const name& owner, int64_t bytes ) {
      check( bytes > 0, "cannot reduce negative byte" );
      user_resources_table userres( get_self(), owner.value );
      auto res_itr = userres.find( owner.value );
      check( res_itr != userres.end(), "no resource row" );
      check( res_itr->ram_bytes >= bytes, "insufficient quota" );

      userres.modify( res_itr, same_payer, [&]( auto& res ) {
          res.ram_bytes -= bytes;
      });
      set_resource_ram_bytes_limits( owner );

      // logging
      system_contract::logramchange_action logramchange_act{ get_self(), { {get_self(), active_permission} }};
      logramchange_act.send( owner, -bytes, res_itr->ram_bytes );
      return res_itr->ram_bytes;
   }

   int64_t system_contract::add_ram( const name& owner, int64_t bytes ) {
      check( bytes > 0, "cannot add negative byte" );
      check( is_account(owner), "owner=" + owner.to_string() + " account does not exist");
      user_resources_table userres( get_self(), owner.value );
      auto res_itr = userres.find( owner.value );
      if ( res_itr == userres.end() ) {
         userres.emplace( owner, [&]( auto& res ) {
            res.owner = owner;
            res.net_weight = asset( 0, core_symbol() );
            res.cpu_weight = asset( 0, core_symbol() );
            res.ram_bytes = bytes;
         });
      } else {
         userres.modify( res_itr, same_payer, [&]( auto& res ) {
            res.ram_bytes += bytes;
         });
      }
      set_resource_ram_bytes_limits( owner );

      // logging
      system_contract::logramchange_action logramchange_act{ get_self(), { {get_self(), active_permission} } };
      logramchange_act.send( owner, bytes, res_itr->ram_bytes );
      return res_itr->ram_bytes;
   }

   void system_contract::set_resource_ram_bytes_limits( const name& owner ) {
      user_resources_table userres( get_self(), owner.value );
      auto res_itr = userres.find( owner.value );

      auto voter_itr = _voters.find( owner.value );
      if ( voter_itr == _voters.end() || !has_field( voter_itr->flags1, voter_info::flags1_fields::ram_managed ) ) {
         int64_t ram_bytes, net, cpu;
         get_resource_limits( owner, ram_bytes, net, cpu );
         set_resource_limits( owner, res_itr->ram_bytes + ram_gift_bytes, net, cpu );
      }
   }

   std::pair<int64_t, int64_t> get_b1_vesting_info() {
      const int64_t base_time = 1527811200; /// Friday, June 1, 2018 12:00:00 AM UTC
      const int64_t current_time = 1638921540; /// Tuesday, December 7, 2021 11:59:00 PM UTC
      const int64_t total_vesting = 100'000'000'0000ll;
      const int64_t vested = int64_t(total_vesting * double(current_time - base_time) / (10*seconds_per_year) );
      return { total_vesting, vested };
   }


   void validate_b1_vesting( int64_t new_stake, asset stake_change ) {
      const auto [total_vesting, vested] = get_b1_vesting_info();
      auto unvestable = total_vesting - vested;

      auto hasAlreadyUnvested = new_stake < unvestable 
            && stake_change.amount < 0 
            && new_stake + std::abs(stake_change.amount) < unvestable;
      if(hasAlreadyUnvested) return;

      check( new_stake >= unvestable, "b1 can only claim what has already vested" ); 
   }

   void system_contract::changebw( name from, const name& receiver,
                                   const asset& stake_net_delta, const asset& stake_cpu_delta, bool transfer )
   {
      require_auth( from );
      check( stake_net_delta.amount != 0 || stake_cpu_delta.amount != 0, "should stake non-zero amount" );
      check( std::abs( (stake_net_delta + stake_cpu_delta).amount )
             >= std::max( std::abs( stake_net_delta.amount ), std::abs( stake_cpu_delta.amount ) ),
             "net and cpu deltas cannot be opposite signs" );

      name source_stake_from = from;
      if ( transfer ) {
         from = receiver;
      }

      update_stake_delegated( from, receiver, stake_net_delta, stake_cpu_delta );
      update_user_resources( from, receiver, stake_net_delta, stake_cpu_delta );

      // create refund or update from existing refund
      if ( stake_account != source_stake_from ) { //for eosio both transfer and refund make no sense
         refunds_table refunds_tbl( get_self(), from.value );
         auto req = refunds_tbl.find( from.value );

         //create/update/delete refund
         auto net_balance = stake_net_delta;
         auto cpu_balance = stake_cpu_delta;

         // net and cpu are same sign by assertions in delegatebw and undelegatebw
         // redundant assertion also at start of changebw to protect against misuse of changebw
         bool is_undelegating = (net_balance.amount + cpu_balance.amount ) < 0;
         bool is_delegating_to_self = (!transfer && from == receiver);

         if( is_delegating_to_self || is_undelegating ) {
            if ( req != refunds_tbl.end() ) { //need to update refund
               refunds_tbl.modify( req, same_payer, [&]( refund_request& r ) {
                  if ( net_balance.amount < 0 || cpu_balance.amount < 0 ) {
                     r.request_time = current_time_point();
                  }
                  r.net_amount -= net_balance;
                  if ( r.net_amount.amount < 0 ) {
                     net_balance = -r.net_amount;
                     r.net_amount.amount = 0;
                  } else {
                     net_balance.amount = 0;
                  }
                  r.cpu_amount -= cpu_balance;
                  if ( r.cpu_amount.amount < 0 ){
                     cpu_balance = -r.cpu_amount;
                     r.cpu_amount.amount = 0;
                  } else {
                     cpu_balance.amount = 0;
                  }
               });

               check( 0 <= req->net_amount.amount, "negative net refund amount" ); //should never happen
               check( 0 <= req->cpu_amount.amount, "negative cpu refund amount" ); //should never happen

               if ( req->is_empty() ) {
                  refunds_tbl.erase( req );
               }
            } else if ( net_balance.amount < 0 || cpu_balance.amount < 0 ) { //need to create refund
               refunds_tbl.emplace( from, [&]( refund_request& r ) {
                  r.owner = from;
                  if ( net_balance.amount < 0 ) {
                     r.net_amount = -net_balance;
                     net_balance.amount = 0;
                  } else {
                     r.net_amount = asset( 0, core_symbol() );
                  }
                  if ( cpu_balance.amount < 0 ) {
                     r.cpu_amount = -cpu_balance;
                     cpu_balance.amount = 0;
                  } else {
                     r.cpu_amount = asset( 0, core_symbol() );
                  }
                  r.request_time = current_time_point();
               });
            } // else stake increase requested with no existing row in refunds_tbl -> nothing to do with refunds_tbl
         } /// end if is_delegating_to_self || is_undelegating

         auto transfer_amount = net_balance + cpu_balance;
         if ( 0 < transfer_amount.amount ) {
            token::transfer_action transfer_act{ token_account, { {source_stake_from, active_permission} } };
            transfer_act.send( source_stake_from, stake_account, asset(transfer_amount), "stake bandwidth" );
         }
      }

      vote_stake_updater( from );
      const int64_t staked = update_voting_power( from, stake_net_delta + stake_cpu_delta );
      if ( from == "b1"_n ) {
         validate_b1_vesting( staked, stake_net_delta + stake_cpu_delta );
      }
   }

   void system_contract::update_stake_delegated( const name from, const name receiver, const asset stake_net_delta, const asset stake_cpu_delta )
   {
      del_bandwidth_table del_tbl( get_self(), from.value );
      auto itr = del_tbl.find( receiver.value );
      if( itr == del_tbl.end() ) {
         itr = del_tbl.emplace( from, [&]( auto& dbo ){
               dbo.from          = from;
               dbo.to            = receiver;
               dbo.net_weight    = stake_net_delta;
               dbo.cpu_weight    = stake_cpu_delta;
            });
      } else {
         del_tbl.modify( itr, same_payer, [&]( auto& dbo ){
               dbo.net_weight    += stake_net_delta;
               dbo.cpu_weight    += stake_cpu_delta;
            });
      }
      check( 0 <= itr->net_weight.amount, "insufficient staked net bandwidth" );
      check( 0 <= itr->cpu_weight.amount, "insufficient staked cpu bandwidth" );
      if ( itr->is_empty() ) {
         del_tbl.erase( itr );
      }
   }

   void system_contract::update_user_resources( const name from, const name receiver, const asset stake_net_delta, const asset stake_cpu_delta )
   {
      user_resources_table   totals_tbl( get_self(), receiver.value );
      auto tot_itr = totals_tbl.find( receiver.value );
      if( tot_itr ==  totals_tbl.end() ) {
         tot_itr = totals_tbl.emplace( from, [&]( auto& tot ) {
               tot.owner = receiver;
               tot.net_weight    = stake_net_delta;
               tot.cpu_weight    = stake_cpu_delta;
            });
      } else {
         totals_tbl.modify( tot_itr, from == receiver ? from : same_payer, [&]( auto& tot ) {
               tot.net_weight    += stake_net_delta;
               tot.cpu_weight    += stake_cpu_delta;
            });
      }
      check( 0 <= tot_itr->net_weight.amount, "insufficient staked total net bandwidth" );
      check( 0 <= tot_itr->cpu_weight.amount, "insufficient staked total cpu bandwidth" );

      {
         bool ram_managed = false;
         bool net_managed = false;
         bool cpu_managed = false;

         auto voter_itr = _voters.find( receiver.value );
         if( voter_itr != _voters.end() ) {
            ram_managed = has_field( voter_itr->flags1, voter_info::flags1_fields::ram_managed );
            net_managed = has_field( voter_itr->flags1, voter_info::flags1_fields::net_managed );
            cpu_managed = has_field( voter_itr->flags1, voter_info::flags1_fields::cpu_managed );
         }

         if( !(net_managed && cpu_managed) ) {
            int64_t ram_bytes, net, cpu;
            get_resource_limits( receiver, ram_bytes, net, cpu );

            set_resource_limits( receiver,
                                 ram_managed ? ram_bytes : std::max( tot_itr->ram_bytes + ram_gift_bytes, ram_bytes ),
                                 net_managed ? net : tot_itr->net_weight.amount,
                                 cpu_managed ? cpu : tot_itr->cpu_weight.amount );
         }
      }

      if ( tot_itr->is_empty() ) {
         totals_tbl.erase( tot_itr );
      } // tot_itr can be invalid, should go out of scope
   }

   int64_t system_contract::update_voting_power( const name& voter, const asset& total_update )
   {
      auto voter_itr = _voters.find( voter.value );
      if( voter_itr == _voters.end() ) {
         voter_itr = _voters.emplace( voter, [&]( auto& v ) {
            v.owner  = voter;
            v.staked = total_update.amount;
         });
      } else {
         _voters.modify( voter_itr, same_payer, [&]( auto& v ) {
            v.staked += total_update.amount;
         });
      }

      check( 0 <= voter_itr->staked, "stake for voting cannot be negative" );

      if( voter_itr->producers.size() || voter_itr->proxy ) {
         update_votes( voter, voter_itr->proxy, voter_itr->producers, false );
      }
      return voter_itr->staked;
   }

   void system_contract::delegatebw( const name& from, const name& receiver,
                                     const asset& stake_net_quantity,
                                     const asset& stake_cpu_quantity, bool transfer )
   {
      asset zero_asset( 0, core_symbol() );
      check( stake_cpu_quantity >= zero_asset, "must stake a positive amount" );
      check( stake_net_quantity >= zero_asset, "must stake a positive amount" );
      check( stake_net_quantity.amount + stake_cpu_quantity.amount > 0, "must stake a positive amount" );
      check( !transfer || from != receiver, "cannot use transfer flag if delegating to self" );

      changebw( from, receiver, stake_net_quantity, stake_cpu_quantity, transfer);
   } // delegatebw

   void system_contract::undelegatebw( const name& from, const name& receiver,
                                       const asset& unstake_net_quantity, const asset& unstake_cpu_quantity )
   {
      asset zero_asset( 0, core_symbol() );
      check( unstake_cpu_quantity >= zero_asset, "must unstake a positive amount" );
      check( unstake_net_quantity >= zero_asset, "must unstake a positive amount" );
      check( unstake_cpu_quantity.amount + unstake_net_quantity.amount > 0, "must unstake a positive amount" );
      check( _gstate.thresh_activated_stake_time != time_point(),
             "cannot undelegate bandwidth until the chain is activated (at least 15% of all tokens participate in voting)" );

      changebw( from, receiver, -unstake_net_quantity, -unstake_cpu_quantity, false);
   } // undelegatebw

   void system_contract::refund( const name& owner ) {
      require_auth( owner );

      refunds_table refunds_tbl( get_self(), owner.value );
      auto req = refunds_tbl.find( owner.value );
      check( req != refunds_tbl.end(), "refund request not found" );
      check( req->request_time + seconds(refund_delay_sec) <= current_time_point(),
             "refund is not available yet" );
      token::transfer_action transfer_act{ token_account, { {stake_account, active_permission}, {req->owner, active_permission} } };
      transfer_act.send( stake_account, req->owner, req->net_amount + req->cpu_amount, "unstake" );
      refunds_tbl.erase( req );
   }

   void system_contract::unvest(const name account, const asset unvest_net_quantity, const asset unvest_cpu_quantity)
   {
      require_auth( get_self() );

      check( account == "b1"_n, "only b1 account can unvest");

      check( unvest_cpu_quantity.amount >= 0, "must unvest a positive amount" );
      check( unvest_net_quantity.amount >= 0, "must unvest a positive amount" );

      const auto [total_vesting, vested] = get_b1_vesting_info();
      const asset unvesting = unvest_net_quantity + unvest_cpu_quantity;
      check( unvesting.amount <= total_vesting - vested , "can only unvest what is not already vested");

      // reduce staked from account
      update_voting_power( account, -unvesting );
      update_stake_delegated( account, account, -unvest_net_quantity, -unvest_cpu_quantity );
      update_user_resources( account, account, -unvest_net_quantity, -unvest_cpu_quantity );
      vote_stake_updater( account );

      // transfer unvested tokens to `eosio`
      token::transfer_action transfer_act{ token_account, { {stake_account, active_permission} } };
      transfer_act.send( stake_account, get_self(), unvesting, "unvest" );

      // retire unvested tokens
      token::retire_action retire_act{ token_account, { {"ra"_n, active_permission} } };
      retire_act.send( unvesting, "unvest" );
   } // unvest

} //namespace rasystem
