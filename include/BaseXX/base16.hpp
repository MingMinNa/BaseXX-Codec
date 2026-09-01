#pragma once
#include "base_codec.hpp"
#include <span>
#include <string>
#include <vector>
#include <cstdint>
#include <string_view>

namespace BaseXX 
{
    
class Base16 : public BaseCodec 
{
    public:
        static const char *base16_alphabet;

        Base16();
        ~Base16();
        
        std::string encode(std::span<const uint8_t> bytes);
        std::vector<uint8_t> decode(std::string_view str);
        bool is_valid(std::string_view str);
};

};