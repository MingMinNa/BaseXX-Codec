# BaseXX Codec

![C++20](https://img.shields.io/badge/C%2B%2B-20-blue) [![License](https://img.shields.io/badge/License-MIT-green)](./LICENSE)

This project provides C++ implementations of Base16, Base32, and Base64 codecs.  
The main purpose is to practice C++ programming and improve my understanding of the language.

## Building and Running
```bash
cmake -S . -B build
cmake --build build

./build/example  # Run the example
./build/tests    # Run the tests
```
Please refer to `example.cpp` for usage examples.  
**Note:** The test suite uses the [Catch2](https://github.com/catchorg/Catch2) framework. See [Catch2](https://github.com/catchorg/Catch2) to learn more.

## References
- [RFC4648](https://datatracker.ietf.org/doc/html/rfc4648)
- [ReneNyffenegger/cpp-base64](https://github.com/ReneNyffenegger/cpp-base64/tree/master)
- [catchorg/Catch2](https://github.com/catchorg/Catch2)