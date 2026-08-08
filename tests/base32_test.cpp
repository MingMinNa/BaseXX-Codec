#include "../include/BaseXX/base32.hpp"
#include <catch2/catch_test_macros.hpp>
#include <vector>
#include <string>
#include <cstdint>

BaseXX::Base32 default_codec32 = BaseXX::Base32(BaseXX::Base32Type::DEFAULT);
BaseXX::Base32 hex_codec32     = BaseXX::Base32(BaseXX::Base32Type::HEX);

static std::vector<uint8_t> to_bytes(const std::string &s) 
{
    return std::vector<uint8_t>(s.begin(), s.end());
}

// Default Codec

/* encode & decode */

TEST_CASE( "Base32-Default: RFC4648 examples", "[base32]" ) 
{
    const std::string cases[][2] = {
        {""      , ""},
        {"f"     , "MY======"},
        {"fo"    , "MZXQ===="},
        {"foo"   , "MZXW6==="},
        {"foob"  , "MZXW6YQ="},
        {"fooba" , "MZXW6YTB"},
        {"foobar", "MZXW6YTBOI======"},
    };

    for (std::size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        std::vector<uint8_t> bytes = to_bytes(cases[i][0]);
        REQUIRE( default_codec32.encode(bytes) == cases[i][1] );
        REQUIRE( default_codec32.decode(cases[i][1]) == bytes );
    }
}

TEST_CASE( "Base32-Default: single byte", "[base32]" ) 
{
    const uint8_t byte_cases[] = {
        0x00, 0x03, 0x05, 0x07, 0x09, 0x0A, 0x0C, 0x0F, 
        0xF0, 0xF3, 0xF5, 0xF7, 0xF9, 0xFA, 0xFC, 0xFF
    };
    const char *cases[] = {
        "AA======", "AM======", "AU======", "A4======", "BE======", "BI======", "BQ======", "B4======", 
        "6A======", "6M======", "6U======", "64======", "7E======", "7I======", "7Q======", "74======"
    };

    for (std::size_t i = 0; i < sizeof(byte_cases) / sizeof(byte_cases[0]); ++i) {
        std::vector<uint8_t> bytes = {byte_cases[i]};
        REQUIRE( default_codec32.encode(bytes) == cases[i] );
        REQUIRE( default_codec32.decode(cases[i]) == bytes );
    }
}

TEST_CASE( "Base32-Default: multiple bytes", "[base32]" ) 
{
    // "Hello, World"
    std::vector<uint8_t> bytes = { 
        0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 
        0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64 
    };
    std::string encoded_str = "JBSWY3DPFQQFO33SNRSA====";

    REQUIRE( default_codec32.encode(bytes) == encoded_str );
    REQUIRE( default_codec32.decode(encoded_str) == bytes );
}

TEST_CASE( "Base32-Default: bytes covering full range 0x00-0x0F", "[base32]" ) 
{
    std::vector<uint8_t> bytes = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
    };
    std::string encoded_str = "AAAQEAYEAUDAOCAJBIFQYDIOB4======";

    REQUIRE( default_codec32.encode(bytes) == encoded_str );
    REQUIRE( default_codec32.decode(encoded_str) == bytes );
}

TEST_CASE( "Base32-Default: long text", "[base32]" ) 
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
        "J52XEICGMF2GQZLSEBUW4IDIMVQXMZLOFQQGQYLMNRXXOZLEEBRGKIDZN52XEIDOMFW"
        "WKLBAPFXXK4RANNUW4Z3EN5WSAY3PNVSSYIDZN52XEIDXNFWGYIDCMUQGI33OMUWCA3"
        "3OEBSWC4TUNAQGC4ZANFXCA2DFMF3GK3ROEBDWS5TFEB2XGIDUN5SGC6JAN52XEIDEM"
        "FUWY6JAMJZGKYLEFYQEM33SM5UXMZJAOVZSA33VOIQHG2LOOMQGC4ZAO5SSAZTPOJTW"
        "S5TFEB2GQ33TMUQHO2DPEBZWS3RAMFTWC2LOON2CA5LTFYQEYZLBMQQHK4ZANZXXIID"
        "JNZ2G6IDUMVWXA5DBORUW63RAMJ2XIIDEMVWGS5TFOIQHK4ZAMZZG63JAMV3GS3BOEB"
        "DG64RAORUGKIDLNFXGOZDPNUWCA5DIMUQHA33XMVZCYIDBNZSCA5DIMUQGO3DPOJ4SA"
        "YLSMUQHS33VOJZSA3TPO4QGC3TEEBTG64RAMV3GK4ROEBAW2ZLOFYQA====";

    REQUIRE( default_codec32.encode(bytes) == encoded_str );
    REQUIRE( default_codec32.decode(encoded_str) == bytes );
}

