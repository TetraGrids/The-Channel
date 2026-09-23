#include <ra.claim/ra.claim.hpp>
#include <ra.authex/ra.authex.hpp>
#include <ra.authex/recover.hpp>

namespace eosio {

   namespace {

      std::string checksum256_hex( const checksum256& h ) {
         static constexpr char lut[] = "0123456789abcdef";
         const auto b = h.extract_as_byte_array();
         std::string out( 64, '0' );
         for ( uint32_t i = 0; i < 32; ++i ) {
            out[2 * i]     = lut[( b[i] >> 4 ) & 0xf];
            out[2 * i + 1] = lut[b[i] & 0xf];
         }
         return out;
      }

   } // namespace

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

   bool claimc::linked_key_matches( const name& account, const name& chain, const vector<char>& packed ) const {
      authex::links_table links{ authex_account, authex_account.value };
      auto idx = links.get_index< "byaccount"_n >();
      for ( auto itr = idx.lower_bound( account.value ); itr != idx.end() && itr->account == account; ++itr ) {
         if ( itr->chain == chain && !itr->packedkey.empty() && itr->packedkey == packed ) {
            return true;
         }
      }
      return false;
   }

   void claimc::pay_claim( deposits_table& deposits, uint64_t id, const asset& payout ) {
      auto itr = deposits.require_find( id, "deposit not found" );
      check( !itr->claimed, "deposit already claimed" );
      check( payout.symbol == itr->quantity.symbol, "payout symbol mismatch" );
      check( payout.amount > 0 && payout.amount <= itr->quantity.amount, "payout out of range" );

      token::transfer_action xfer{ token_account, { { get_self(), "active"_n } } };
      xfer.send( get_self(), itr->recipient, payout, "cross-chain claim" );

      deposits.modify( itr, same_payer, [&]( auto& row ) {
         row.claimed = true;
      });
   }

