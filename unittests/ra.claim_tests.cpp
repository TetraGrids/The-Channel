#include <eosio/chain/abi_serializer.hpp>
#include <eosio/testing/tester.hpp>

#include <fc/crypto/hex.hpp>
#include <fc/io/fstream.hpp>
#include <fc/io/raw.hpp>
#include <fc/variant_object.hpp>

#include <boost/test/unit_test.hpp>

#include <contracts.hpp>
#include <test_contracts.hpp>

#include <filesystem>

using namespace eosio::testing;
using namespace eosio;
using namespace eosio::chain;
using namespace fc;

using mvo = fc::mutable_variant_object;

static bool load_core_contract( tester& chain, const name& account, const std::string& stem ) {
#ifdef CHANNEL_CONTRACTS_DIR
   const std::string base = std::string( CHANNEL_CONTRACTS_DIR ) + "/" + stem + "/" + stem;
   if ( !std::filesystem::exists( base + ".wasm" ) || !std::filesystem::exists( base + ".abi" ) )
      return false;
   std::string wasm;
   std::string abi;
   fc::read_file_contents( base + ".wasm", wasm );
   fc::read_file_contents( base + ".abi", abi );
   chain.set_code( account, std::vector<uint8_t>( wasm.begin(), wasm.end() ) );
   chain.set_abi( account, abi );
   return true;
#else
   (void)chain;
   (void)account;
   (void)stem;
   return false;
#endif
}

static std::string checksum_hex( const checksum256_type& h ) {
   return h.str();
}

static std::string link_message( const std::string& pubkey, const name& account, const name& chain, uint64_t nonce ) {
   return pubkey + "|" + account.to_string() + "|" + chain.to_string() + "|" + std::to_string( nonce ) + "|createlink auth";
}

static std::string claim_message( const checksum256_type& ext_txid, const name& recipient, const asset& quantity,
                                  const name& chain, uint64_t id ) {
   return checksum_hex( ext_txid ) + "|" + recipient.to_string() + "|" + quantity.to_string() + "|"
          + chain.to_string() + "|" + std::to_string( id ) + "|claim auth";
}

static std::vector<char> sign_msg( const private_key_type& priv, const std::string& msg ) {
   const auto digest = fc::sha256::hash( msg );
   return fc::raw::pack( priv.sign( digest ) );
}

struct authex_claim_tester : tester {
   authex_claim_tester() : tester( setup_policy::preactivate_feature_and_new_bios ) {
      const auto& pfm = control->get_protocol_feature_manager();
      const auto& d = pfm.get_builtin_digest( builtin_protocol_feature_t::em_ed_keys );
      BOOST_REQUIRE( d );
      activate_protocol_features( {*d} );
      produce_block();

      create_accounts( {"alice"_n, "bob"_n, "relayer"_n, "ra.token"_n, "ra.authex"_n, "ra.claim"_n} );
      produce_block();

      set_code( "ra.token"_n, test_contracts::eosio_token_wasm() );
      set_abi( "ra.token"_n, test_contracts::eosio_token_abi() );
      produce_block();

      core_contracts = load_core_contract( *this, "ra.authex"_n, "ra.authex" )
                       && load_core_contract( *this, "ra.claim"_n, "ra.claim" );
      produce_block();

      if ( core_contracts ) {
         const auto& accnt = control->db().get<account_object, by_name>( "ra.authex"_n );
         abi_def abi;
         BOOST_REQUIRE_EQUAL( abi_serializer::to_abi( accnt.abi, abi ), true );
         authex_abi.set_abi( std::move( abi ), abi_serializer::create_yield_function( abi_serializer_max_time ) );

         const auto& claim_acc = control->db().get<account_object, by_name>( "ra.claim"_n );
         abi_def claim_abi_def;
         BOOST_REQUIRE_EQUAL( abi_serializer::to_abi( claim_acc.abi, claim_abi_def ), true );
         claim_abi.set_abi( std::move( claim_abi_def ), abi_serializer::create_yield_function( abi_serializer_max_time ) );
      }

      const auto& tok = control->db().get<account_object, by_name>( "ra.token"_n );
      abi_def tok_abi;
      BOOST_REQUIRE_EQUAL( abi_serializer::to_abi( tok.abi, tok_abi ), true );
      token_abi.set_abi( std::move( tok_abi ), abi_serializer::create_yield_function( abi_serializer_max_time ) );
   }

