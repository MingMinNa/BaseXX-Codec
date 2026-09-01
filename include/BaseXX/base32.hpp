#pragma once
#include "base_codec.hpp"
#include <span>
#include <string>
#include <vector>
#include <cstdint>
#include <string_view>

namespace BaseXX 
{

enum class Base32Type 
{
    DEFAULT = 0,    // base32_alphabets[0]
    HEX     = 1,    // base32_alphabets[1]
};

class Base32 : public BaseCodec 
{
    public:
        static const char *base32_alphabets[2];

        Base32(Base32Type type_ = Base32Type::DEFAULT);
        ~Base32();
        
        std::string encode(std::span<const uint8_t> bytes);
        std::vector<uint8_t> decode(std::string_view str);
        bool is_valid(std::string_view str);
        Base32Type get_type();
    
    private:
        Base32Type type;
};

};