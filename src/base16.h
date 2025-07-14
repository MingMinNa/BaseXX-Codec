#pragma once
#include "base_codec.h"
#include <string>
#include <vector>
#include <cstdint>


class Base16 : public BaseCodec {

    public:
        static const char *base16_alphabet;

        Base16();
        ~Base16();
        std::string encode(const std::vector<uint8_t> &bytes);
        std::vector<uint8_t> decode(const std::string &str);
};