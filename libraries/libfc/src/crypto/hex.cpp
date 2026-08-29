#include <fc/crypto/hex.hpp>
#include <fc/exception/exception.hpp>

namespace fc {

    uint8_t from_hex( char c ) {
      if( c >= '0' && c <= '9' )
        return c - '0';
      if( c >= 'a' && c <= 'f' )
          return c - 'a' + 10;
      if( c >= 'A' && c <= 'F' )
          return c - 'A' + 10;
      FC_THROW_EXCEPTION( exception, "Invalid hex character '${c}'", ("c", std::string(&c,1) ) );
      return 0;
    }

    std::string to_hex( const char* d, uint32_t s, bool add_prefix )
    {
        std::string r;
        r.reserve(s * 2 + (add_prefix ? 2 : 0));
        if (add_prefix) r += "0x";
        const char* to_hex="0123456789abcdef";
        const uint8_t* c = (const uint8_t*)d;
        for( uint32_t i = 0; i < s; ++i )
            (r += to_hex[(c[i]>>4)]) += to_hex[(c[i] &0x0f)];
        return r;
    }

    size_t from_hex( std::string_view hex_str, char* out_data, size_t out_data_len ) {
        auto i = hex_str.begin();
        uint8_t* out_pos = (uint8_t*)out_data;
        uint8_t* out_end = out_pos + out_data_len;
        while( i != hex_str.end() && out_end != out_pos ) {
          *out_pos = from_hex( *i ) << 4;
          ++i;
          if( i != hex_str.end() )  {
              *out_pos |= from_hex( *i );
              ++i;
          }
          ++out_pos;
        }
        return out_pos - (uint8_t*)out_data;
    }

    std::vector<uint8_t> from_hex( const std::string& hex, bool trim_prefix ) {
        auto cleaned_hex = trim_prefix ? trim_hex_prefix(hex) : hex;
        if (cleaned_hex.size() % 2) {
           cleaned_hex = "0" + cleaned_hex;
        }
        std::vector<uint8_t> out;
        out.reserve(cleaned_hex.size() / 2);

        for (size_t i = 0; i < cleaned_hex.size(); i += 2) {
           uint8_t byte = (from_hex(cleaned_hex[i]) << 4) | from_hex(cleaned_hex[i + 1]);
           out.push_back(byte);
        }

        return out;
    }

    std::string trim_hex_prefix(const std::string& hex) {
        if (hex.starts_with("0x") || hex.starts_with("0X")) {
           return hex.substr(2);
        }
        return hex;
    }

    std::string to_hex( const std::vector<char>& data )
    {
       if( data.size() )
          return to_hex( data.data(), data.size() );
       return "";
    }

}
