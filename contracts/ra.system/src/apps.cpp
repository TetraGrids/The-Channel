#include <ra.system/ra.system.hpp>

namespace rasystem {

   void system_contract::newapp( const name& account, const name& admin, const std::string& description )
   {
      require_auth( account );
      check( is_account( account ), "app account does not exist" );
      check( is_account( admin ), "admin account does not exist" );
      check( description.size() <= 256, "description has more than 256 bytes" );
      check( account != "owner"_n && account != "active"_n, "app account cannot be a reserved permission name" );
      check( _apps.find( account.value ) == _apps.end(), "app already registered" );

      _apps.emplace( account, [&]( auto& row ) {
         row.account     = account;
         row.admin       = admin;
         row.description = description;
         row.enabled     = true;
      });
   }

   void system_contract::adminsetapp( const name& account, const name& admin, const std::string& description, bool enabled )
   {
      require_auth( get_self() );
      check( is_account( account ), "app account does not exist" );
      check( is_account( admin ), "admin account does not exist" );
      check( description.size() <= 256, "description has more than 256 bytes" );
      check( account != "owner"_n && account != "active"_n, "app account cannot be a reserved permission name" );

      auto itr = _apps.find( account.value );
      if ( itr == _apps.end() ) {
         _apps.emplace( get_self(), [&]( auto& row ) {
            row.account     = account;
            row.admin       = admin;
            row.description = description;
            row.enabled     = enabled;
         });
      } else {
         _apps.modify( itr, same_payer, [&]( auto& row ) {
            row.admin       = admin;
            row.description = description;
            row.enabled     = enabled;
         });
      }
   }

   void system_contract::deleteapp( const name& account )
   {
      auto itr = _apps.require_find( account.value, "app is not registered" );
      check( has_auth( account ) || has_auth( itr->admin ) || has_auth( get_self() ),
             "missing authority of app, admin, or ra" );
      _apps.erase( itr );
   }

   void system_contract::loginwithapp( const name& user, const name& app, const name& parent,
                                       const eosio::public_key& key )
   {
      require_auth( user );
      check( is_account( user ), "user account does not exist" );
      check( parent.value != 0, "parent permission required" );
      check( app != "owner"_n && app != "active"_n, "app permission cannot replace owner or active" );

      auto itr = _apps.require_find( app.value, "app is not registered" );
      check( itr->enabled, "app is disabled" );

      authority auth;
      auth.threshold = 1;
      auth.keys.push_back( key_weight{ key, 1 } );

      native::updateauth_action upd{ get_self(), { { user, parent } } };
      upd.send( user, app, parent, auth, eosio::binary_extension<name>{} );
   }

} // namespace rasystem