/* invalid decode */

TEST_CASE( "Base32-Default: invalid length string throws or fails", "[base32]" ) 
{
    REQUIRE_THROWS  ( default_codec32.decode("A") );                // length 1
    REQUIRE_THROWS  ( default_codec32.decode("AB") );               // length 2
    REQUIRE_THROWS  ( default_codec32.decode("ABC") );              // length 3
    REQUIRE_THROWS  ( default_codec32.decode("ABCD") );             // length 4
    REQUIRE_THROWS  ( default_codec32.decode("ABCDE") );            // length 5
    REQUIRE_THROWS  ( default_codec32.decode("ABCDEF") );           // length 6
    REQUIRE_THROWS  ( default_codec32.decode("ABCDEFG") );          // length 7
    REQUIRE_NOTHROW ( default_codec32.decode("ABCDEFGH") );         // length 8  (OK)
    REQUIRE_THROWS  ( default_codec32.decode("ABCDEFGHI") );        // length 9
    REQUIRE_NOTHROW ( default_codec32.decode("ABCDEFGHIJKLMNOP") ); // length 16 (OK)
}

TEST_CASE( "Base32-Default: padded strings of valid length decode correctly", "[base32]" ) 
{
    REQUIRE_NOTHROW( default_codec32.decode("AA======") );   // 1 byte 
    REQUIRE_NOTHROW( default_codec32.decode("ABCA====") );   // 2 bytes
    REQUIRE_NOTHROW( default_codec32.decode("ABCDE===") );   // 3 bytes 
    REQUIRE_NOTHROW( default_codec32.decode("ABCDEFA=") );   // 4 bytes 
    REQUIRE_NOTHROW( default_codec32.decode("ABCDEFGH") );   // 5 bytes 
}

TEST_CASE( "Base32-Default: incorrect padding throws", "[base32]" ) 
{
    REQUIRE_THROWS( default_codec32.decode("AB=====") );    
    REQUIRE_THROWS( default_codec32.decode("A=======") );
    REQUIRE_THROWS( default_codec32.decode("ABCD=E==") );
    REQUIRE_THROWS( default_codec32.decode("========") );
}

TEST_CASE( "Base32-Default: invalid characters throw", "[base32]" ) 
{
    REQUIRE_THROWS( default_codec32.decode("11111111") );   // '1' is not in alphabet.
    REQUIRE_THROWS( default_codec32.decode("ABCDEFG0") );   // '0' is not in alphabet.
    REQUIRE_THROWS( default_codec32.decode("1!234567") );   // '1', '!' are not in alphabet.
    REQUIRE_THROWS( default_codec32.decode("+/ABCD89") );   // '8', '9' are not in alphabet.
}

// Hex Codec

/* encode & decode */

TEST_CASE( "Base32-Hex: RFC4648 examples", "[base32]" ) 
{
    const std::string cases[][2] = {
        {""      , ""},
        {"f"     , "CO======"},
        {"fo"    , "CPNG===="},
        {"foo"   , "CPNMU==="},
        {"foob"  , "CPNMUOG="},
        {"fooba" , "CPNMUOJ1"},
        {"foobar", "CPNMUOJ1E8======"},
    };

    for (std::size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        std::vector<uint8_t> bytes = to_bytes(cases[i][0]);
        REQUIRE( hex_codec32.encode(bytes) == cases[i][1] );
        REQUIRE( hex_codec32.decode(cases[i][1]) == bytes );
    }
}

