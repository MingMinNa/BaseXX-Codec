#include "base_codec.h"
#include "base32.h"
#include <regex>
#include <cctype>
#include <stdexcept>

#include <iostream>

const char *Base32::base32_alphabets[] = {
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "234567",

    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUV",
};
    

static uint8_t charToIndex(char base32_char, Base32Type type) {

    if(type == Base32Type::DEFAULT) {
        if     ('2' <= base32_char && base32_char <= '7')  return base32_char - '2' + 26;
        else if('A' <= base32_char && base32_char <= 'Z')  return base32_char - 'A';
        else                                               throw  std::runtime_error("Invalid base32 character");
    }
    else if(type == Base32Type::HEX){
        if     (isdigit(base32_char))                      return base32_char - '0';
        else if('A' <= base32_char && base32_char <= 'V')  return base32_char - 'A' + 10;
        else                                               throw  std::runtime_error("Invalid base32 character");
    }
    else 
        throw std::runtime_error("Invalid base32 type");
}

static char getBase32Char(const char *alphabets, const uint8_t *bytes_ptr, int chunk_index) {
    
    /*
        +--first octet--+-second octet--+--third octet--+--fourth octet-+--fifth octet--+
        |7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|
        +---------+---------+---------+---------+---------+---------+---------+---------+
        |4 3 2 1 0|4 3 2 1 0|4 3 2 1 0|4 3 2 1 0|4 3 2 1 0|4 3 2 1 0|4 3 2 1 0|4 3 2 1 0|
        +-1.index-+-2.index-+-3.index-+-4.index-+-5.index-+-6.index-+-7.index-+-8.index-+
    */

    uint32_t index;
    switch (chunk_index) {
        case 1:  /* 1-th chunk */
            index = ((*bytes_ptr) & 0xf8) >> 3;
            break;
        case 2:  /* 2-th chunk */
            index = (((*bytes_ptr) & 0x07) << 2) + (((*(bytes_ptr + 1)) & 0xc0) >> 6);
            break;
        case 3:  /* 3-th chunk */
            index = (((*(bytes_ptr + 1)) & 0x3e) >> 1);
            break;
        case 4:  /* 4-th chunk */
            index = (((*(bytes_ptr + 1)) & 0x01) << 4) + (((*(bytes_ptr + 2)) & 0xf0) >> 4);
            break;
        case 5:  /* 5-th chunk */
            index = (((*(bytes_ptr + 2)) & 0x0f) << 1) + (((*(bytes_ptr + 3)) & 0x80) >> 7);
            break;
        case 6:  /* 6-th chunk */
            index = (((*(bytes_ptr + 3)) & 0x7c) >> 2);
            break;
        case 7:  /* 7-th chunk */
            index = (((*(bytes_ptr + 3)) & 0x03) << 3) + (((*(bytes_ptr + 4)) & 0xe0) >> 5);
            break;
        case 8:  /* 8-th chunk */
            index = ((*(bytes_ptr + 4)) & 0x1f);
            break;    
        default:
            throw std::runtime_error("Invalid chunk index");
            break;
    }

    char base32_char = alphabets[index];
    return base32_char;
}

static uint8_t getRawByte(const char *base32_ptr, int data_index, Base32Type type, int num_chars) {

    /*
        +--first octet--+-second octet--+--third octet--+--fourth octet-+--fifth octet--+
        |7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|
        +---------+---------+---------+---------+---------+---------+---------+---------+
        |4 3 2 1 0|4 3 2 1 0|4 3 2 1 0|4 3 2 1 0|4 3 2 1 0|4 3 2 1 0|4 3 2 1 0|4 3 2 1 0|
        +-1.index-+-2.index-+-3.index-+-4.index-+-5.index-+-6.index-+-7.index-+-8.index-+
    */

    uint8_t raw_byte; 
    uint8_t raw_index[9];
    for(int i = 1; i <= num_chars; ++i){
        raw_index[i] = charToIndex(*(base32_ptr + (i - 1)), type);
    }

    switch (data_index) {
        case 1:  /* 1-th byte */
            raw_byte = (raw_index[1] << 3) + ((raw_index[2] & 0x1c) >> 2);
            break;
        case 2:  /* 2-th byte */
            raw_byte = ((raw_index[2] & 0x03) << 6) + ((raw_index[3]) << 1) + ((raw_index[4] & 0x10) >> 4);
            break;
        case 3:  /* 3-th byte */
            raw_byte = ((raw_index[4] & 0x0f) << 4) + ((raw_index[5] & 0x1e) >> 1);
            break;
        case 4:  /* 4-th byte */
            raw_byte = ((raw_index[5] & 0x01) << 7) + ((raw_index[6]) << 2) + ((raw_index[7] & 0x18) >> 3);
            break;
        case 5:  /* 5-th byte */
            raw_byte = ((raw_index[7] & 0x07) << 5) + raw_index[8];
            break;
        default:
            throw std::runtime_error("Invalid data index");
            break;
    }

    return raw_byte;
}

