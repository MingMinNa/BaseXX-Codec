#include "../include/BaseXX/base64.hpp"
#include <catch2/catch_test_macros.hpp>
#include <vector>
#include <string>
#include <cstdint>

BaseXX::Base64 default_codec64 = BaseXX::Base64(BaseXX::Base64Type::DEFAULT);
BaseXX::Base64 url_codec64     = BaseXX::Base64(BaseXX::Base64Type::URL);
BaseXX::Base64 pem_codec64     = BaseXX::Base64(BaseXX::Base64Type::PEM);

static std::vector<uint8_t> to_bytes(const std::string &s) 
{
    return std::vector<uint8_t>(s.begin(), s.end());
}

// Default Codec

/* encode & decode */

TEST_CASE( "Base64-Default: RFC4648 examples", "[base64]" ) 
{
    const std::string cases[][2] = {
        {""      , ""},
        {"f"     , "Zg=="},
        {"fo"    , "Zm8="},
        {"foo"   , "Zm9v"},
        {"foob"  , "Zm9vYg=="},
        {"fooba" , "Zm9vYmE="},
        {"foobar", "Zm9vYmFy"},
    };

    for (std::size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        std::vector<uint8_t> bytes = to_bytes(cases[i][0]);
        REQUIRE( default_codec64.encode(bytes) == cases[i][1] );
        REQUIRE( default_codec64.decode(cases[i][1]) == bytes );
    }
}

TEST_CASE( "Base64-Default: single byte", "[base64]" ) 
{
    const uint8_t byte_cases[] = {
        0x00, 0x03, 0x05, 0x07, 0x09, 0x0A, 0x0C, 0x0F, 
        0xF0, 0xF3, 0xF5, 0xF7, 0xF9, 0xFA, 0xFC, 0xFF
    };
    const char *cases[] = {
        "AA==", "Aw==", "BQ==", "Bw==", "CQ==", "Cg==", "DA==", "Dw==", 
        "8A==", "8w==", "9Q==", "9w==", "+Q==", "+g==", "/A==", "/w=="

    };

    for (std::size_t i = 0; i < sizeof(byte_cases) / sizeof(byte_cases[0]); ++i) {
        std::vector<uint8_t> bytes = {byte_cases[i]};
        REQUIRE( default_codec64.encode(bytes) == cases[i] );
        REQUIRE( default_codec64.decode(cases[i]) == bytes );
    }
}

TEST_CASE( "Base64-Default: multiple bytes", "[base64]" ) 
{
    // "Hello, World"
    std::vector<uint8_t> bytes = { 
        0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 
        0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64 
    };
    std::string encoded_str = "SGVsbG8sIFdvcmxk";

    REQUIRE( default_codec64.encode(bytes) == encoded_str );
    REQUIRE( default_codec64.decode(encoded_str) == bytes );
}

TEST_CASE( "Base64-Default: bytes covering full range 0x00-0x0F", "[base64]" ) 
{
    std::vector<uint8_t> bytes = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
    };
    std::string encoded_str = "AAECAwQFBgcICQoLDA0ODw==";

    REQUIRE( default_codec64.encode(bytes) == encoded_str );
    REQUIRE( default_codec64.decode(encoded_str) == bytes );
}

TEST_CASE( "Base64-Default: long text", "[base64]" ) 
{
    std::string the_lord_prayer = 
        "Our Father in heaven, "
        "hallowed be your name, "
        "your kingdom come, "
        "your will be done, "
        "on earth as in heaven. "
        "Give us today our daily bread. "
        "Forgive us our sins "
        "as we forgive those who sin against us. "
        "Lead us not into temptation "
        "but deliver us from evil. "
        "For the kingdom, the power, "
        "and the glory are yours "
        "now and for ever. "
        "Amen. ";

    std::vector<uint8_t> bytes = to_bytes(the_lord_prayer);
    std::string encoded_str = 
        "T3VyIEZhdGhlciBpbiBoZWF2ZW4sIGhhbGxvd2VkIGJlIHlvdXIgbmFtZSwgeW91ciB"
        "raW5nZG9tIGNvbWUsIHlvdXIgd2lsbCBiZSBkb25lLCBvbiBlYXJ0aCBhcyBpbiBoZW"
        "F2ZW4uIEdpdmUgdXMgdG9kYXkgb3VyIGRhaWx5IGJyZWFkLiBGb3JnaXZlIHVzIG91c"
        "iBzaW5zIGFzIHdlIGZvcmdpdmUgdGhvc2Ugd2hvIHNpbiBhZ2FpbnN0IHVzLiBMZWFk"
        "IHVzIG5vdCBpbnRvIHRlbXB0YXRpb24gYnV0IGRlbGl2ZXIgdXMgZnJvbSBldmlsLiB"
        "Gb3IgdGhlIGtpbmdkb20sIHRoZSBwb3dlciwgYW5kIHRoZSBnbG9yeSBhcmUgeW91cn"
        "Mgbm93IGFuZCBmb3IgZXZlci4gQW1lbi4g";

    REQUIRE( default_codec64.encode(bytes) == encoded_str );
    REQUIRE( default_codec64.decode(encoded_str) == bytes );
}

