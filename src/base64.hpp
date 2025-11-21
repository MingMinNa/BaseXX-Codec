#pragma once
#include "base_codec.hpp"
#include <string>
#include <vector>
#include <cstdint>


enum class Base64Type {
    DEFAULT, 
    URL,
    PEM         = 64,
    MIME        = 76,
};

class Base64 : public BaseCodec {

    public:
        static const char *base64_alphabet;

        Base64();
        Base64(Base64Type type_);
        ~Base64();
        std::string encode(const std::vector<uint8_t> &bytes);
        std::vector<uint8_t> decode(const std::string &str);
        Base64Type get_type();

    private:
        Base64Type type;
};