   void claimc::emplace_deposit( const name& chain, const checksum256& ext_txid, const name& recipient,
                                 const asset& quantity, const std::string& pubkey, const vector<char>& packedkey ) {
      check( chain.value != 0, "chain name required" );
      check( recipient.value != 0, "recipient name required" );
      check( quantity.amount > 0, "quantity must be positive" );
      check( quantity.is_valid(), "invalid quantity" );

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
         row.pubkey    = pubkey;
         row.packedkey = packedkey;
      });
      sing.set( st, get_self() );
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
      emplace_deposit( chain, ext_txid, recipient, quantity, {}, {} );
   }

   void claimc::claim( uint64_t id )
   {
      deposits_table deposits{ get_self(), get_self().value };
      auto itr = deposits.require_find( id, "deposit not found" );
      require_auth( itr->recipient );
      pay_claim( deposits, id, itr->quantity );
   }

   void claimc::claimsig( uint64_t id, const vector<char>& sig )
   {
      deposits_table deposits{ get_self(), get_self().value };
      auto itr = deposits.require_find( id, "deposit not found" );
      check( !itr->claimed, "deposit already claimed" );

      const string msg = checksum256_hex( itr->ext_txid ) + "|" + itr->recipient.to_string() + "|"
                         + itr->quantity.to_string() + "|" + itr->chain.to_string() + "|"
                         + std::to_string( itr->id ) + "|claim auth";
      auto packed = recover_packed_key( sha256_msg( msg ), sig );
      check( linked_key_matches( itr->recipient, itr->chain, packed ), "signature does not match a linked key" );

      pay_claim( deposits, id, itr->quantity );
   }

   void claimc::creditopen( const name& chain, const checksum256& ext_txid, const name& recipient,
                            const asset& quantity, const std::string& pubkey, const vector<char>& packedkey )
   {
      require_relayer();
      check( !is_account( recipient ), "recipient already exists; use credit" );
      // ra.system only lets a non-system creator open a 12-character name with no dot.
      check( recipient.length() == 12, "recipient must be 12 characters" );
      check( recipient.to_string().find( '.' ) == std::string::npos, "recipient cannot contain a dot" );
      check( pubkey.size() > 0 && pubkey.size() <= 256, "pubkey must be 1..256 bytes" );
      check( packedkey.size() >= 8 && packedkey.size() <= 128, "packed key must be 8..128 bytes" );
      emplace_deposit( chain, ext_txid, recipient, quantity, pubkey, packedkey );
   }

   void claimc::cfgclaim( uint32_t ram_bytes, const asset& ram_fee )
   {
      require_auth( get_self() );
      check( ram_bytes <= 1024 * 1024, "ram_bytes too large" );
      check( ram_fee.amount >= 0, "ram fee must be non-negative" );
      if ( ram_fee.amount > 0 ) {
         check( ram_fee.is_valid(), "invalid ram fee" );
      }
      cfg_singleton cfg{ get_self(), get_self().value };
      cfg.set( claim_cfg{ ram_bytes, ram_fee }, get_self() );
   }

   namespace {

      void append_bytes( std::vector<char>& out, const char* p, size_t n ) {
         out.insert( out.end(), p, p + n );
      }

      void append_u16( std::vector<char>& out, uint16_t v ) {
         char b[2] = { static_cast<char>( v & 0xff ), static_cast<char>( ( v >> 8 ) & 0xff ) };
         append_bytes( out, b, 2 );
      }

      void append_u32( std::vector<char>& out, uint32_t v ) {
         char b[4];
         for ( int i = 0; i < 4; ++i ) b[i] = static_cast<char>( ( v >> ( 8 * i ) ) & 0xff );
         append_bytes( out, b, 4 );
      }

      void append_u64( std::vector<char>& out, uint64_t v ) {
         char b[8];
         for ( int i = 0; i < 8; ++i ) b[i] = static_cast<char>( ( v >> ( 8 * i ) ) & 0xff );
         append_bytes( out, b, 8 );
      }

      void append_varuint( std::vector<char>& out, uint32_t v ) {
         do {
            uint8_t b = static_cast<uint8_t>( v & 0x7f );
            v >>= 7;
            if ( v ) b |= 0x80;
            out.push_back( static_cast<char>( b ) );
         } while ( v );
      }

      void append_authority( std::vector<char>& out, const std::vector<char>& packed_key ) {
         append_u32( out, 1 );
         append_varuint( out, 1 );
         append_bytes( out, packed_key.data(), packed_key.size() );
         append_u16( out, 1 );
         append_varuint( out, 0 );
         append_varuint( out, 0 );
      }

   } // namespace

   void claimc::claimopen( uint64_t id, const vector<char>& sig )
   {
      deposits_table deposits{ get_self(), get_self().value };
      auto itr = deposits.require_find( id, "deposit not found" );
      check( !itr->claimed, "deposit already claimed" );
      check( !itr->packedkey.empty() && !itr->pubkey.empty(), "deposit is not a lazy-account credit" );
      check( !is_account( itr->recipient ), "recipient already exists; use claim or claimsig" );

      const string msg = itr->pubkey + "|" + checksum256_hex( itr->ext_txid ) + "|" + itr->recipient.to_string() + "|"
                         + itr->quantity.to_string() + "|" + itr->chain.to_string() + "|"
                         + std::to_string( itr->id ) + "|claimopen auth";
      auto packed = recover_packed_key( sha256_msg( msg ), sig );
      check( packed == itr->packedkey, "signature does not match the credited key" );

      // === Create the account with the recovered key === //
      std::vector<char> data;
      append_u64( data, get_self().value );
      append_u64( data, itr->recipient.value );
      append_authority( data, itr->packedkey );
      append_authority( data, itr->packedkey );

      action create_act;
      create_act.account = name{ "ra" };
      create_act.name    = "newaccount"_n;
      create_act.authorization.push_back( permission_level{ get_self(), "active"_n } );
      create_act.data = std::move( data );
      create_act.send();

      cfg_singleton cfg{ get_self(), get_self().value };
      claim_cfg c = cfg.get_or_default();
      if ( c.ram_bytes > 0 ) {
         action(
            permission_level{ get_self(), "active"_n },
            name{ "ra" },
            "buyrambytes"_n,
            std::make_tuple( get_self(), itr->recipient, c.ram_bytes )
         ).send();
      }

      asset payout = itr->quantity;
      if ( c.ram_fee.amount > 0 ) {
         check( c.ram_fee.symbol == itr->quantity.symbol, "ram fee symbol mismatch" );
         check( itr->quantity.amount > c.ram_fee.amount, "deposit does not cover account ram" );
         payout -= c.ram_fee;
      }

      std::string address = itr->pubkey.size() <= 256 ? itr->pubkey : itr->pubkey.substr( 0, 256 );
      action(
         permission_level{ get_self(), "active"_n },
         authex_account,
         "bridgelink"_n,
         std::make_tuple( itr->recipient, itr->chain, address, itr->packedkey )
      ).send();

      pay_claim( deposits, id, payout );
   }

} // namespace eosio
