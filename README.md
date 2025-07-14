# BaseXX Codec

<a href="README.md">English</a> | <a href="README.zh-TW.md">繁體中文</a>

This project provides a C++ implementation of Base16, Base32, and Base64 codecs.  
Due to the existence of BaseXX codec implementations in multiple programming languages,   
the practical value of this project is relatively limited.  
The main purpose is to practice and get more familiar with C++ programming.

## Building and Running
```bash
mkdir -p build
g++ -Wall -O2 -std=c++20 -c src/base16.cpp -o build/base16.o
g++ -Wall -O2 -std=c++20 -c src/base32.cpp -o build/base32.o
g++ -Wall -O2 -std=c++20 -c src/base64.cpp -o build/base64.o
g++ -Wall -O2 -std=c++20  build/base16.o  build/base32.o  build/base64.o example.cpp -o example -lm
./example
```
Please refer to `example.cpp` for usage examples.

## References
- [RFC4648](https://datatracker.ietf.org/doc/html/rfc4648)
- [ReneNyffenegger/cpp-base64](https://github.com/ReneNyffenegger/cpp-base64/tree/master)