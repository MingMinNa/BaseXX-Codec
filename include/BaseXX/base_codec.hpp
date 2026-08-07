#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace BaseXX 
{

const uint8_t ERROR_CODE = 0xff;

class BaseCodec 
{
    public:
        virtual ~BaseCodec() = default;
        virtual std::string encode(const std::vector<uint8_t> &bytes) = 0;
        virtual std::vector<uint8_t> decode(const std::string &str) = 0;
        virtual bool is_valid(const std::string &str) = 0;
};

}