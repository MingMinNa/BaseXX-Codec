#pragma once
#include "base_codec.h"
#include <string>
#include <vector>
#include <cstdint>

enum Base64Type {
    DEFAULT, 
    URL,
    PEM         = 64,
    MIME        = 76,
};

class Base64 : public BaseCodec {

    public:
        static const char *base64_alphabet;

        Base64();
        Base64(enum Base64Type type_);
        ~Base64();
        std::string encode(const std::vector<uint8_t> &bytes);
        std::vector<uint8_t> decode(const std::string &str);
        enum Base64Type getType();

    private:
        enum Base64Type type;
};