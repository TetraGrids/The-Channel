#include <ra.authex/ra.authex.hpp>
#include <ra.authex/recover.hpp>

namespace eosio {

   uint64_t authex::expected_nonce( const name& account ) const {
      nonces_table nonces{ get_self(), get_self().value };
      auto itr = nonces.find( account.value );
      return itr == nonces.end() ? 0 : itr->next_nonce;
   }

   void authex::bump_nonce( const name& account, const name& ram_payer ) {
      nonces_table nonces{ get_self(), get_self().value };
      auto itr = nonces.find( account.value );
      if ( itr == nonces.end() ) {
         nonces.emplace( ram_payer, [&]( auto& row ) {
            row.account    = account;
            row.next_nonce = 1;
         });
      } else {
         nonces.modify( itr, same_payer, [&]( auto& row ) {
            row.next_nonce += 1;
         });
      }
   }

   void authex::add_link( const name& account, const name& chain, const std::string& address,
                          const vector<char>& packedkey, const name& ram_payer )
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
         row.id        = st.next_id++;
         row.account   = account;
         row.chain     = chain;
         row.address   = address;
         row.created   = current_time_point();
         row.packedkey = packedkey;
      });
      sing.set( st, get_self() );
   }

   void authex::createlink( const name& account, const name& chain, const std::string& address )
   {
      require_auth( account );
      add_link( account, chain, address, {}, account );
   }

   void authex::adminlink( const name& account, const name& chain, const std::string& address )
   {
      require_auth( get_self() );
      add_link( account, chain, address, {}, get_self() );
   }

   void authex::linksig( const name& account, const name& chain, const std::string& address,
                         const std::string& pubkey, uint64_t nonce, const vector<char>& sig )
   {
      require_auth( account );
      check( pubkey.size() > 0 && pubkey.size() <= 256, "pubkey must be 1..256 bytes" );
      check( nonce == expected_nonce( account ), "wrong nonce" );

      const string msg = pubkey + "|" + account.to_string() + "|" + chain.to_string() + "|"
                         + std::to_string( nonce ) + "|createlink auth";
      auto packed = recover_packed_key( sha256_msg( msg ), sig );
      check( packed.size() > 0, "recovered key empty" );

      add_link( account, chain, address, packed, account );
      bump_nonce( account, account );
   }

   void authex::unlink( uint64_t id )
   {
      links_table links{ get_self(), get_self().value };
      auto itr = links.require_find( id, "link does not exist" );
      check( has_auth( itr->account ) || has_auth( get_self() ), "missing authority of account or ra.authex" );
      links.erase( itr );
   }

} // namespace eosio