Base32::Base32() : Base32::Base32(Base32Type::DEFAULT) {}
Base32::Base32(Base32Type type_) : type(type_) {}

Base32::~Base32() = default;

std::string Base32::encode(const std::vector<uint8_t> &bytes) {
    size_t num_bytes = bytes.size();
    std::string encoding;
    encoding.reserve(num_bytes / 5 * 8 + 10);
    
    size_t curr = 0;
    size_t num_bytes_3_multiple = num_bytes - num_bytes % 5;

    for(; curr < num_bytes_3_multiple; curr += 5) {

        const uint8_t *bytes_ptr = bytes.data() + curr;
        // split 5 bytes into 8 chunks. The size of each chunk is 5 bits.
        for(int chunk = 1; chunk <= 8; ++chunk){
            char base32_char = getBase32Char(
                this->base32_alphabets[static_cast<int>(this->getType())], bytes_ptr, chunk
            );
            encoding.push_back(base32_char);
        }
    }
    uint8_t bytes_tail[5] = {0};
    for(uint8_t i = 0; i < num_bytes % 5; ++i){
        bytes_tail[i] = bytes[curr + i];
    }

    if(num_bytes % 5 >= 1) {
        /* 
            1 bytes => 2 chunks
            2 bytes => 4 chunks
            3 bytes => 5 chunks
            4 bytes => 7 chunks
        */

        // Ceiling
        int num_chunks = (8 * (num_bytes % 5) + 4) / 5; 
        
        for(int chunk = 1; chunk <= num_chunks; ++chunk){
            encoding.push_back(getBase32Char(
                this->base32_alphabets[static_cast<int>(this->getType())], bytes_tail, chunk
            ));
        }
        encoding += std::string(8 - num_chunks, '=');
    }

    return encoding;
}

std::vector<uint8_t> Base32::decode(const std::string &str) {

    std::string encoding(str);
    
    // clear '='
    std::regex pattern_assign("=");
    encoding = std::move(std::regex_replace(encoding, pattern_assign, ""));

    size_t num_chars = encoding.size();
    std::vector<uint8_t> raw_data;
    raw_data.reserve(num_chars * 5 / 8 + 10);

    
    size_t curr = 0;
    size_t num_chars_8_multiple = num_chars - num_chars % 8;

    for(; curr < num_chars_8_multiple; curr += 8) {

        const char *base32_ptr = encoding.c_str() + curr;

        for(int data_index = 1; data_index <= 5; ++data_index){
            uint8_t byte_data = getRawByte(
                base32_ptr, data_index, this->getType(), 8
            );
            raw_data.push_back(byte_data);
        }
    }
    
    int num_leftover = 0;
    if(num_chars % 8 == 2) {
        // 1 bytes + padding => 2 base32 characters
        num_leftover = 1;
    }   
    else if(num_chars % 8 == 4) {
        // 2 bytes + padding => 4 base32 characters
        num_leftover = 2;
    } 
    else if(num_chars % 8 == 5) {
        // 3 bytes + padding => 5 base32 characters
        num_leftover = 3;
    }
    else if(num_chars % 8 == 7) {
        // 4 bytes + padding => 7 base32 characters
        num_leftover = 4;
    }
    else if(num_chars % 8 > 0)
        throw std::runtime_error("Invalid base32 encoding");
    
    const char *base32_ptr = encoding.c_str() + curr;
    for(int data_index = 1; data_index <= num_leftover; ++data_index){
        uint8_t byte_data = getRawByte(
            base32_ptr, data_index, this->getType(), (num_chars % 8)
        );
        raw_data.push_back(byte_data);
    }

    return raw_data;
}

Base32Type Base32::getType() {
    return this->type;
}
