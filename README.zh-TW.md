# BaseXX 編解碼器

<a href="README.md">English</a> | <a href="README.zh-TW.md">繁體中文</a>

本專案為使用 C++ 的 Base16, Base32 跟 Base64 編解碼器之實作。  
由於許多程式語言均已實作了 BaseXX 編解碼器，故本專案之實際價值較低。  
專案目的在於藉由撰寫專案的過程，訓練與熟悉 C++ 程式設計。

## 建置與執行
```bash
mkdir -p build
g++ -Wall -O2 -std=c++20 -c src/base16.cpp -o build/base16.o
g++ -Wall -O2 -std=c++20 -c src/base32.cpp -o build/base32.o
g++ -Wall -O2 -std=c++20 -c src/base64.cpp -o build/base64.o
g++ -Wall -O2 -std=c++20  build/base16.o  build/base32.o  build/base64.o example.cpp -o example -lm
./example
```
使用範例請參考 `example.cpp`。

## 參考資料
- [RFC4648](https://datatracker.ietf.org/doc/html/rfc4648)
- [ReneNyffenegger/cpp-base64](https://github.com/ReneNyffenegger/cpp-base64/tree/master)