#pragma once
#include <string>
#include <cstdint>
#include <unordered_map>

class BaseCodec {

    public:
        BaseCodec() = default;
        virtual ~BaseCodec() = default;
        virtual std::string encode(const uint8_t *bytes, size_t num_bytes) = 0;
        virtual std::string decode(const std::string &str) = 0;
};