   action_result push( const name& code, const name& act, const name& signer, abi_serializer& ser, const variant_object& data ) {
      action a;
      a.account = code;
      a.name    = act;
      a.data    = ser.variant_to_binary( ser.get_action_type( act ), data,
                                         abi_serializer::create_yield_function( abi_serializer_max_time ) );
      return base_tester::push_action( std::move( a ), signer.to_uint64_t() );
   }

   bool           core_contracts = false;
   abi_serializer authex_abi;
   abi_serializer claim_abi;
   abi_serializer token_abi;
};

BOOST_AUTO_TEST_SUITE( ra_claim_tests )

BOOST_AUTO_TEST_CASE( v1_credit_and_claim ) try {
   authex_claim_tester chain;
   if ( !chain.core_contracts ) {
      BOOST_TEST_MESSAGE( "skip: ra.authex/ra.claim wasm not built (configure -DBUILD_CONTRACTS=ON)" );
      return;
   }

   const auto supply = asset::from_string( "1000000.0000 RA" );
   BOOST_REQUIRE_EQUAL( chain.success(),
                        chain.push( "ra.token"_n, "create"_n, "ra.token"_n, chain.token_abi,
                                    mvo()( "issuer", "ra.token" )( "maximum_supply", supply ) ) );
   BOOST_REQUIRE_EQUAL( chain.success(),
                        chain.push( "ra.token"_n, "issue"_n, "ra.token"_n, chain.token_abi,
                                    mvo()( "to", "ra.token" )( "quantity", supply )( "memo", "issue" ) ) );
   BOOST_REQUIRE_EQUAL( chain.success(),
                        chain.push( "ra.token"_n, "transfer"_n, "ra.token"_n, chain.token_abi,
                                    mvo()( "from", "ra.token" )( "to", "ra.claim" )( "quantity", asset::from_string( "100.0000 RA" ) )(
                                       "memo", "treasury" ) ) );

   BOOST_REQUIRE_EQUAL( chain.success(),
                        chain.push( "ra.claim"_n, "addrelayer"_n, "ra.claim"_n, chain.claim_abi,
                                    mvo()( "account", "relayer" ) ) );

   const auto txid = checksum256_type::hash( std::string( "ext-txid-v1" ) );
   BOOST_REQUIRE_EQUAL( chain.success(),
                        chain.push( "ra.claim"_n, "credit"_n, "relayer"_n, chain.claim_abi,
                                    mvo()( "chain", "eth" )( "ext_txid", txid )( "recipient", "alice" )(
                                       "quantity", asset::from_string( "5.0000 RA" ) ) ) );

   BOOST_REQUIRE_EQUAL( chain.success(),
                        chain.push( "ra.claim"_n, "claim"_n, "alice"_n, chain.claim_abi, mvo()( "id", 1 ) ) );

   BOOST_REQUIRE_EQUAL( asset::from_string( "5.0000 RA" ),
                        chain.get_currency_balance( "ra.token"_n, symbol( SY( 4, RA ) ), "alice"_n ) );
} FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_CASE( linksig_and_claimsig ) try {
   authex_claim_tester chain;
   if ( !chain.core_contracts ) {
      BOOST_TEST_MESSAGE( "skip: ra.authex/ra.claim wasm not built (configure -DBUILD_CONTRACTS=ON)" );
      return;
   }

   const auto supply = asset::from_string( "1000000.0000 RA" );
   BOOST_REQUIRE_EQUAL( chain.success(),
                        chain.push( "ra.token"_n, "create"_n, "ra.token"_n, chain.token_abi,
                                    mvo()( "issuer", "ra.token" )( "maximum_supply", supply ) ) );
   BOOST_REQUIRE_EQUAL( chain.success(),
                        chain.push( "ra.token"_n, "issue"_n, "ra.token"_n, chain.token_abi,
                                    mvo()( "to", "ra.token" )( "quantity", supply )( "memo", "issue" ) ) );
   BOOST_REQUIRE_EQUAL( chain.success(),
                        chain.push( "ra.token"_n, "transfer"_n, "ra.token"_n, chain.token_abi,
                                    mvo()( "from", "ra.token" )( "to", "ra.claim" )( "quantity", asset::from_string( "100.0000 RA" ) )(
                                       "memo", "treasury" ) ) );
   BOOST_REQUIRE_EQUAL( chain.success(),
                        chain.push( "ra.claim"_n, "addrelayer"_n, "ra.claim"_n, chain.claim_abi,
                                    mvo()( "account", "relayer" ) ) );

   auto em_priv = private_key_type::generate( private_key_type::key_type::em );
   const auto pubkey = em_priv.get_public_key().to_string( {} );
   const auto msg = link_message( pubkey, "alice"_n, "eth"_n, 0 );
   const auto sig = sign_msg( em_priv, msg );

   BOOST_REQUIRE_EQUAL( chain.wasm_assert_msg( "wrong nonce" ),
                        chain.push( "ra.authex"_n, "linksig"_n, "alice"_n, chain.authex_abi,
                                    mvo()( "account", "alice" )( "chain", "eth" )( "address", "0xabc" )( "pubkey", pubkey )(
                                       "nonce", 1 )( "sig", fc::to_hex( sig ) ) ) );

   BOOST_REQUIRE_EQUAL( chain.success(),
                        chain.push( "ra.authex"_n, "linksig"_n, "alice"_n, chain.authex_abi,
                                    mvo()( "account", "alice" )( "chain", "eth" )( "address", "0xabc" )( "pubkey", pubkey )(
                                       "nonce", 0 )( "sig", fc::to_hex( sig ) ) ) );

   const auto qty = asset::from_string( "7.0000 RA" );
   const auto txid = checksum256_type::hash( std::string( "ext-txid-sig" ) );
   BOOST_REQUIRE_EQUAL( chain.success(),
                        chain.push( "ra.claim"_n, "credit"_n, "relayer"_n, chain.claim_abi,
                                    mvo()( "chain", "eth" )( "ext_txid", txid )( "recipient", "alice" )( "quantity", qty ) ) );

   const auto cmsg = claim_message( txid, "alice"_n, qty, "eth"_n, 1 );
   const auto csig = sign_msg( em_priv, cmsg );

   BOOST_REQUIRE_EQUAL( chain.success(),
                        chain.push( "ra.claim"_n, "claimsig"_n, "bob"_n, chain.claim_abi,
                                    mvo()( "id", 1 )( "sig", fc::to_hex( csig ) ) ) );

   BOOST_REQUIRE_EQUAL( qty, chain.get_currency_balance( "ra.token"_n, symbol( SY( 4, RA ) ), "alice"_n ) );

   BOOST_REQUIRE_EQUAL( chain.wasm_assert_msg( "deposit already claimed" ),
                        chain.push( "ra.claim"_n, "claimsig"_n, "bob"_n, chain.claim_abi,
                                    mvo()( "id", 1 )( "sig", fc::to_hex( csig ) ) ) );
} FC_LOG_AND_RETHROW()

