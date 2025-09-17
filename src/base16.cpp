#include "base_codec.h"
#include "base16.h"
#include <regex>
#include <cctype>
#include <stdexcept>


const char *Base16::base16_alphabet =
    "0123456789"
    "ABCDEF";


static uint8_t char_to_index(char base16_char) {

    if     ('0' <= base16_char && base16_char <= '9')  return base16_char - '0';
    else if('A' <= base16_char && base16_char <= 'F')  return base16_char - 'A' + 10;
    else                                               throw  std::runtime_error("Invalid base16 character");
}

static char get_base16_char(const char *alphabet, const uint8_t *bytes_ptr, int chunk_index) {
    
    /*
        +--first octet--+
        |7 6 5 4 3 2 1 0|
        +-------+-------+
        |3 2 1 0|3 2 1 0|
        +---1---+---2---+
    */

    uint32_t index;
    switch (chunk_index) {
        case 1:  /* 1-th chunk */
            index = ((*bytes_ptr) & 0b11110000) >> 4;
            break;
        case 2:  /* 2-th chunk */
            index = ((*bytes_ptr) & 0b00001111);
            break;
        default:
            throw std::runtime_error("Invalid chunk index");
            break;
    }

    char base16_char = alphabet[index];
    return base16_char;
}

static uint8_t get_raw_byte(const char *base16_ptr) {

    /*
        +--first octet--+
        |7 6 5 4 3 2 1 0|
        +-------+-------+
        |3 2 1 0|3 2 1 0|
        +---1---+---2---+
    */

    uint8_t raw_byte = 0; 
    for(int i = 0; i < 2; ++i){
        raw_byte = (raw_byte << 4) | char_to_index(*(base16_ptr + i));
    }

    return raw_byte;
}

Base16::Base16() = default;
Base16::~Base16() = default;

std::string Base16::encode(const std::vector<uint8_t> &bytes) {

    size_t num_bytes = bytes.size();
    std::string encoding;
    encoding.reserve(num_bytes * 2);
    
    for(size_t curr = 0; curr < num_bytes; ++ curr) {

        const uint8_t *bytes_ptr = bytes.data() + curr;
        // split 1 bytes into 2 chunks. The size of each chunk is 4 bits.
        for(int chunk = 1; chunk <= 2; ++chunk){
            char base16_char = get_base16_char(
                this->base16_alphabet, bytes_ptr, chunk
            );
            encoding.push_back(base16_char);
        }
    }

    return encoding;
}

std::vector<uint8_t> Base16::decode(const std::string &str) {

    std::string encoding(str);

    size_t num_chars = encoding.size();
    std::vector<uint8_t> raw_data;
    raw_data.reserve(num_chars / 2 + 2);

    if(num_chars & 1)   // the size of an valid base16 encoding must be even
        throw std::runtime_error("Invalid base16 encoding");
    

    for(size_t curr = 0; curr < num_chars; curr += 2) {

        const char *base16_ptr = encoding.c_str() + curr;
        uint8_t byte_data = get_raw_byte(base16_ptr);
        raw_data.push_back(byte_data);
    }

    return raw_data;
}

