#include <ra.org/ra.org.hpp>

#include <tuple>
#include <vector>

namespace eosio {

   namespace {

      constexpr int64_t k_day = 86400;

      // Howard Hinnant civil date (UTC).
      int64_t days_from_civil( int y, unsigned m, unsigned d ) {
         y -= m <= 2;
         const int era = ( y >= 0 ? y : y - 399 ) / 400;
         const unsigned yoe = static_cast<unsigned>( y - era * 400 );
         const unsigned doy = ( 153 * ( m + ( m > 2 ? -3 : 9 ) ) + 2 ) / 5 + d - 1;
         const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
         return era * 146097 + static_cast<int>( doe ) - 719468;
      }

      uint32_t unix_from_ymd( int y, unsigned m, unsigned d ) {
         return static_cast<uint32_t>( days_from_civil( y, m, d ) * k_day );
      }

      uint32_t period_end_unix( uint32_t period ) {
         const int y = static_cast<int>( period / 100 );
         const unsigned m = period % 100;
         if ( m == 12 ) {
            return unix_from_ymd( y + 1, 1, 1 );
         }
         return unix_from_ymd( y, m + 1, 1 );
      }

      uint32_t now_unix() {
         return current_time_point().sec_since_epoch();
      }

   } // namespace

   org::config_row org::get_config() const {
      config_singleton cfg( get_self(), get_self().value );
      check( cfg.exists(), "config not set; call setconfig" );
      return cfg.get();
   }

   void org::set_cfg( const config_row& c ) {
      config_singleton cfg( get_self(), get_self().value );
      cfg.set( c, get_self() );
   }

   org::stat_row org::get_stat() const {
      stat_singleton st( get_self(), get_self().value );
      check( st.exists(), "stat not set; call setconfig" );
      return st.get();
   }

   void org::set_st( const stat_row& s ) {
      stat_singleton st( get_self(), get_self().value );
      st.set( s, get_self() );
   }

   bool org::is_admin( const name& account ) const {
      if ( account == get_self() ) {
         return true;
      }
      admins_table admins( get_self(), get_self().value );
      return admins.find( account.value ) != admins.end();
   }

   void org::require_admin() const {
      if ( has_auth( get_self() ) ) {
         return;
      }
      admins_table admins( get_self(), get_self().value );
      for ( const auto& row : admins ) {
         if ( has_auth( row.account ) ) {
            return;
         }
      }
      check( false, "missing authority of an admin or ra.org" );
   }

   bool org::has_seat( uint64_t role_id, const name& worker ) const {
      seats_table seats( get_self(), get_self().value );
      auto idx = seats.get_index< "byworker"_n >();
      for ( auto itr = idx.lower_bound( worker.value ); itr != idx.end() && itr->worker == worker; ++itr ) {
         if ( itr->role_id == role_id ) {
            return true;
         }
      }
      return false;
   }

   uint32_t org::count_seats( uint64_t role_id ) const {
      seats_table seats( get_self(), get_self().value );
      auto idx = seats.get_index< "byrole"_n >();
      uint32_t n = 0;
      for ( auto itr = idx.lower_bound( role_id ); itr != idx.end() && itr->role_id == role_id; ++itr ) {
         ++n;
      }
      return n;
   }

   uint32_t org::count_peers( uint64_t peer_role, const name& worker ) const {
      seats_table seats( get_self(), get_self().value );
      auto idx = seats.get_index< "byrole"_n >();
      uint32_t n = 0;
      for ( auto itr = idx.lower_bound( peer_role ); itr != idx.end() && itr->role_id == peer_role; ++itr ) {
         if ( itr->worker != worker && !is_admin( itr->worker ) ) {
            ++n;
         }
      }
      return n;
   }

   void org::check_period( uint32_t period ) const {
      const uint32_t year  = period / 100;
      const uint32_t month = period % 100;
      check( year >= 1970 && year <= 3000, "period year is out of range" );
      check( month >= 1 && month <= 12, "period month must be 1-12" );
   }

   void org::check_salary( const asset& quantity, const symbol& buck ) const {
      check( quantity.is_valid(), "salary is invalid" );
      check( quantity.symbol == buck, "salary must be in bucks" );
      check( quantity.amount >= 0, "salary cannot be negative" );
   }