static std::string claimopen_message( const std::string& pubkey, const checksum256_type& ext_txid, const name& recipient,
                                      const asset& quantity, const name& chain, uint64_t id ) {
   return pubkey + "|" + checksum_hex( ext_txid ) + "|" + recipient.to_string() + "|" + quantity.to_string() + "|"
          + chain.to_string() + "|" + std::to_string( id ) + "|claimopen auth";
}

static void fund_treasury( authex_claim_tester& chain ) {
   const auto supply = asset::from_string( "1000000.0000 RA" );
   BOOST_REQUIRE_EQUAL( chain.success(),
                        chain.push( "ra.token"_n, "create"_n, "ra.token"_n, chain.token_abi,
                                    mvo()( "issuer", "ra.token" )( "maximum_supply", supply ) ) );
   BOOST_REQUIRE_EQUAL( chain.success(),
                        chain.push( "ra.token"_n, "issue"_n, "ra.token"_n, chain.token_abi,
                                    mvo()( "to", "ra.token" )( "quantity", supply )( "memo", "issue" ) ) );
   BOOST_REQUIRE_EQUAL( chain.success(),
                        chain.push( "ra.token"_n, "transfer"_n, "ra.token"_n, chain.token_abi,
                                    mvo()( "from", "ra.token" )( "to", "ra.claim" )( "quantity", asset::from_string( "100.0000 RA" ) )(
                                       "memo", "treasury" ) ) );
   BOOST_REQUIRE_EQUAL( chain.success(),
                        chain.push( "ra.claim"_n, "addrelayer"_n, "ra.claim"_n, chain.claim_abi,
                                    mvo()( "account", "relayer" ) ) );
}

