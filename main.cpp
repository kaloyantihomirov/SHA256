#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cstdint>
#include <vector>
#include <bitset>

using namespace std;

string readTextFromFile(const string & fileName)
{
    ifstream fStream(fileName);

    string result;
    string line;

    if (fStream.is_open())
    {
        while (getline(fStream, line))
        {
            if (fStream.eof())
            {
                result += line;
            }
            else
            {
                result += line + '\n';
            }
        }
    }
    else
    {
        cout << "Error opening file: " << fileName << endl;
    }

    return result;
}

void writeTextToFile(const string& text, const string& fileName)
{
    ofstream ofstream1;

    ofstream1.open(fileName);

    ofstream1 << text;

    ofstream1.close();
}

string encodeStringToBinary(const string & input)
{
    string binaryString;

    for (char character : input) {
        binaryString += bitset<8>(character).to_string();
    }

    return binaryString;
}

// Add padding and length to the input
string preprocess(const string & input) {
    // Step 1: Append a single '1' bit
    string result = input + '1';

    // Step 2: Append K '0' bits, where K is the minimum number >= 0
    // such that (L + 1 + K + 64) is a multiple of 512
    size_t originalLength = input.length() * 8;
    size_t k = 0;
    while ((result.length() + k) % 512 != 448) {
        k++;
    }
    result += string(k, '0');

    result += bitset<8>(originalLength).to_string();


    return result;
}

// Right rotate operation
uint32_t rightRotate(uint32_t value, int count) {
    return (value >> count) | (value << (32 - count));
}

// Process a 512-bit chunk of data
void processChunk(const char* chunk, uint32_t& h0, uint32_t& h1, uint32_t& h2, uint32_t& h3, uint32_t& h4, uint32_t& h5, uint32_t& h6, uint32_t& h7, const uint32_t k[]) {
    // Create a 64-entry message schedule array w[0..63] of 32-bit words
    vector<uint32_t> w(64, 0);

    // Copy chunk into first 16 words w[0..15] of the message schedule array
    for (int i = 0; i < 16; i++) {
        w[i] = static_cast<uint32_t>(
                (static_cast<uint8_t>(chunk[i * 4]) << 24) |
                (static_cast<uint8_t>(chunk[i * 4 + 1]) << 16) |
                (static_cast<uint8_t>(chunk[i * 4 + 2]) << 8) |
                static_cast<uint8_t>(chunk[i * 4 + 3])
        );
    }

    // Extend the first 16 words into the remaining 48 words w[16..63] of the message schedule array
    for (int i = 16; i < 64; i++) {
        uint32_t s0 = rightRotate(w[i - 15], 7) ^ rightRotate(w[i - 15], 18) ^ (w[i - 15] >> 3);
        uint32_t s1 = rightRotate(w[i - 2], 17) ^ rightRotate(w[i - 2], 19) ^ (w[i - 2] >> 10);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }

    // Initialize working variables to current hash value
    uint32_t a = h0;
    uint32_t b = h1;
    uint32_t c = h2;
    uint32_t d = h3;
    uint32_t e = h4;
    uint32_t f = h5;
    uint32_t g = h6;
    uint32_t h = h7;

    // Compression function main loop
    for (int i = 0; i < 64; i++) {
        uint32_t S1 = rightRotate(e, 6) ^ rightRotate(e, 11) ^ rightRotate(e, 25);
        uint32_t ch = (e & f) ^ (~e & g);
        uint32_t temp1 = h + S1 + ch + k[i] + w[i];
        uint32_t S0 = rightRotate(a, 2) ^ rightRotate(a, 13) ^ rightRotate(a, 22);
        uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
        uint32_t temp2 = S0 + maj;

        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }

    // Add the compressed chunk to the current hash value
    h0 += a;
    h1 += b;
    h2 += c;
    h3 += d;
    h4 += e;
    h5 += f;
    h6 += g;
    h7 += h;
}

string hash256(const string& input) {
    // Initialize hash values
    uint32_t h0 = 0x6a09e667;
    uint32_t h1 = 0xbb67ae85;
    uint32_t h2 = 0x3c6ef372;
    uint32_t h3 = 0xa54ff53a;
    uint32_t h4 = 0x510e527f;
    uint32_t h5 = 0x9b05688c;
    uint32_t h6 = 0x1f83d9ab;
    uint32_t h7 = 0x5be0cd19;

    // Initialize array of round constants
    const uint32_t k[64] = {
            0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
            0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
            0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
            0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
            0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
            0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
            0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
            0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    string encoded = encodeStringToBinary(input);

    // Pre-processing
    string processedInput = preprocess(encoded);

    cout << processedInput << endl;

    // Process each 512-bit chunk
    for (size_t i = 0; i < processedInput.length(); i += 64) {
        processChunk(&processedInput[i], h0, h1, h2, h3, h4, h5, h6, h7, k);
    }

    // Final hash value
    stringstream ss;
    ss << hex << setw(8) << setfill('0') << h0
       << setw(8) << setfill('0') << h1
       << setw(8) << setfill('0') << h2
       << setw(8) << setfill('0') << h3
       << setw(8) << setfill('0') << h4
       << setw(8) << setfill('0') << h5
       << setw(8) << setfill('0') << h6
       << setw(8) << setfill('0') << h7;

    return ss.str();
}

int main()
{
//    string textToWrite = "this message is going to be hashed using SHA256";
//
//    string fileName = "originalMessage.txt";
//    writeTextToFile(textToWrite, fileName);
//
//    string textToHash = readTextFromFile(fileName);
//
//    cout << textToHash << endl;
//
//    cout << (textToHash == textToWrite) << endl;
//
//    cout << preprocess("FMI") << endl;

    cout << preprocess("FMAJSDJADJADJASDJASDJASJDASFJASFJASFJASJFAJSFJASGJDFGJSDKFGSJDFKGHJSDFGHSFGHDFI") << endl;

    return 0;
}
