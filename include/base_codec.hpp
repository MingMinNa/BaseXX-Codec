#pragma once
#include <string>
#include <vector>
#include <cstdint>


class BaseCodec {

    public:
        BaseCodec() = default;
        virtual ~BaseCodec() = default;
        virtual std::string encode(const std::vector<uint8_t> &bytes) = 0;
        virtual std::vector<uint8_t> decode(const std::string &str) = 0;
};