static void prove_key_auths_account( authex_claim_tester& chain, const name& account, const private_key_type& priv ) {
   action a;
   a.account       = "ra.authex"_n;
   a.name          = "createlink"_n;
   a.authorization = { { account, "active"_n } };
   a.data          = chain.authex_abi.variant_to_binary( chain.authex_abi.get_action_type( "createlink"_n ),
                                                         mvo()( "account", account )( "chain", "eth" )( "address", "0xafter" ),
                                                         abi_serializer::create_yield_function( abi_serializer_max_time ) );
   signed_transaction trx;
   trx.actions.push_back( a );
   chain.set_transaction_headers( trx );
   trx.sign( priv, chain.control->get_chain_id() );
   auto trace = chain.push_transaction( trx );
   BOOST_REQUIRE( trace );
   BOOST_REQUIRE_EQUAL( trace->except, nullptr );
}

// Host EM signatures are the MetaMask / EIP-191 path (personal_sign of the 32-byte digest).
// Host ED signatures are the Phantom path (sign the digest; the pubkey rides in the signature).
static void expect_lazy_claim( private_key_type::key_type kind, const name& newbie ) {
   authex_claim_tester chain;
   if ( !chain.core_contracts ) {
      BOOST_TEST_MESSAGE( "skip: ra.authex/ra.claim wasm not built (configure -DBUILD_CONTRACTS=ON)" );
      return;
   }
   fund_treasury( chain );

   auto priv = private_key_type::generate( kind );
   const auto pubkey = priv.get_public_key().to_string( {} );
   const auto packed = fc::raw::pack( priv.get_public_key() );
   const auto qty    = asset::from_string( "9.0000 RA" );
   const auto txid   = checksum256_type::hash( std::string( "lazy-" ) + newbie.to_string() );

   BOOST_REQUIRE( chain.control->db().find<account_object, by_name>( newbie ) == nullptr );

   BOOST_REQUIRE_EQUAL( chain.wasm_assert_msg( "recipient account does not exist" ),
                        chain.push( "ra.claim"_n, "credit"_n, "relayer"_n, chain.claim_abi,
                                    mvo()( "chain", "eth" )( "ext_txid", txid )( "recipient", newbie )( "quantity", qty ) ) );

   BOOST_REQUIRE_EQUAL( chain.success(),
                        chain.push( "ra.claim"_n, "creditopen"_n, "relayer"_n, chain.claim_abi,
                                    mvo()( "chain", "eth" )( "ext_txid", txid )( "recipient", newbie )( "quantity", qty )(
                                       "pubkey", pubkey )( "packedkey", fc::to_hex( packed ) ) ) );

   auto other = private_key_type::generate( kind );
   const auto bad = sign_msg( other, claimopen_message( pubkey, txid, newbie, qty, "eth"_n, 1 ) );
   BOOST_REQUIRE_EQUAL( chain.wasm_assert_msg( "signature does not match the credited key" ),
                        chain.push( "ra.claim"_n, "claimopen"_n, "bob"_n, chain.claim_abi,
                                    mvo()( "id", 1 )( "sig", fc::to_hex( bad ) ) ) );

   const auto sig = sign_msg( priv, claimopen_message( pubkey, txid, newbie, qty, "eth"_n, 1 ) );
   BOOST_REQUIRE_EQUAL( chain.success(),
                        chain.push( "ra.claim"_n, "claimopen"_n, "bob"_n, chain.claim_abi,
                                    mvo()( "id", 1 )( "sig", fc::to_hex( sig ) ) ) );

   BOOST_REQUIRE( chain.control->db().find<account_object, by_name>( newbie ) != nullptr );
   BOOST_REQUIRE_EQUAL( qty, chain.get_currency_balance( "ra.token"_n, symbol( SY( 4, RA ) ), newbie ) );
   prove_key_auths_account( chain, newbie, priv );

   BOOST_REQUIRE_EQUAL( chain.wasm_assert_msg( "deposit already claimed" ),
                        chain.push( "ra.claim"_n, "claimopen"_n, "bob"_n, chain.claim_abi,
                                    mvo()( "id", 1 )( "sig", fc::to_hex( sig ) ) ) );
}

