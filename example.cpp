#include "src/base16.hpp"
#include "src/base32.hpp"
#include "src/base64.hpp"
#include <iostream>
#include <string>
#include <cstdint>
#include <vector>

void base16_example(std::string input);
void base32_example(std::string input);
void base64_example(std::string input);

int main(int argc, char *argv[]) {
    
    std::string input;
    std::cout << "Please input a string: ";
    std::getline(std::cin, input);

    base16_example(input);
    base32_example(input);
    base64_example(input);

    return 0;
}

void base16_example(std::string input) {

    std::vector<uint8_t> input_bytes(input.begin(), input.end());
    std::vector<uint8_t> decoding_bytes;
    std::string encoding_str;
    std::string decoding_str;

    Base16 handler;
    encoding_str   = handler.encode(input_bytes);
    decoding_bytes = handler.decode(encoding_str);
    decoding_str   = std::string(decoding_bytes.begin(), decoding_bytes.end());

    std::cout << std::endl << "------------ Base16 ------------" << std::endl;
    std::cout << "Encoding: " << encoding_str << std::endl;
    std::cout << "Decoding: " << decoding_str << std::endl;
    std::cout << "Is the Same: " << ((input == decoding_str) ? ("true") : ("false")) << std::endl;
    
    return;
}

void base32_example(std::string input) {

    std::vector<uint8_t> input_bytes(input.begin(), input.end());
    std::vector<uint8_t> decoding_bytes;
    std::string encoding_str;
    std::string decoding_str;

    // Base32 handler(Base32Type::HEX);
    Base32 handler(Base32Type::DEFAULT); 
    encoding_str   = handler.encode(input_bytes);
    decoding_bytes = handler.decode(encoding_str);
    decoding_str   = std::string(decoding_bytes.begin(), decoding_bytes.end());

    std::cout << std::endl << "------------ Base32 ------------" << std::endl;
    std::cout << "Encoding: " << encoding_str << std::endl;
    std::cout << "Decoding: " << decoding_str << std::endl;
    std::cout << "Is the Same: " << ((input == decoding_str) ? ("true") : ("false")) << std::endl;

    return;
}

void base64_example(std::string input) {

    std::vector<uint8_t> input_bytes(input.begin(), input.end());
    std::vector<uint8_t> decoding_bytes;
    std::string encoding_str;
    std::string decoding_str;

    // Base64 handler(Base32Type::URL); 
    // Base64 handler(Base32Type::PEM); 
    // Base64 handler(Base32Type::MIME);
    Base64 handler(Base64Type::DEFAULT);
    encoding_str   = handler.encode(input_bytes);
    decoding_bytes = handler.decode(encoding_str);
    decoding_str   = std::string(decoding_bytes.begin(), decoding_bytes.end());

    std::cout << std::endl << "------------ Base64 ------------" << std::endl;
    std::cout << "Encoding: " << encoding_str << std::endl;
    std::cout << "Decoding: " << decoding_str << std::endl;
    std::cout << "Is the Same: " << ((input == decoding_str) ? ("true") : ("false")) << std::endl;

    return;
}