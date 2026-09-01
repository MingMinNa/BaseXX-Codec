# BaseXX Codec

![C++20](https://img.shields.io/badge/C%2B%2B-20-blue) [![License](https://img.shields.io/badge/License-MIT-green)](./LICENSE)

This project provides C++ implementations of Base16, Base32, and Base64 codecs.  
The main purpose is to practice C++ programming and improve my understanding of the language.

## Building and Running
```bash
$ cmake -S . -B build -DBASEXX_BUILD_EXAMPLES=ON -DBASEXX_BUILD_TESTS=ON
$ cmake --build build

$ ./build/example  # Run the example
$ ./build/tests    # Run the tests
```

**Note:** The test suite uses the Catch2 framework. See [Catch2](https://github.com/catchorg/Catch2) to learn more.

## Usage
Each codec class inherits from `BaseCodec` and provides the following interface:

```C++
std::string encode(std::span<const uint8_t> bytes);   // → encode bytes (uint8) into an encoded string
std::vector<uint8_t> decode(std::string_view str);    // → decode an encoded string back into bytes (uint8)
bool is_valid(std::string_view str);                  // → check whether the encoded string is valid
```

### Example
```C++
#include "include/BaseXX/base16.hpp"
#include "include/BaseXX/base32.hpp"
#include "include/BaseXX/base64.hpp"
#include <string>
#include <vector>
#include <cstdint>

int main()
{
    std::vector<uint8_t> data = {'h', 'e', 'l', 'l', 'o'};
    
    // "BaseXX" is the namespace.
    // "Base16" / "Base32" / "Base64" are the classes.
    // "encode" / "decode" / "is_valid" are the methods.
    // "get_type" method is only available for "Base32" and "Base64" classes.
    
    BaseXX::Base16 codec16;
    
    // Optional types: DEFAULT, HEX
    BaseXX::Base32 codec32(BaseXX::Base32Type::DEFAULT);

    // Optional types: DEFAULT, URL, PEM and MIME
    BaseXX::Base64 codec64(BaseXX::Base64Type::DEFAULT);

    // Usage is the same across all codecs; Base16 is shown here as an example.
    std::string encoded_str = codec16.encode(data);
    std::vector<uint8_t> decoded_bytes = codec16.decode(encoded_str);

    // It is expected that `decoded_bytes == data`.
}
```

Please refer to [`example.cpp`](./example.cpp) for usage examples.  

## Installation
To use this library in your own CMake project, you can add it with CMake's `FetchContent` module.  
Add the following to your `CMakeLists.txt`:
```cmake
include(FetchContent)

FetchContent_Declare(
    BaseXX
    GIT_REPOSITORY https://github.com/MingMinNa/BaseXX-Codec.git
    GIT_TAG v1.1.0
)

FetchContent_MakeAvailable(BaseXX)

add_executable(<target_name>
    <source_file>.cpp 
    ...
)

target_link_libraries(<target_name>
    PRIVATE
        BaseXX::basexx
)
```

You can then include the library headers in your source file as follows:
```C++
#include <BaseXX/base16.hpp>
#include <BaseXX/base32.hpp>
#include <BaseXX/base64.hpp>
```

## References
- [RFC4648](https://datatracker.ietf.org/doc/html/rfc4648)
- [ReneNyffenegger/cpp-base64](https://github.com/ReneNyffenegger/cpp-base64/tree/master)
- [catchorg/Catch2](https://github.com/catchorg/Catch2)