TEST_CASE( "Base32-Hex: single byte", "[base32]" ) 
{
    const uint8_t byte_cases[] = {
        0x00, 0x03, 0x05, 0x07, 0x09, 0x0A, 0x0C, 0x0F, 
        0xF0, 0xF3, 0xF5, 0xF7, 0xF9, 0xFA, 0xFC, 0xFF
    };
    const char *cases[] = {
        "00======", "0C======", "0K======", "0S======", "14======", "18======", "1G======", "1S======", 
        "U0======", "UC======", "UK======", "US======", "V4======", "V8======", "VG======", "VS======"
    };

    for (std::size_t i = 0; i < sizeof(byte_cases) / sizeof(byte_cases[0]); ++i) {
        std::vector<uint8_t> bytes = {byte_cases[i]};
        REQUIRE( hex_codec32.encode(bytes) == cases[i] );
        REQUIRE( hex_codec32.decode(cases[i]) == bytes );
    }
}

TEST_CASE( "Base32-Hex: multiple bytes", "[base32]" ) 
{
    // "Hello, World"
    std::vector<uint8_t> bytes = { 
        0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 
        0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64 
    };
    std::string encoded_str = "91IMOR3F5GG5ERRIDHI0====";

    REQUIRE( hex_codec32.encode(bytes) == encoded_str );
    REQUIRE( hex_codec32.decode(encoded_str) == bytes );
}

TEST_CASE( "Base32-Hex: bytes covering full range 0x00-0x0F", "[base32]" ) 
{
    std::vector<uint8_t> bytes = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
    };
    std::string encoded_str = "000G40O40K30E209185GO38E1S======";

    REQUIRE( hex_codec32.encode(bytes) == encoded_str );
    REQUIRE( hex_codec32.decode(encoded_str) == bytes );
}

TEST_CASE( "Base32-Hex: long text", "[base32]" ) 
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
        "9TQN4826C5Q6GPBI41KMS838CLGNCPBE5GG6GOBCDHNNEPB441H6A83PDTQN483EC5M"
        "MAB10F5NNASH0DDKMSPR4DTMI0ORFDLIIO83PDTQN483ND5M6O832CKG68RRECKM20R"
        "RE41IM2SJKD0G62SP0D5N20Q35C5R6ARHE413MITJ541QN683KDTI62U90DTQN4834C"
        "5KMOU90C9P6AOB45OG4CRRICTKNCP90ELPI0RRLE8G76QBEECG62SP0ETII0PJFE9JM"
        "ITJ541Q6GRRJCKG7EQ3F41PMIRH0C5JM2QBEEDQ20TBJ5OG4OPB1CGG7ASP0DPNN883"
        "9DPQ6U83KCLMN0T31EHKMURH0C9QN8834CLM6ITJ5E8G7ASP0CPP6UR90CLR6IR1E41"
        "36USH0EHK6A83BD5N6EP3FDKM20T38CKG70RRNCLP2O831DPI20T38CKG6ER3FE9SI0"
        "OBICKG7IRRLE9PI0RJFESG62RJ441J6USH0CLR6ASHE410MQPBE5OG0====";

    REQUIRE( hex_codec32.encode(bytes) == encoded_str );
    REQUIRE( hex_codec32.decode(encoded_str) == bytes );
}

/* is_valid function */

TEST_CASE( "Base32: distinguish Base32-Default and Base32-Hex", "[base32][is_valid]" )
{
    REQUIRE( default_codec32.is_valid("JBSWY3DPFQQFO33SNRSA====")         == true  );
    REQUIRE( default_codec32.is_valid("000G40O40K30E209185GO38E1S======") == false );
    REQUIRE( hex_codec32.is_valid("JBSWY3DPFQQFO33SNRSA====")         == false );
    REQUIRE( hex_codec32.is_valid("000G40O40K30E209185GO38E1S======") == true  );
}

TEST_CASE( "Base32: empty string is valid", "[base32][is_valid]" ) 
{
    REQUIRE( default_codec32.is_valid("") == true );
    REQUIRE( hex_codec32.is_valid("") == true );
}

TEST_CASE( "Base32: valid base32 string", "[base32][is_valid]" ) 
{
    REQUIRE( default_codec32.is_valid("MFRGGZDF") == true );
    REQUIRE( default_codec32.is_valid("abcdefgh") == true ); // lowercase is ok for decoder.
    REQUIRE( default_codec32.is_valid("ABCDEFGHIJKLMNOPQRSTUVWXYZ234567") == true );
}

