#pragma once
#include "base_codec.hpp"
#include <span>
#include <string>
#include <vector>
#include <cstdint>
#include <string_view>

namespace BaseXX 
{

enum class Base64Type 
{
    DEFAULT, 
    URL,
    PEM         = 64,
    MIME        = 76,
};

class Base64 : public BaseCodec 
{
    public:
        static const char *base64_alphabet;

        Base64(Base64Type type_ = Base64Type::DEFAULT);
        ~Base64();
        
        std::string encode(std::span<const uint8_t> bytes);
        std::vector<uint8_t> decode(std::string_view str);
        bool is_valid(std::string_view str);
        Base64Type get_type();

    private:
        Base64Type type;
};

}