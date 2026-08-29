#include <ra.system/native.hpp>

#include <eosio/check.hpp>

namespace rasystem {

   void native::onerror( ignore<uint128_t>, ignore<std::vector<char>> ) {
      eosio::check( false, "the onerror action cannot be called directly" );
   }

}
