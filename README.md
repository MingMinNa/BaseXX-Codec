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
Please refer to `example.cpp` for usage examples.  
**Note:** The test suite uses the [Catch2](https://github.com/catchorg/Catch2) framework. See [Catch2](https://github.com/catchorg/Catch2) to learn more.

## Installation
To use this library in your own CMake project, you can add it with CMake's `FetchContent` module.  
Add the following to your `CMakeLists.txt`:
```cmake
include(FetchContent)

FetchContent_Declare(
    BaseXX
    GIT_REPOSITORY https://github.com/MingMinNa/BaseXX-Codec.git
    GIT_TAG v1.0.0
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