   void org::write_role( role& row, const string& kpi_name, const name& kpi_type,
                           const asset& base_salary, const asset& bucket_salary,
                           uint16_t boost, uint64_t review_role, const symbol& buck ) const {
      check( kpi_name.size() > 0 && kpi_name.size() <= 64, "kpi name must be 1..64 bytes" );
      check( kpi_type.value != 0, "kpi type required" );
      check_salary( base_salary, buck );
      check_salary( bucket_salary, buck );
      check( boost <= 10000, "boost cannot exceed 10000 percent" );
      if ( review_role != 0 ) {
         roles_table roles( get_self(), get_self().value );
         check( roles.find( review_role ) != roles.end(), "review role not found" );
      }
      row.kpi_name      = kpi_name;
      row.kpi_type      = kpi_type;
      row.base_salary   = base_salary;
      row.bucket_salary = bucket_salary;
      row.boost         = boost;
      row.review_role   = review_role;
   }

   void org::add_owed( const name& owner, const asset& quantity ) {
      if ( quantity.amount <= 0 ) {
         return;
      }
      accounts_table accounts( get_self(), get_self().value );
      auto itr = accounts.find( owner.value );
      if ( itr == accounts.end() ) {
         accounts.emplace( get_self(), [&]( auto& row ) {
            row.owner = owner;
            row.owed  = quantity;
         });
      } else {
         accounts.modify( itr, same_payer, [&]( auto& row ) {
            row.owed += quantity;
         });
      }
      auto st = get_stat();
      st.owed += quantity;
      set_st( st );
   }

   name org::infer_kind( const name& reviewer, const name& worker, uint64_t role_id, uint64_t review_role ) const {
      if ( reviewer == worker ) {
         check( has_seat( role_id, worker ), "worker does not hold this role" );
         return "self"_n;
      }
      if ( is_admin( reviewer ) ) {
         return "manager"_n;
      }
      const uint64_t peer_role = review_role == 0 ? role_id : review_role;
      check( has_seat( peer_role, reviewer ), "reviewer is not a peer for this role" );
      return "peer"_n;
   }

   // === Set Config === //
   void org::setconfig( const name& buck_contract, const symbol& buck )
   {
      require_auth( get_self() );
      check( is_account( buck_contract ), "buck contract account does not exist" );
      check( buck.is_valid(), "invalid buck symbol" );

      config_singleton cfg( get_self(), get_self().value );
      auto c = cfg.get_or_default();
      if ( c.buck.is_valid() && ( c.buck != buck || c.buck_contract != buck_contract ) ) {
         auto st = get_stat();
         check( st.owed.amount == 0, "cannot change buck while backpay is owed" );
      }
      c.buck_contract = buck_contract;
      c.buck         = buck;
      if ( c.open_days == 0 )  c.open_days  = 3;
      if ( c.grace_days == 0 ) c.grace_days = 3;
      if ( c.next_role_id == 0 ) c.next_role_id = 1;
      if ( c.next_seat_id == 0 ) c.next_seat_id = 1;
      cfg.set( c, get_self() );

      stat_singleton st( get_self(), get_self().value );
      auto s = st.get_or_default();
      s.owed = asset{ s.owed.symbol == buck ? s.owed.amount : 0, buck };
      s.paid = asset{ s.paid.symbol == buck ? s.paid.amount : 0, buck };
      st.set( s, get_self() );
   }//END setconfig()

   // === Admins === //
   void org::addadmin( const name& account )
   {
      require_admin();
      check( is_account( account ), "account does not exist" );
      check( account != get_self(), "ra.org is already an admin" );
      admins_table admins( get_self(), get_self().value );
      check( admins.find( account.value ) == admins.end(), "admin already added" );
      admins.emplace( get_self(), [&]( auto& row ) {
         row.account = account;
      });
   }//END addadmin()

   void org::rmadmin( const name& account )
   {
      require_admin();
      admins_table admins( get_self(), get_self().value );
      auto itr = admins.require_find( account.value, "admin not found" );
      admins.erase( itr );
   }//END rmadmin()

   // === Roles === //
   void org::addrole( const string& kpi_name, const name& kpi_type,
                        const asset& base_salary, const asset& bucket_salary,
                        uint16_t boost, uint64_t review_role )
   {
      require_admin();
      auto c = get_config();
      roles_table roles( get_self(), get_self().value );
      roles.emplace( get_self(), [&]( auto& row ) {
         row.id = c.next_role_id++;
         write_role( row, kpi_name, kpi_type, base_salary, bucket_salary, boost, review_role, c.buck );
      });
      set_cfg( c );
   }//END addrole()