/* invalid decode */

TEST_CASE( "Base64-Default: invalid length string throws or fails", "[base64]" ) 
{
    REQUIRE_THROWS  ( default_codec64.decode("A") );                  // length 1
    REQUIRE_THROWS  ( default_codec64.decode("AB") );                 // length 2
    REQUIRE_THROWS  ( default_codec64.decode("ABC") );                // length 3
    REQUIRE_NOTHROW ( default_codec64.decode("ABCD") );               // length 4  (OK, 3 bytes)
    REQUIRE_THROWS  ( default_codec64.decode("ABCDE") );              // length 5
    REQUIRE_THROWS  ( default_codec64.decode("ABCDEF") );             // length 6
    REQUIRE_THROWS  ( default_codec64.decode("ABCDEFG") );            // length 7
    REQUIRE_NOTHROW ( default_codec64.decode("ABCDEFGH") );           // length 8  (OK, 6 bytes)
    REQUIRE_THROWS  ( default_codec64.decode("ABCDEFGHI") );          // length 9
    REQUIRE_NOTHROW ( default_codec64.decode("ABCDEFGHIJKLMNOP") );   // length 16 (OK, 12 bytes)
}

TEST_CASE( "Base64-Default: padded strings of valid length decode correctly", "[base64]" ) 
{
    REQUIRE_NOTHROW( default_codec64.decode("AA==") );      // 1 byte
    REQUIRE_NOTHROW( default_codec64.decode("ABA=") );      // 2 bytes
    REQUIRE_NOTHROW( default_codec64.decode("ABCD") );      // 3 bytes
    REQUIRE_NOTHROW( default_codec64.decode("ABCDABA=") );  // 5 bytes
    REQUIRE_NOTHROW( default_codec64.decode("ABCDABCD") );  // 6 bytes
}

TEST_CASE( "Base64-Default: incorrect padding throws", "[base64]" ) 
{
    REQUIRE_THROWS( default_codec64.decode("A===") );
    REQUIRE_THROWS( default_codec64.decode("====") );
    REQUIRE_THROWS( default_codec64.decode("AB=D") );
    REQUIRE_THROWS( default_codec64.decode("A=CD") );
    REQUIRE_THROWS( default_codec64.decode("ABCD=") );
}

TEST_CASE( "Base64-Default: invalid characters throw", "[base64]" ) 
{
    REQUIRE_THROWS( default_codec64.decode("AB!D") );
    REQUIRE_THROWS( default_codec64.decode("A@CD") );
    REQUIRE_THROWS( default_codec64.decode(" BCD") );
    REQUIRE_THROWS( default_codec64.decode("ABD-") );
    REQUIRE_THROWS( default_codec64.decode("A_CD") );  
}

// PEM Codec

/* encode & decode */

TEST_CASE( "Base64-PEM: long text", "[base64]" ) 
{
    std::string the_lord_prayer = 
        "Our Father in heaven, "
        "hallowed be your name, "
        "your kingdom come, "
        "your will be done, "
        "on earth as in heaven. "
        "Give us today our daily bread. "
        "Forgive us our sins "
        "as we forgive those who sin against us. "
        "Lead us not into temptation "
        "but deliver us from evil. "
        "For the kingdom, the power, "
        "and the glory are yours "
        "now and for ever. "
        "Amen. ";

    std::vector<uint8_t> bytes = to_bytes(the_lord_prayer);
    std::string encoded_str = 
        "T3VyIEZhdGhlciBpbiBoZWF2ZW4sIGhhbGxvd2VkIGJlIHlvdXIgbmFtZSwgeW91\n"
        "ciBraW5nZG9tIGNvbWUsIHlvdXIgd2lsbCBiZSBkb25lLCBvbiBlYXJ0aCBhcyBp\n"
        "biBoZWF2ZW4uIEdpdmUgdXMgdG9kYXkgb3VyIGRhaWx5IGJyZWFkLiBGb3JnaXZl\n"
        "IHVzIG91ciBzaW5zIGFzIHdlIGZvcmdpdmUgdGhvc2Ugd2hvIHNpbiBhZ2FpbnN0\n"
        "IHVzLiBMZWFkIHVzIG5vdCBpbnRvIHRlbXB0YXRpb24gYnV0IGRlbGl2ZXIgdXMg\n"
        "ZnJvbSBldmlsLiBGb3IgdGhlIGtpbmdkb20sIHRoZSBwb3dlciwgYW5kIHRoZSBn\n"
        "bG9yeSBhcmUgeW91cnMgbm93IGFuZCBmb3IgZXZlci4gQW1lbi4g";;

    REQUIRE( pem_codec64.encode(bytes) == encoded_str );
    REQUIRE( pem_codec64.decode(encoded_str) == bytes );
}