BOOST_AUTO_TEST_CASE( linksig_nonce_replay_and_rebound_claim ) try {
   authex_claim_tester chain;
   if ( !chain.core_contracts ) {
      BOOST_TEST_MESSAGE( "skip: ra.authex/ra.claim wasm not built (configure -DBUILD_CONTRACTS=ON)" );
      return;
   }
   fund_treasury( chain );

   auto em_priv = private_key_type::generate( private_key_type::key_type::em );
   const auto pubkey = em_priv.get_public_key().to_string( {} );
   const auto sig = sign_msg( em_priv, link_message( pubkey, "alice"_n, "eth"_n, 0 ) );

   BOOST_REQUIRE_EQUAL( chain.success(),
                        chain.push( "ra.authex"_n, "linksig"_n, "alice"_n, chain.authex_abi,
                                    mvo()( "account", "alice" )( "chain", "eth" )( "address", "0xabc" )( "pubkey", pubkey )(
                                       "nonce", 0 )( "sig", fc::to_hex( sig ) ) ) );
   BOOST_REQUIRE_EQUAL( chain.wasm_assert_msg( "wrong nonce" ),
                        chain.push( "ra.authex"_n, "linksig"_n, "alice"_n, chain.authex_abi,
                                    mvo()( "account", "alice" )( "chain", "eth" )( "address", "0xother" )( "pubkey", pubkey )(
                                       "nonce", 0 )( "sig", fc::to_hex( sig ) ) ) );

   const auto qty = asset::from_string( "3.0000 RA" );
   const auto txid = checksum256_type::hash( std::string( "rebind" ) );
   BOOST_REQUIRE_EQUAL( chain.success(),
                        chain.push( "ra.claim"_n, "credit"_n, "relayer"_n, chain.claim_abi,
                                    mvo()( "chain", "eth" )( "ext_txid", txid )( "recipient", "alice" )( "quantity", qty ) ) );

   const auto rebound = sign_msg( em_priv, claim_message( txid, "bob"_n, qty, "btc"_n, 1 ) );
   BOOST_REQUIRE_EQUAL( chain.wasm_assert_msg( "signature does not match a linked key" ),
                        chain.push( "ra.claim"_n, "claimsig"_n, "bob"_n, chain.claim_abi,
                                    mvo()( "id", 1 )( "sig", fc::to_hex( rebound ) ) ) );
} FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_CASE( ed_linksig_and_claimsig ) try {
   authex_claim_tester chain;
   if ( !chain.core_contracts ) {
      BOOST_TEST_MESSAGE( "skip: ra.authex/ra.claim wasm not built (configure -DBUILD_CONTRACTS=ON)" );
      return;
   }
   fund_treasury( chain );

   auto ed_priv = private_key_type::generate( private_key_type::key_type::ed );
   const auto pubkey = ed_priv.get_public_key().to_string( {} );
   const auto sig = sign_msg( ed_priv, link_message( pubkey, "alice"_n, "sol"_n, 0 ) );
   BOOST_REQUIRE_EQUAL( chain.success(),
                        chain.push( "ra.authex"_n, "linksig"_n, "alice"_n, chain.authex_abi,
                                    mvo()( "account", "alice" )( "chain", "sol" )( "address", "phantom" )( "pubkey", pubkey )(
                                       "nonce", 0 )( "sig", fc::to_hex( sig ) ) ) );

   const auto qty = asset::from_string( "4.0000 RA" );
   const auto txid = checksum256_type::hash( std::string( "ed-claim" ) );
   BOOST_REQUIRE_EQUAL( chain.success(),
                        chain.push( "ra.claim"_n, "credit"_n, "relayer"_n, chain.claim_abi,
                                    mvo()( "chain", "sol" )( "ext_txid", txid )( "recipient", "alice" )( "quantity", qty ) ) );
   const auto csig = sign_msg( ed_priv, claim_message( txid, "alice"_n, qty, "sol"_n, 1 ) );
   BOOST_REQUIRE_EQUAL( chain.success(),
                        chain.push( "ra.claim"_n, "claimsig"_n, "bob"_n, chain.claim_abi,
                                    mvo()( "id", 1 )( "sig", fc::to_hex( csig ) ) ) );
   BOOST_REQUIRE_EQUAL( qty, chain.get_currency_balance( "ra.token"_n, symbol( SY( 4, RA ) ), "alice"_n ) );
} FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_CASE( lazy_claimopen_em ) try {
   expect_lazy_claim( private_key_type::key_type::em, "newbieuser11"_n );
} FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_CASE( lazy_claimopen_ed ) try {
   expect_lazy_claim( private_key_type::key_type::ed, "neweduser111"_n );
} FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
