#include "src/base16.h"
#include "src/base32.h"
#include "src/base64.h"
#include <iostream>
#include <string>
#include <cstdint>
#include <vector>
using namespace std;

void base16_example(string input);
void base32_example(string input);
void base64_example(string input);

int main(int argc, char *argv[]) {
    string input;
    getline(cin, input);

    base16_example(input);
    base32_example(input);
    base64_example(input);

    return 0;
}

void base16_example(string input) {

    vector<uint8_t> input_bytes(input.begin(), input.end());
    vector<uint8_t> decoding_bytes;
    string encoding_str;
    string decoding_str;

    Base16 handler;
    encoding_str   = handler.encode(input_bytes);
    decoding_bytes = handler.decode(encoding_str);
    decoding_str = string(decoding_bytes.begin(), decoding_bytes.end());

    cout << endl << "------------ Base16 ------------" << endl;
    cout << "Encoding: " << encoding_str << endl;
    cout << "Decoding: " << decoding_str << endl;
    cout << "Is the Same: " << (input == decoding_str) << endl;
    
    return;
}

void base32_example(string input) {

    vector<uint8_t> input_bytes(input.begin(), input.end());
    vector<uint8_t> decoding_bytes;
    string encoding_str;
    string decoding_str;

    // Base32 handler(Base32Type::HEX);
    Base32 handler(Base32Type::DEFAULT); 
    encoding_str   = handler.encode(input_bytes);
    decoding_bytes = handler.decode(encoding_str);
    decoding_str = string(decoding_bytes.begin(), decoding_bytes.end());

    cout << endl << "------------ Base32 ------------" << endl;
    cout << "Encoding: " << encoding_str << endl;
    cout << "Decoding: " << decoding_str << endl;
    cout << "Is the Same: " << (input == decoding_str) << endl;

    return;
}

void base64_example(string input) {

    vector<uint8_t> input_bytes(input.begin(), input.end());
    vector<uint8_t> decoding_bytes;
    string encoding_str;
    string decoding_str;

    // Base64 handler(Base32Type::URL); 
    // Base64 handler(Base32Type::PEM); 
    // Base64 handler(Base32Type::MIME);
    Base64 handler(Base64Type::DEFAULT);
    encoding_str   = handler.encode(input_bytes);
    decoding_bytes = handler.decode(encoding_str);
    decoding_str = string(decoding_bytes.begin(), decoding_bytes.end());

    cout << endl << "------------ Base64 ------------" << endl;
    cout << "Encoding: " << encoding_str << endl;
    cout << "Decoding: " << decoding_str << endl;
    cout << "Is the Same: " << (input == decoding_str) << endl;

    return;
}