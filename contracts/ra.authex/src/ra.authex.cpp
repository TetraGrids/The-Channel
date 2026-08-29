#include <ra.authex/ra.authex.hpp>

namespace eosio {

   void authex::add_link( const name& account, const name& chain, const std::string& address, const name& ram_payer )
   {
      check( is_account( account ), "account does not exist" );
      check( chain.value != 0, "chain name required" );
      check( address.size() > 0 && address.size() <= 128, "address must be 1..128 bytes" );

      links_table links{ get_self(), get_self().value };
      auto idx = links.get_index< "byaccount"_n >();
      for ( auto itr = idx.lower_bound( account.value ); itr != idx.end() && itr->account == account; ++itr ) {
         check( itr->chain != chain || itr->address != address, "link already exists" );
      }

      state_singleton sing{ get_self(), get_self().value };
      auto st = sing.get_or_default();
      if ( st.next_id == 0 ) st.next_id = 1;

      links.emplace( ram_payer, [&]( auto& row ) {
         row.id       = st.next_id++;
         row.account  = account;
         row.chain    = chain;
         row.address  = address;
         row.created  = current_time_point();
      });
      sing.set( st, get_self() );
   }

   void authex::createlink( const name& account, const name& chain, const std::string& address )
   {
      require_auth( account );
      add_link( account, chain, address, account );
   }

   void authex::adminlink( const name& account, const name& chain, const std::string& address )
   {
      require_auth( get_self() );
      add_link( account, chain, address, get_self() );
   }

   void authex::unlink( uint64_t id )
   {
      links_table links{ get_self(), get_self().value };
      auto itr = links.require_find( id, "link does not exist" );
      check( has_auth( itr->account ) || has_auth( get_self() ), "missing authority of account or ra.authex" );
      links.erase( itr );
   }

} // namespace eosio
