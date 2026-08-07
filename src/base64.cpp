#include "../include/BaseXX/base_codec.hpp"
#include "../include/BaseXX/base64.hpp"
#include <cctype>
#include <string>
#include <algorithm>
#include <stdexcept>

namespace BaseXX 
{

const char *Base64::base64_alphabet =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789" "+/";

static uint8_t char_to_index(char base64_char) 
{
    if      ('A' <= base64_char && base64_char <= 'Z') return base64_char - 'A';
    else if ('a' <= base64_char && base64_char <= 'z') return base64_char - 'a' + 26;
    else if ('0' <= base64_char && base64_char <= '9') return base64_char - '0' + 52;
    else if (base64_char == '+')                       return 62; // ('Z' - 'A' + 1) * 2 + ('9' - '0') + 1
    else if (base64_char == '/')                       return 63; // ('Z' - 'A' + 1) * 2 + ('9' - '0') + 2
    return ERROR_CODE;
}

static char get_base64_char(
    const char *alphabet, 
    const uint8_t *bytes_ptr, 
    size_t chunk_index
) {
    /* Reference: https://datatracker.ietf.org/doc/html/rfc4648#section-9
        +--first octet--+-second octet--+--third octet--+
        |7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|
        +-----------+---+-------+-------+---+-----------+
        |5 4 3 2 1 0|5 4 3 2 1 0|5 4 3 2 1 0|5 4 3 2 1 0|
        +--1.index--+--2.index--+--3.index--+--4.index--+
    */

    size_t index;
    switch (chunk_index) {
        case 1:  /* 1-st chunk */
            index = ((*bytes_ptr) & 0b11111100) >> 2;
            break;
        case 2:  /* 2-nd chunk */
            index = (((*bytes_ptr) & 0b00000011) << 4) + ((*(bytes_ptr + 1) & 0b11110000) >> 4);
            break;
        case 3:  /* 3-rd chunk */
            index = (((*(bytes_ptr + 1)) & 0b00001111) << 2) + ((((*(bytes_ptr + 2)) & 0b11000000) >> 6));
            break;
        case 4:  /* 4-th chunk */
            index = ((*(bytes_ptr + 2)) & 0b00111111);
            break;
        default:
            throw std::invalid_argument("Invalid chunk index");
            break;
    }

    char base64_char = alphabet[index];
    return base64_char;
}

static uint8_t get_raw_byte(
    const uint8_t *raw_index,
    size_t data_index
) {
    /* Reference: https://datatracker.ietf.org/doc/html/rfc4648#section-9
        +--first octet--+-second octet--+--third octet--+
        |7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|
        +-----------+---+-------+-------+---+-----------+
        |5 4 3 2 1 0|5 4 3 2 1 0|5 4 3 2 1 0|5 4 3 2 1 0|
        +--1.index--+--2.index--+--3.index--+--4.index--+
    */

    uint8_t raw_byte; 

    switch (data_index) {
        case 1:  /* 1-st byte */
            raw_byte = (raw_index[1] << 2) + ((raw_index[2] & 0b110000) >> 4);
            break;
        case 2:  /* 2-nd byte */
            raw_byte = ((raw_index[2] & 0b001111) << 4) + ((raw_index[3] & 0b111100) >> 2);
            break;
        case 3:  /* 3-rd byte */
            raw_byte = ((raw_index[3] & 0b000011) << 6) + (raw_index[4]);
            break;
        default:
            throw std::invalid_argument("Invalid data index");
            break;
    }

    return raw_byte;
}

Base64::Base64(Base64Type type_) : type(type_) {}
Base64::~Base64() = default;

std::string Base64::encode(const std::vector<uint8_t> &bytes) 
{
    size_t num_bytes = bytes.size();
    std::string encoding;
    encoding.reserve(num_bytes / 3 * 4 + 4);
    
    size_t curr = 0;
    size_t num_bytes_3_multiple = num_bytes - num_bytes % 3;

    for (; curr < num_bytes_3_multiple; curr += 3) {

        const uint8_t *bytes_ptr = bytes.data() + curr;

        // split 3 bytes into 4 chunks. The size of each chunk is 6 bits.
        for (size_t chunk = 1; chunk <= 4; ++chunk) {
            char base64_char = get_base64_char(
                this->base64_alphabet, bytes_ptr, chunk
            );
            encoding.push_back(base64_char);
        }
    }

    uint8_t bytes_tail[3] = {0};
    for (size_t i = 0; i < num_bytes % 3; ++i) {
        bytes_tail[i] = bytes[curr + i];
    }

    if (num_bytes % 3 >= 1) {

        size_t num_chunks = (8 * (num_bytes % 3) + 5) / 6; 

        for (size_t chunk = 1; chunk <= num_chunks; ++chunk) {
            encoding.push_back(get_base64_char(
                this->base64_alphabet, bytes_tail, chunk
            ));
        }
        encoding += std::string(4 - num_chunks, '=');
    }

    if (this->get_type() == Base64Type::URL) {
        // '+' -> '-' & '/' -> '_'
        for (char &c : encoding) {
            if      (c == '+') c = '-';
            else if (c == '/') c = '_';
        }
    }
    else if (
        this->get_type() == Base64Type::PEM ||
        this->get_type() == Base64Type::MIME
    ) {
        // insert newline blanks
        // Code Reference: https://github.com/ReneNyffenegger/cpp-base64/blob/master/base64.cpp : insert_linebreaks
        size_t pos = static_cast<size_t>(this->get_type()); 

        while (pos < encoding.size()) {
            encoding.insert(pos, "\n");
            pos += static_cast<size_t>(this->get_type()) + 1;
        }
    }

    encoding.shrink_to_fit();
    return encoding;
}

std::vector<uint8_t> Base64::decode(const std::string &str) 
{
    if (!is_valid(str)) {
        throw std::invalid_argument("Invalid base64 encoding");
    }

    std::string encoding;
    encoding.reserve(str.size());

    for (char c : str) {
        switch (c) {
            case '-':  encoding.push_back('+'); break;  // '-' -> '+'   (URL)
            case '_':  encoding.push_back('/'); break;  // '_' -> '/'   (URL)
            case '\n':                          break;  // drop newline (PEM / MIME)
            case '=':                           break;  // drop padding
            default:   encoding.push_back(c);   break;
        }
    }

    size_t num_chars = encoding.size();
    std::vector<uint8_t> raw_data;
    raw_data.reserve(num_chars * 3 / 4 + 2);

    uint8_t raw_index[5];
    size_t curr = 0;
    size_t num_chars_4_multiple = num_chars - num_chars % 4;

    for (; curr < num_chars_4_multiple; curr += 4) {

        const char *base64_ptr = encoding.c_str() + curr;
        for (size_t i = 1; i <= 4; ++i) {
            raw_index[i] = char_to_index(*(base64_ptr + (i - 1)));
        }

        for (size_t data_index = 1; data_index <= 3; ++data_index) {
            uint8_t byte_data = get_raw_byte(
                raw_index, 
                data_index
            );
            raw_data.push_back(byte_data);
        }
    }

    size_t num_leftover = (num_chars % 4 + 1) >> 1;

    /* 
    // Old version: Case by case

    size_t num_leftover = 0;

    // 1 raw data + padding => 2 base64 characters
    if (num_chars % 4 == 2) {
        num_leftover = 1;
    }   
    // 2 raw data + padding => 3 base64 characters
    else if (num_chars % 4 == 3) {
        num_leftover = 2;
    }
    
    */

    const char *base64_ptr = encoding.c_str() + curr;
    std::fill(raw_index, raw_index + sizeof(raw_index) / sizeof(raw_index[0]), 0);
    
    for (size_t i = 1; i <= num_chars % 4; ++i) {
        raw_index[i] = char_to_index(*(base64_ptr + (i - 1)));
    }

    for (size_t data_index = 1; data_index <= num_leftover; ++data_index) {
        uint8_t byte_data = get_raw_byte(
            raw_index,
            data_index
        );
        raw_data.push_back(byte_data);
    }

    raw_data.shrink_to_fit();
    return raw_data;
}

bool Base64::is_valid(const std::string &str)
{
    // For PEM/MIME, newlines are inserted by encode() and are not data.
    std::string encoding(str);

    if (
        this->get_type() == Base64Type::PEM || 
        this->get_type() == Base64Type::MIME
    ) {
        encoding.erase(
            std::remove(encoding.begin(), encoding.end(), '\n'),
            encoding.end()
        );
    }

    size_t num_raw_chars = encoding.size();

    if (num_raw_chars == 0)     return true;
    if (num_raw_chars % 4 != 0) return false;

    // Count trailing '=' padding characters.
    size_t num_padding = 0;
    while (
        num_padding < num_raw_chars &&
        encoding[num_raw_chars - 1 - num_padding] == '='
    ) {
        ++ num_padding;
    }
    
    bool is_url = (this->get_type() == Base64Type::URL);
    size_t num_chars = num_raw_chars - num_padding;
    uint8_t last_val = char_to_index(str[num_chars - 1]);

    if (is_url) {
        if      (str[num_chars - 1] == '-') last_val = char_to_index('+');
        else if (str[num_chars - 1] == '/') last_val = char_to_index('_');
    } 

    if (last_val == ERROR_CODE) return false;

    if      (num_padding == 1 && last_val & 0b00000011) return false;
    else if (num_padding == 2 && last_val & 0b00001111) return false;
    else if (num_padding > 2)                           return false;

    for (size_t i = 0; i < num_chars; ++i) {
        char c = encoding[i];

        if (is_url) {
            if (c == '+' || c == '/') return false;
            if (c == '-' || c == '_') continue;
        } 
        
        if (char_to_index(c) == ERROR_CODE) {
            return false;
        }
    }

    return true;
}

Base64Type Base64::get_type() 
{
    return this->type;
}

};