   void org::editrole( uint64_t role_id, const string& kpi_name, const name& kpi_type,
                         const asset& base_salary, const asset& bucket_salary,
                         uint16_t boost, uint64_t review_role )
   {
      require_admin();
      auto c = get_config();
      roles_table roles( get_self(), get_self().value );
      auto itr = roles.require_find( role_id, "role not found" );
      roles.modify( itr, same_payer, [&]( auto& row ) {
         write_role( row, kpi_name, kpi_type, base_salary, bucket_salary, boost, review_role, c.buck );
      });
   }//END editrole()

   void org::grantrole( uint64_t role_id, const name& worker )
   {
      require_admin();
      check( is_account( worker ), "worker account does not exist" );
      auto c = get_config();
      roles_table roles( get_self(), get_self().value );
      roles.require_find( role_id, "role not found" );
      check( !has_seat( role_id, worker ), "worker already has this role" );

      seats_table seats( get_self(), get_self().value );
      seats.emplace( get_self(), [&]( auto& row ) {
         row.id      = c.next_seat_id++;
         row.role_id = role_id;
         row.worker  = worker;
         row.granted = current_time_point();
      });
      set_cfg( c );
   }//END grantrole()

   void org::revokerole( uint64_t role_id, const name& worker )
   {
      require_admin();
      seats_table seats( get_self(), get_self().value );
      auto idx = seats.get_index< "byworker"_n >();
      for ( auto itr = idx.lower_bound( worker.value ); itr != idx.end() && itr->worker == worker; ++itr ) {
         if ( itr->role_id == role_id ) {
            idx.erase( itr );
            return;
         }
      }
      check( false, "worker does not hold this role" );
   }//END revokerole()

   // === Review === //
   void org::review( const name& reviewer, uint32_t period, uint64_t role_id,
                       const name& worker, uint8_t rating, const string& proof )
   {
      require_auth( reviewer );
      check_period( period );
      check( rating <= RATING_MAX, "rating must be 0-10" );
      check( proof.size() <= 256, "proof has more than 256 bytes" );
      check( is_account( worker ), "worker account does not exist" );

      const auto c = get_config();
      const uint32_t now = now_unix();
      const uint32_t end = period_end_unix( period );
      check( now >= end - uint32_t( c.open_days ) * k_day, "review window has not opened" );
      check( now <  end + uint32_t( c.grace_days ) * k_day, "review window has closed" );

      periods_table periods( get_self(), get_self().value );
      check( periods.find( period ) == periods.end(), "period is already closed" );

      roles_table roles( get_self(), get_self().value );
      const auto& role_row = roles.get( role_id, "role not found" );
      check( has_seat( role_id, worker ), "worker does not hold this role" );
      const name kind = infer_kind( reviewer, worker, role_id, role_row.review_role );

      reviews_table reviews( get_self(), uint64_t( period ) );
      auto idx = reviews.get_index< "bytarget"_n >();
      const uint128_t key = ( uint128_t( role_id ) << 64 ) | worker.value;
      for ( auto itr = idx.lower_bound( key ); itr != idx.end() && itr->role_id == role_id && itr->worker == worker; ++itr ) {
         if ( itr->reviewer == reviewer ) {
            idx.modify( itr, same_payer, [&]( auto& row ) {
               row.kind   = kind;
               row.rating = rating;
               row.proof  = proof;
            });
            return;
         }
      }
      reviews.emplace( get_self(), [&]( auto& row ) {
         row.id       = reviews.available_primary_key();
         row.role_id  = role_id;
         row.worker   = worker;
         row.reviewer = reviewer;
         row.kind     = kind;
         row.rating   = rating;
         row.proof    = proof;
      });
   }//END review()

