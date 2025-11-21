#include "base_codec.hpp"
#include "base64.hpp"
#include <regex>
#include <cctype>
#include <stdexcept>


const char *Base64::base64_alphabet =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "+/";


static uint8_t char_to_index(char base64_char) {

    if     ('A' <= base64_char && base64_char <= 'Z')   return base64_char - 'A';
    else if('a' <= base64_char && base64_char <= 'z')   return base64_char - 'a' + ('Z' - 'A' + 1);
    else if('0' <= base64_char && base64_char <= '9')   return base64_char - '0' + ('Z' - 'A' + 1) * 2;
    else if(base64_char == '+')                         return 62; // ('Z' - 'A' + 1) * 2 + ('9' - '0') + 1
    else if(base64_char == '/')                         return 63; // ('Z' - 'A' + 1) * 2 + ('9' - '0') + 2
    throw  std::runtime_error("Invalid base64 character");
}

static char get_base64_char(const char *alphabet, const uint8_t *bytes_ptr, size_t chunk_index) {
    
    /* Reference: https://datatracker.ietf.org/doc/html/rfc4648#section-9
        +--first octet--+-second octet--+--third octet--+
        |7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|
        +-----------+---+-------+-------+---+-----------+
        |5 4 3 2 1 0|5 4 3 2 1 0|5 4 3 2 1 0|5 4 3 2 1 0|
        +--1.index--+--2.index--+--3.index--+--4.index--+
    */

    size_t index;
    switch (chunk_index) {
        case 1:  /* 1-th chunk */
            index = ((*bytes_ptr) & 0b11111100) >> 2;
            break;
        case 2:  /* 2-th chunk */
            index = (((*bytes_ptr) & 0b00000011) << 4) + ((*(bytes_ptr + 1) & 0b11110000) >> 4);
            break;
        case 3:  /* 3-th chunk */
            index = (((*(bytes_ptr + 1)) & 0b00001111) << 2) + ((((*(bytes_ptr + 2)) & 0b11000000) >> 6));
            break;
        case 4:  /* 4-th chunk */
            index = ((*(bytes_ptr + 2)) & 0b00111111);
            break;
        default:
            throw std::runtime_error("Invalid chunk index");
            break;
    }

    char base64_char = alphabet[index];
    return base64_char;
}

static uint8_t get_raw_byte(const char *base64_ptr, size_t data_index, size_t num_chars) {

    /* Reference: https://datatracker.ietf.org/doc/html/rfc4648#section-9
        +--first octet--+-second octet--+--third octet--+
        |7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|
        +-----------+---+-------+-------+---+-----------+
        |5 4 3 2 1 0|5 4 3 2 1 0|5 4 3 2 1 0|5 4 3 2 1 0|
        +--1.index--+--2.index--+--3.index--+--4.index--+
    */

    uint8_t raw_byte; 
    uint8_t raw_index[5];
    for(size_t i = 1; i <= num_chars; ++i){
        raw_index[i] = char_to_index(*(base64_ptr + (i - 1)));
    }

    switch (data_index) {
        case 1:  /* 1-th byte */
            raw_byte = (raw_index[1] << 2) + ((raw_index[2] & 0b110000) >> 4);
            break;
        case 2:  /* 2-th byte */
            raw_byte = ((raw_index[2] & 0b001111) << 4) + ((raw_index[3] & 0b111100) >> 2);
            break;
        case 3:  /* 3-th byte */
            raw_byte = ((raw_index[3] & 0b000011) << 6) + (raw_index[4]);
            break;
        default:
            throw std::runtime_error("Invalid data index");
            break;
    }

    return raw_byte;
}

Base64::Base64() : Base64::Base64(Base64Type::DEFAULT) {}
Base64::Base64(Base64Type type_) : type(type_) {}

Base64::~Base64() = default;

