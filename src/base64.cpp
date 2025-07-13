#include "base_codec.h"
#include "base64.h"
#include <regex>
#include <stdexcept>

#include <iostream>
using namespace std;


const char *Base64::base64_alphabet =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "+/";


static char getBase64Char(const char *alphabet, const uint8_t *bytes_ptr, int chunk_index) {
    
    /* Reference: https://datatracker.ietf.org/doc/html/rfc4648#section-9
        +--first octet--+-second octet--+--third octet--+
        |7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|
        +-----------+---+-------+-------+---+-----------+
        |5 4 3 2 1 0|5 4 3 2 1 0|5 4 3 2 1 0|5 4 3 2 1 0|
        +--1.index--+--2.index--+--3.index--+--4.index--+
    */

    uint32_t index;
    switch (chunk_index) {
        case 1:  /* 1-th chunk */
            index = ((*bytes_ptr) & 0xfc) >> 2;
            break;
        case 2:  /* 2-th chunk */
            index = (((*bytes_ptr) & 0x03) << 4) + ((*(bytes_ptr + 1) & 0xf0) >> 4);
            break;
        case 3:  /* 3-th chunk */
            index = (((*(bytes_ptr + 1)) & 0x0f) << 2) + ((((*(bytes_ptr + 2)) & 0xc0) >> 6));
            break;
        case 4:  /* 4-th chunk */
            index = ((*(bytes_ptr + 2)) & 0x3f);
            break;
        default:
            throw std::runtime_error("Invalid chunk index");
            break;
    }

    char base64_char = alphabet[index];
    return base64_char;
}

Base64::Base64() : Base64::Base64(DEFAULT) {}
Base64::Base64(enum Base64Type type_) : type(type_) {}

Base64::~Base64() = default;

std::string Base64::encode(const uint8_t *bytes, size_t num_bytes) {
    std::string encoding;
    encoding.reserve(num_bytes / 3 * 4 + 4);
    
    size_t curr = 0;
    size_t num_bytes_3_multiple = num_bytes - num_bytes % 3;

    for(; curr < num_bytes_3_multiple; curr += 3) {

        const uint8_t *bytes_ptr = bytes + curr;
        // split 3 bytes into 4 chunks. The size of each chunk is 6 bits.
        for(int chunk = 1; chunk <= 4; ++chunk){
            char base64_char = getBase64Char(
                this->base64_alphabet, bytes_ptr, chunk
            );
            encoding.push_back(base64_char);
        }
    }
    uint8_t bytes_tail[3] = {0};
    for(int i = 0; i < num_bytes % 3; ++i){
        bytes_tail[i] = bytes[curr + i];
    }

    if(num_bytes % 3 == 1) {
        
        for(int chunk = 1; chunk <= 2; ++chunk){
            encoding.push_back(getBase64Char(
                this->base64_alphabet, bytes_tail, chunk
            ));
        }
        encoding += "==";
    }
    else if(num_bytes % 3 == 2) {
        
        for(int chunk = 1; chunk <= 3; ++chunk){
            encoding.push_back(getBase64Char(
                this->base64_alphabet, bytes_tail, chunk
            ));
        }
        encoding.push_back('=');
    }

    if(this->type == URL) {
        // (1) '+' -> '-' & '/' -> '_'
        std::regex pattern_plus("+"), pattern_slash("/");
        encoding = std::move(std::regex_replace(encoding, pattern_plus, "-"));
        encoding = std::move(std::regex_replace(encoding, pattern_slash, "_"));
    }
    else if(this->type == PEM ||
            this->type == MIME) {
        
        // insert newline blanks
        // Code Reference: https://github.com/ReneNyffenegger/cpp-base64/blob/master/base64.cpp : insert_linebreaks
        size_t pos = this->type; 

        while (pos < encoding.size()) {
            encoding.insert(pos, "\n");
            pos += this->type + 1;
        }
    }

    return encoding;
}

std::string Base64::decode(const std::string &str) {
    return "TODO";
}

enum Base64Type Base64::getType() {
    return this->type;
}
