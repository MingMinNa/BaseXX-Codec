#include "src/base64.h"
#include <iostream>
#include <string>
#include <cstdint>
#include <vector>
using namespace std;

int main(int argc, char *argv[]) {

    Base64 base64_obj;
    string input;
    getline(cin, input);
    
    string encoding = base64_obj.encode(vector<uint8_t>(input.begin(), input.end()));
    vector<uint8_t> decoding = base64_obj.decode(encoding);
    string decoding_str = string(decoding.begin(), decoding.end());
    cout << endl;
    cout << "Encoding: " << encoding << endl;
    cout << "Decoding: " << decoding_str << endl;
    cout << "Is the same: " << ((input == decoding_str)?("true"):("false")) << endl;
    return 0;
}