std::string Base64::encode(const std::vector<uint8_t> &bytes) {

    size_t num_bytes = bytes.size();
    std::string encoding;
    encoding.reserve(num_bytes / 3 * 4 + 4);
    
    size_t curr = 0;
    size_t num_bytes_3_multiple = num_bytes - num_bytes % 3;

    for(; curr < num_bytes_3_multiple; curr += 3) {

        const uint8_t *bytes_ptr = bytes.data() + curr;
        // split 3 bytes into 4 chunks. The size of each chunk is 6 bits.
        for(size_t chunk = 1; chunk <= 4; ++chunk){
            char base64_char = get_base64_char(
                this->base64_alphabet, bytes_ptr, chunk
            );
            encoding.push_back(base64_char);
        }
    }

    uint8_t bytes_tail[3] = {0};
    for(size_t i = 0; i < num_bytes % 3; ++i){
        bytes_tail[i] = bytes[curr + i];
    }

    if(num_bytes % 3 >= 1) {

        size_t num_chunks = (8 * (num_bytes % 3) + 5) / 6; 

        for(size_t chunk = 1; chunk <= num_chunks; ++chunk){
            encoding.push_back(get_base64_char(
                this->base64_alphabet, bytes_tail, chunk
            ));
        }
        encoding += std::string(4 - num_chunks, '=');
    }

    if(this->get_type() == Base64Type::URL) {
        // '+' -> '-' & '/' -> '_'
        std::regex pattern_plus("\\+"), pattern_slash("/");
        encoding = std::move(std::regex_replace(encoding, pattern_plus, "-"));
        encoding = std::move(std::regex_replace(encoding, pattern_slash, "_"));
    }
    else if(this->get_type() == Base64Type::PEM ||
            this->get_type() == Base64Type::MIME) {
        
        // insert newline blanks
        // Code Reference: https://github.com/ReneNyffenegger/cpp-base64/blob/master/base64.cpp : insert_linebreaks
        size_t pos = static_cast<size_t>(this->get_type()); 

        while (pos < encoding.size()) {
            encoding.insert(pos, "\n");
            pos += static_cast<size_t>(this->get_type()) + 1;
        }
    }

    return encoding;
}

std::vector<uint8_t> Base64::decode(const std::string &str) {

    std::string encoding(str);

    /* for type == URL*/
    // '-' -> '+' & '_' -> '/'
    std::regex pattern_minus("-"), pattern_underline("_");
    encoding = std::move(std::regex_replace(encoding, pattern_minus    , "+"));
    encoding = std::move(std::regex_replace(encoding, pattern_underline, "/"));

    /* for type == PEM || type == MIME */
    // clear '\n'
    std::regex pattern_newline("\n");
    encoding = std::move(std::regex_replace(encoding, pattern_newline, ""));

    // clear '='
    std::regex pattern_assign("=");
    encoding = std::move(std::regex_replace(encoding, pattern_assign, ""));

    size_t num_chars = encoding.size();
    std::vector<uint8_t> raw_data;
    raw_data.reserve(num_chars * 3 / 4 + 2);

    size_t curr = 0;
    size_t num_chars_4_multiple = num_chars - num_chars % 4;

    for(; curr < num_chars_4_multiple; curr += 4) {

        const char *base64_ptr = encoding.c_str() + curr;

        for(size_t data_index = 1; data_index <= 3; ++data_index){
            uint8_t byte_data = get_raw_byte(
                base64_ptr, data_index, 4
            );
            raw_data.push_back(byte_data);
        }
    }

    size_t num_leftover = 0;
    if(num_chars % 4 == 2) {
        // 1 raw data + padding => 2 base64 characters
        num_leftover = 1;
    }   
    else if(num_chars % 4 == 3) {
        // 2 raw data + padding => 3 base64 characters
        num_leftover = 2;
    } 
    else if(num_chars % 4 > 0)
        throw std::runtime_error("Invalid base64 encoding");

    const char *base64_ptr = encoding.c_str() + curr;
    for(size_t data_index = 1; data_index <= num_leftover; ++data_index){
        uint8_t byte_data = get_raw_byte(
            base64_ptr, data_index, (num_chars % 4)
        );
        raw_data.push_back(byte_data);
    }

    return raw_data;
}

Base64Type Base64::get_type() {
    return this->type;
}
