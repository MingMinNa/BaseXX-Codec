#include "../include/BaseXX/base_codec.hpp"
#include "../include/BaseXX/base16.hpp"
#include <cctype>
#include <string>
#include <stdexcept>

namespace BaseXX 
{

const char *Base16::base16_alphabet =
    "0123456789" "ABCDEF";

static uint8_t char_to_index(char base16_char) 
{
    if      ('0' <= base16_char && base16_char <= '9') return base16_char - '0';
    else if ('A' <= base16_char && base16_char <= 'F') return base16_char - 'A' + 10;
    else if ('a' <= base16_char && base16_char <= 'f') return base16_char - 'a' + 10;
    return ERROR_CODE;
}

static char get_base16_char(
    const char *alphabet, 
    const uint8_t *bytes_ptr, 
    size_t chunk_index
) {    
    /*
        +--first octet--+
        |7 6 5 4 3 2 1 0|
        +-------+-------+
        |3 2 1 0|3 2 1 0|
        +---1---+---2---+
    */

    size_t index;
    switch (chunk_index) {
        case 1:  /* 1-st chunk */
            index = ((*bytes_ptr) & 0b11110000) >> 4;
            break;
        case 2:  /* 2-nd chunk */
            index = ((*bytes_ptr) & 0b00001111);
            break;
        default:
            throw std::invalid_argument("Invalid chunk index");
            break;
    }

    char base16_char = alphabet[index];
    return base16_char;
}

static uint8_t get_raw_byte(const char *base16_ptr) 
{
    /*
        +--first octet--+
        |7 6 5 4 3 2 1 0|
        +-------+-------+
        |3 2 1 0|3 2 1 0|
        +---1---+---2---+
    */

    uint8_t raw_byte = 0; 

    raw_byte = (char_to_index(*base16_ptr) << 4) | 
                char_to_index(*(base16_ptr + 1));

    return raw_byte;
}

Base16::Base16() = default;
Base16::~Base16() = default;

std::string Base16::encode(std::span<const uint8_t> bytes) 
{
    size_t num_bytes = bytes.size();
    std::string encoding;
    encoding.reserve(num_bytes * 2);
    
    for (size_t curr = 0; curr < num_bytes; ++ curr) {

        const uint8_t *bytes_ptr = bytes.data() + curr;
        
        // split 1 bytes into 2 chunks. The size of each chunk is 4 bits.
        for (size_t chunk = 1; chunk <= 2; ++chunk) {
            char base16_char = get_base16_char(base16_alphabet, bytes_ptr, chunk);
            encoding.push_back(base16_char);
        }
    }

    encoding.shrink_to_fit();
    return encoding;
}

std::vector<uint8_t> Base16::decode(std::string_view str) 
{
    if (!is_valid(str)) {
        throw std::invalid_argument(
            "Invalid base16-encoded string"
        );
    }

    size_t num_chars = str.size();    
    std::vector<uint8_t> raw_data;
    raw_data.reserve(num_chars / 2 + 2);
    
    for (size_t curr = 0; curr < num_chars; curr += 2) {
        const char *base16_ptr = str.data() + curr;
        uint8_t byte_data = get_raw_byte(base16_ptr);
        raw_data.push_back(byte_data);
    }

    raw_data.shrink_to_fit();
    return raw_data;
}

bool Base16::is_valid(std::string_view str)
{
    size_t num_chars = str.size();

    // The size of an valid base16 encoding must be even.
    if (num_chars & 1) return false;

    // All characters must be in base16_alphabet.
    for (size_t curr = 0; curr < num_chars; curr ++) {
        if (char_to_index(str[curr]) == ERROR_CODE) {
            return false;
        }
    }

    return true;
}
   
};