   // === Close Period === //
   void org::closeperiod( uint32_t period )
   {
      check_period( period );
      const auto c = get_config();
      const uint32_t now = now_unix();
      const uint32_t end = period_end_unix( period );
      check( now >= end + uint32_t( c.grace_days ) * k_day, "review grace has not ended" );

      periods_table periods( get_self(), get_self().value );
      check( periods.find( period ) == periods.end(), "period is already closed" );

      roles_table roles( get_self(), get_self().value );
      seats_table seats( get_self(), get_self().value );
      reviews_table reviews( get_self(), uint64_t( period ) );
      auto by_target = reviews.get_index< "bytarget"_n >();

      for ( const auto& seat_row : seats ) {
         if ( uint32_t( seat_row.granted.sec_since_epoch() ) >= end ) {
            continue;
         }
         const auto& role_row = roles.get( seat_row.role_id, "role not found" );

         int64_t self_rating = -1;
         int64_t mgr_sum = 0;
         uint32_t mgr_n = 0;
         int64_t peer_sum = 0;
         uint32_t peer_n = 0;

         const uint128_t key = ( uint128_t( seat_row.role_id ) << 64 ) | seat_row.worker.value;
         for ( auto itr = by_target.lower_bound( key );
               itr != by_target.end() && itr->role_id == seat_row.role_id && itr->worker == seat_row.worker;
               ++itr ) {
            if ( itr->kind == "self"_n ) {
               self_rating = itr->rating;
            } else if ( itr->kind == "manager"_n ) {
               mgr_sum += itr->rating;
               ++mgr_n;
            } else if ( itr->kind == "peer"_n ) {
               peer_sum += itr->rating;
               ++peer_n;
            }
         }

         if ( self_rating < 0 || mgr_n == 0 ) {
            continue;
         }
         const uint64_t peer_role = role_row.review_role == 0 ? role_row.id : role_row.review_role;
         const uint32_t eligible_peers = count_peers( peer_role, seat_row.worker );
         if ( peer_n == 0 && eligible_peers > 0 ) {
            continue;
         }

         const int64_t self_term = int64_t( REVIEW_BPS ) * self_rating / RATING_MAX;
         const int64_t mgr_term  = int64_t( REVIEW_BPS ) * mgr_sum / ( int64_t( mgr_n ) * RATING_MAX );
         const int64_t peer_term  = ( peer_n == 0 )
            ? int64_t( REVIEW_BPS )
            : int64_t( REVIEW_BPS ) * peer_sum / ( int64_t( peer_n ) * RATING_MAX );
         const int64_t weight = int64_t( GUARANTEED_BPS ) + self_term + mgr_term + peer_term;

         const uint32_t n = count_seats( role_row.id );
         const int64_t boosted = role_row.base_salary.amount
            + role_row.base_salary.amount * int64_t( role_row.boost ) / 100;
         const int64_t bucket  = n > 0 ? role_row.bucket_salary.amount / int64_t( n ) : 0;
         const int64_t start   = boosted + bucket;
         if ( start <= 0 ) {
            continue;
         }
         const int64_t owed_amt = static_cast<int64_t>( ( __int128 )start * weight / 10000 );
         add_owed( seat_row.worker, asset{ owed_amt, c.buck } );
      }

      periods.emplace( get_self(), [&]( auto& row ) {
         row.yyyymm = period;
      });
   }//END closeperiod()

   // === Trickle === //
   void org::on_transfer( const name& from, const name& to, const asset& quantity, const string& memo )
   {
      (void)memo;
      if ( to != get_self() || from == get_self() ) {
         return;
      }
      config_singleton cfg( get_self(), get_self().value );
      if ( !cfg.exists() ) {
         return;
      }
      const auto c = cfg.get();
      if ( get_first_receiver() != c.buck_contract || quantity.symbol != c.buck ) {
         return;
      }
      check( quantity.amount > 0, "quantity must be positive" );

      auto st = get_stat();
      if ( st.owed.amount <= 0 ) {
         return;
      }

      accounts_table accounts( get_self(), get_self().value );
      struct pay_item { name to; int64_t amount; };
      std::vector<pay_item> pays;
      pays.reserve( 8 );

      const int64_t pool  = quantity.amount;
      const int64_t total = st.owed.amount;
      int64_t paid_sum = 0;

      for ( auto itr = accounts.begin(); itr != accounts.end(); ++itr ) {
         if ( itr->owed.amount <= 0 ) {
            continue;
         }
         int64_t share = static_cast<int64_t>( ( __int128 )pool * itr->owed.amount / total );
         if ( share > itr->owed.amount ) {
            share = itr->owed.amount;
         }
         if ( share <= 0 || !is_account( itr->owner ) ) {
            continue;
         }
         pays.push_back( { itr->owner, share } );
         accounts.modify( itr, same_payer, [&]( auto& row ) {
            row.owed.amount -= share;
         });
         paid_sum += share;
      }

      st.owed.amount -= paid_sum;
      st.paid.amount += paid_sum;
      set_st( st );

      for ( const auto& p : pays ) {
         action( permission_level{ get_self(), "active"_n }, c.buck_contract, "transfer"_n,
                 std::make_tuple( get_self(), p.to, asset{ p.amount, c.buck }, string( "backpay" ) ) )
            .send();
      }
   }//END on_transfer()

} // namespace eosio