/* is_valid function */

TEST_CASE( "Base64: distinguish Base64-Default and Base64-URL", "[base64][is_valid]" )
{
    REQUIRE( default_codec64.is_valid("ABCDA+/B+13/Cw==") == true );
    REQUIRE( default_codec64.is_valid("ABCDA-_B-13_Cw==") == false );
    REQUIRE( url_codec64.is_valid("ABCDA+/B+13/Cw==") == false );
    REQUIRE( url_codec64.is_valid("ABCDA-_B-13_Cw==") == true );
}

TEST_CASE( "Base64: empty string is valid", "[base64][is_valid]" ) 
{
    REQUIRE( default_codec64.is_valid("") == true );
    REQUIRE( url_codec64.is_valid("") == true );
}

TEST_CASE( "Base64: valid base64 string", "[base64][is_valid]" ) 
{
    REQUIRE( default_codec64.is_valid("MFRG") == true );
    REQUIRE( default_codec64.is_valid("abcdefgh") == true );
    REQUIRE( default_codec64.is_valid("ABCDEFGHIJKLMNOPQRSTUVWX") == true );
}

TEST_CASE( "Base64: invalid length string is invalid", "[base64][is_valid]" ) 
{
    REQUIRE( default_codec64.is_valid("A") == false );       // length 1
    REQUIRE( default_codec64.is_valid("AB") == false );      // length 2
    REQUIRE( default_codec64.is_valid("ABC") == false );     // length 3
    REQUIRE( default_codec64.is_valid("ABCD") == true  );    // length 4 (OK)
    REQUIRE( default_codec64.is_valid("ABCDE") == false );   // length 5
    REQUIRE( default_codec64.is_valid("ABCDEF") == false );  // length 6
    REQUIRE( default_codec64.is_valid("ABCDEFG") == false ); // length 7
}

TEST_CASE( "Base64: string with non-base64 characters is invalid", "[base64][is_valid]" ) 
{
    REQUIRE( default_codec64.is_valid("ABC!") == false );
    REQUIRE( default_codec64.is_valid("ABC@") == false );
    REQUIRE( default_codec64.is_valid("ABCD-EFG") == false );
    REQUIRE( default_codec64.is_valid("D234 567") == false ); 
}

/* padding bit-validity checks */

TEST_CASE( "Base64: more than 2 padding characters is always invalid", "[base64][is_valid]" ) 
{
    REQUIRE( default_codec64.is_valid("A===") == false );     // num_padding = 3
    REQUIRE( default_codec64.is_valid("====") == false );     // num_padding = 4
    REQUIRE( default_codec64.is_valid("AAAA====") == false ); // num_padding = 4
}

TEST_CASE( "Base64: padding=1 requires last char's lowest 2 bits to be zero", "[base64][is_valid]" ) 
{
    REQUIRE( default_codec64.is_valid("AAA=") == true );   // last char 'A' = 0  (0b000000) -> valid
    REQUIRE( default_codec64.is_valid("AAE=") == true );   // last char 'E' = 4  (0b000100) -> valid
    REQUIRE( default_codec64.is_valid("AAQ=") == true );   // last char 'Q' = 16 (0b010000) -> valid
    REQUIRE( default_codec64.is_valid("AAY=") == true );   // last char 'Y' = 24 (0b011000) -> valid

    REQUIRE( default_codec64.is_valid("AAB=") == false );  // last char 'B' = 1  (0b000001) -> invalid
    REQUIRE( default_codec64.is_valid("AAC=") == false );  // last char 'C' = 2  (0b000010) -> invalid
    REQUIRE( default_codec64.is_valid("AAF=") == false );  // last char 'F' = 5  (0b000101) -> invalid
}

TEST_CASE( "Base64: padding=2 requires last char's lowest 4 bits to be zero", "[base64][is_valid]" ) 
{
    REQUIRE( default_codec64.is_valid("AA==") == true );   // last char 'A' = 0  (0b000000) -> valid
    REQUIRE( default_codec64.is_valid("AQ==") == true );   // last char 'Q' = 16 (0b010000) -> valid
    REQUIRE( default_codec64.is_valid("Ag==") == true );   // last char 'g' = 32 (0b100000) -> valid
    REQUIRE( default_codec64.is_valid("Aw==") == true );   // last char 'w' = 48 (0b110000) -> valid

    REQUIRE( default_codec64.is_valid("AB==") == false );  // last char 'B' = 1  -> invalid
    REQUIRE( default_codec64.is_valid("AE==") == false );  // last char 'E' = 4  -> invalid
    REQUIRE( default_codec64.is_valid("AP==") == false );  // last char 'P' = 15 -> invalid
}