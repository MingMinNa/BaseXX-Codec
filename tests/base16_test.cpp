#include "../include/BaseXX/base16.hpp"
#include <catch2/catch_test_macros.hpp>
#include <vector>
#include <string>
#include <cstdint>

BaseXX::Base16 codec16 = BaseXX::Base16();

static std::vector<uint8_t> to_bytes(const std::string &s) 
{
    return std::vector<uint8_t>(s.begin(), s.end());
}

/* encode & decode */

TEST_CASE( "Base16: RFC4648 examples", "[base16]" ) 
{
    const std::string cases[][2] = {
        {""      , ""},
        {"f"     , "66"},
        {"fo"    , "666F"},
        {"foo"   , "666F6F"},
        {"foob"  , "666F6F62"},
        {"fooba" , "666F6F6261"},
        {"foobar", "666F6F626172"},
    };

    for (std::size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        std::vector<uint8_t> bytes = to_bytes(cases[i][0]);
        REQUIRE( codec16.encode(bytes) == cases[i][1] );
        REQUIRE( codec16.decode(cases[i][1]) == bytes );
    }
}

TEST_CASE( "Base16: single byte", "[base16]" ) 
{
    const char *cases[] = {
        "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B", "0C", "0D", "0E", "0F",
        "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1A", "1B", "1C", "1D", "1E", "1F",
        "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2A", "2B", "2C", "2D", "2E", "2F",
        "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3A", "3B", "3C", "3D", "3E", "3F",
        "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "4A", "4B", "4C", "4D", "4E", "4F",
        "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "5A", "5B", "5C", "5D", "5E", "5F",
        "60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "6A", "6B", "6C", "6D", "6E", "6F",
        "70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "7A", "7B", "7C", "7D", "7E", "7F",
        "80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "8A", "8B", "8C", "8D", "8E", "8F",
        "90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "9A", "9B", "9C", "9D", "9E", "9F",
        "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "AA", "AB", "AC", "AD", "AE", "AF",
        "B0", "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9", "BA", "BB", "BC", "BD", "BE", "BF",
        "C0", "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9", "CA", "CB", "CC", "CD", "CE", "CF",
        "D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9", "DA", "DB", "DC", "DD", "DE", "DF",
        "E0", "E1", "E2", "E3", "E4", "E5", "E6", "E7", "E8", "E9", "EA", "EB", "EC", "ED", "EE", "EF",
        "F0", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "FA", "FB", "FC", "FD", "FE", "FF"
    };

    for (std::size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        std::vector<uint8_t> bytes = {static_cast<uint8_t>(i)};
        REQUIRE( codec16.encode(bytes) == cases[i] );
        REQUIRE( codec16.decode(cases[i]) == bytes );
    }
}

TEST_CASE( "Base16: multiple bytes", "[base16]" ) 
{
    // "Hello, World"
    std::vector<uint8_t> bytes = { 
        0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 
        0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64 
    };
    std::string encoded_str = "48656C6C6F2C20576F726C64";

    REQUIRE( codec16.encode(bytes) == encoded_str );
    REQUIRE( codec16.decode(encoded_str) == bytes );
}

TEST_CASE( "Base16: bytes covering full range 0x00-0x0F", "[base16]" ) 
{
    std::vector<uint8_t> bytes = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
    };
    std::string encoded_str = "000102030405060708090A0B0C0D0E0F";

    REQUIRE( codec16.encode(bytes) == encoded_str );
    REQUIRE( codec16.decode(encoded_str) == bytes );
}

TEST_CASE( "Base16: long text", "[base16]" ) 
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
        "4F75722046617468657220696E2068656176656E2C2068616C6C6F7765642062652"
        "0796F7572206E616D652C20796F7572206B696E67646F6D20636F6D652C20796F75"
        "722077696C6C20626520646F6E652C206F6E20656172746820617320696E2068656"
        "176656E2E204769766520757320746F646179206F7572206461696C792062726561"
        "642E20466F7267697665207573206F75722073696E7320617320776520666F72676"
        "976652074686F73652077686F2073696E20616761696E73742075732E204C656164"
        "207573206E6F7420696E746F2074656D70746174696F6E206275742064656C69766"
        "5722075732066726F6D206576696C2E20466F7220746865206B696E67646F6D2C20"
        "74686520706F7765722C20616E642074686520676C6F72792061726520796F75727"
        "3206E6F7720616E6420666F7220657665722E20416D656E2E20";

    REQUIRE( codec16.encode(bytes) == encoded_str );
    REQUIRE( codec16.decode(encoded_str) == bytes );
}

/* invalid decode */

TEST_CASE( "Base16: odd length string throws or fails", "[base16]" ) 
{
    REQUIRE_THROWS ( codec16.decode("A") );        // length 1
    REQUIRE_NOTHROW( codec16.decode("AB") );       // length 2 (OK)
    REQUIRE_THROWS ( codec16.decode("ABC") );      // length 3
    REQUIRE_NOTHROW( codec16.decode("ABCD") );     // length 4 (OK)
    REQUIRE_THROWS ( codec16.decode("12345") );    // length 5
    REQUIRE_NOTHROW( codec16.decode("123456") );   // length 6 (OK)
    REQUIRE_THROWS ( codec16.decode("1234567") );  // length 7
}

TEST_CASE( "Base16: invalid characters throw", "[base16]" ) 
{
    REQUIRE_THROWS( codec16.decode("ZZ") );
    REQUIRE_THROWS( codec16.decode("G0") );
    REQUIRE_THROWS( codec16.decode("1!") );
}

/* is_valid function */

TEST_CASE( "Base16: empty string is valid", "[base16][is_valid]" ) 
{
    REQUIRE( codec16.is_valid("") == true );
}

TEST_CASE( "Base16: valid hex string", "[base16][is_valid]" ) 
{
    REQUIRE( codec16.is_valid("0123456789ABCDEF") == true );
    REQUIRE( codec16.is_valid("0123456789abcdef") == true );  // lowercase is ok for decoder.
    REQUIRE( codec16.is_valid("0123456789aBcDeF") == true ); 
}

TEST_CASE( "Base16: odd length string is invalid", "[base16][is_valid]" ) 
{
    REQUIRE( codec16.is_valid("ABC") == false );
}

TEST_CASE( "Base16: string with non-hex characters is invalid", "[base16][is_valid]" ) 
{
    REQUIRE( codec16.is_valid("GG") == false );        // 'G' is not in alphabet.
    REQUIRE( codec16.is_valid("-12-34") == false );    // '-' is not in alphabet.
    REQUIRE( codec16.is_valid(" 12 34") == false );    // ' ' is not in alphabet.
}