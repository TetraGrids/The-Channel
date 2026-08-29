#include <ra.fees/ra.fees.hpp>

namespace eosio {

void fees::on_transfer( const name from, const name to, const asset quantity, const string memo )
{
   if ( to != get_self() ) {
      return;
   }
   if (rasystem::system_contract::rex_available()) {
      rasystem::system_contract::donatetorex_action donatetorex( "ra"_n, { get_self(), "active"_n });
      donatetorex.send(get_self(), quantity, memo);
   }
}

void fees::noop()
{
   require_auth( get_self() );
}

} /// namespace eosio
