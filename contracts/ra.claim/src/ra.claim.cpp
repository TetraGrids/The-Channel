#include <ra.claim/ra.claim.hpp>

namespace eosio {

   void claimc::require_relayer() const {
      if ( has_auth( get_self() ) ) {
         return;
      }
      relayers_table relayers{ get_self(), get_self().value };
      bool ok = false;
      for ( const auto& row : relayers ) {
         if ( has_auth( row.account ) ) {
            ok = true;
            break;
         }
      }
      check( ok, "missing authority of a relayer or ra.claim" );
   }

   void claimc::addrelayer( const name& account )
   {
      require_auth( get_self() );
      check( is_account( account ), "account does not exist" );
      relayers_table relayers{ get_self(), get_self().value };
      check( relayers.find( account.value ) == relayers.end(), "relayer already added" );
      relayers.emplace( get_self(), [&]( auto& row ) {
         row.account = account;
      });
   }

   void claimc::rmrelayer( const name& account )
   {
      require_auth( get_self() );
      relayers_table relayers{ get_self(), get_self().value };
      auto itr = relayers.require_find( account.value, "relayer not found" );
      relayers.erase( itr );
   }

   void claimc::credit( const name& chain, const checksum256& ext_txid, const name& recipient, const asset& quantity )
   {
      require_relayer();
      check( is_account( recipient ), "recipient account does not exist" );
      check( chain.value != 0, "chain name required" );
      check( quantity.amount > 0, "quantity must be positive" );

      deposits_table deposits{ get_self(), get_self().value };
      const uint64_t prefix = [&]() {
         const auto bytes = ext_txid.extract_as_byte_array();
         uint64_t v = 0;
         for ( size_t i = 0; i < 8; ++i ) {
            v = ( v << 8 ) | bytes[i];
         }
         return v;
      }();

      auto idx = deposits.get_index< "bytxid"_n >();
      for ( auto itr = idx.lower_bound( prefix ); itr != idx.end() && itr->by_txid() == prefix; ++itr ) {
         check( itr->ext_txid != ext_txid, "external transaction already credited" );
      }

      state_singleton sing{ get_self(), get_self().value };
      auto st = sing.get_or_default();
      if ( st.next_id == 0 ) st.next_id = 1;

      deposits.emplace( get_self(), [&]( auto& row ) {
         row.id        = st.next_id++;
         row.ext_txid  = ext_txid;
         row.chain     = chain;
         row.recipient = recipient;
         row.quantity  = quantity;
         row.claimed   = false;
         row.created   = current_time_point();
      });
      sing.set( st, get_self() );
   }

   void claimc::claim( uint64_t id )
   {
      deposits_table deposits{ get_self(), get_self().value };
      auto itr = deposits.require_find( id, "deposit not found" );
      require_auth( itr->recipient );
      check( !itr->claimed, "deposit already claimed" );

      token::transfer_action xfer{ token_account, { { get_self(), "active"_n } } };
      xfer.send( get_self(), itr->recipient, itr->quantity, "cross-chain claim" );

      deposits.modify( itr, same_payer, [&]( auto& row ) {
         row.claimed = true;
      });
   }

} // namespace eosio