TEST_CASE( "Base32: invalid length string is invalid", "[base32][is_valid]" ) 
{
    REQUIRE( default_codec32.is_valid("A") == false );      // length 1
    REQUIRE( default_codec32.is_valid("ABC") == false );    // length 3
    REQUIRE( default_codec32.is_valid("ABCDEF") == false ); // length 6
}

TEST_CASE( "Base32: string with non-base32 characters is invalid", "[base32][is_valid]" ) 
{
    REQUIRE( default_codec32.is_valid("11111111") == false );  // '1' is not in alphabet.
    REQUIRE( default_codec32.is_valid("ABCDEFG0") == false );  // '0' is not in alphabet.
    REQUIRE( default_codec32.is_valid("ABCD-EFG") == false );  // '-' is not in alphabet.
    REQUIRE( default_codec32.is_valid("D2345 67") == false );  // ' ' is not in alphabet.
}

/* padding bit-validity checks */

TEST_CASE( "Base32: padding=1 requires last char's lowest 3 bits to be zero", "[base32][is_valid]" ) 
{
    REQUIRE( default_codec32.is_valid("AAAAAAA=") == true );  // last char 'A' = 0  (0b00000) -> valid
    REQUIRE( default_codec32.is_valid("AAAAAAI=") == true );  // last char 'I' = 8  (0b01000) -> valid
    REQUIRE( default_codec32.is_valid("AAAAAAQ=") == true );  // last char 'Q' = 16 (0b10000) -> valid
    REQUIRE( default_codec32.is_valid("AAAAAAY=") == true );  // last char 'Y' = 24 (0b11000) -> valid

    REQUIRE( default_codec32.is_valid("AAAAAAB=") == false ); // last char 'B' = 1  (0b00001) -> invalid
    REQUIRE( default_codec32.is_valid("AAAAAAH=") == false ); // last char 'H' = 7  (0b00111) -> invalid
    REQUIRE( default_codec32.is_valid("AAAAAAZ=") == false ); // last char 'Z' = 25 (0b11001) -> invalid
}

TEST_CASE( "Base32: padding=3 requires last char's lowest 1 bit to be zero", "[base32][is_valid]" ) 
{
    REQUIRE( default_codec32.is_valid("AAAAA===") == true );  // last char 'A' = 0  (even) -> valid
    REQUIRE( default_codec32.is_valid("AAAAC===") == true );  // last char 'C' = 2  (even) -> valid

    REQUIRE( default_codec32.is_valid("AAAA7===") == false ); // last char '7' = 31 (odd) -> invalid
    REQUIRE( default_codec32.is_valid("AAAAB===") == false ); // last char 'B' = 1  (odd) -> invalid
    REQUIRE( default_codec32.is_valid("AAAAD===") == false ); // last char 'D' = 3  (odd) -> invalid
}

TEST_CASE( "Base32: padding=4 requires last char's lowest 4 bits to be zero", "[base32][is_valid]" ) 
{
    REQUIRE( default_codec32.is_valid("AAAA====") == true );  // last char 'A' = 0  (0b00000) -> valid
    REQUIRE( default_codec32.is_valid("AAAQ====") == true );  // last char 'Q' = 16 (0b10000) -> valid

    REQUIRE( default_codec32.is_valid("AAAB====") == false ); // last char 'B' = 1  (0b00001) -> invalid
    REQUIRE( default_codec32.is_valid("AAAP====") == false ); // last char 'P' = 15 (0b01111) -> invalid
}

TEST_CASE( "Base32: padding=6 requires last char's lowest 2 bits to be zero", "[base32][is_valid]" ) 
{
    REQUIRE( default_codec32.is_valid("AA======") == true );  // last char 'A' = 0  (0b00000) -> valid
    REQUIRE( default_codec32.is_valid("AE======") == true );  // last char 'E' = 4  (0b00100) -> valid
    REQUIRE( default_codec32.is_valid("AY======") == true );  // last char 'Y' = 24 (0b11000) -> valid

    REQUIRE( default_codec32.is_valid("AB======") == false ); // last char 'B' = 1  (0b00001) -> invalid
    REQUIRE( default_codec32.is_valid("AF======") == false ); // last char 'F' = 5  (0b00101) -> invalid
    REQUIRE( default_codec32.is_valid("AZ======") == false ); // last char 'Z' = 25 (0b11001) -> invalid
}
