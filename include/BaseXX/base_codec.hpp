#pragma once
#include <span>
#include <string>
#include <vector>
#include <cstdint>
#include <string_view>

namespace BaseXX 
{

const uint8_t ERROR_CODE = 0xff;

class BaseCodec 
{
    public:
        virtual ~BaseCodec() = default;
        virtual std::string encode(std::span<const uint8_t> bytes) = 0;
        virtual std::vector<uint8_t> decode(std::string_view str) = 0;
        virtual bool is_valid(std::string_view str) = 0;
};

}