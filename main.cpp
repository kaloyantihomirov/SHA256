#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bitset>
#include <iomanip>

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

// Converts the ASCII string to its binary representation.
vector<unsigned long> convertToBinary(const string);

// Pads the message to make sure the length is a multiple of 512.
vector<unsigned long> padToMultipleOf512Bits(const vector<unsigned long>);

// Changes the n 8-bit segments, representing every ASCII character to 32-bit words.
vector<unsigned long> resizeBlock(vector<unsigned long>);

// The actual hash computing.
string computeHash(const vector<unsigned long>);

// Taken from NIST spec.
#define ROTRIGHT(word,bits) (((word) >> (bits)) | ((word) << (32-(bits))))
#define SSIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
#define SSIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))
#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

#define EP0(x) (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22))
#define EP1(x) (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25))

vector<unsigned long> convertToBinary(const string input)
{
    vector<unsigned long> block;

    for (int i = 0; i < input.size(); i++)
    {
        bitset<8> b(input.c_str()[i]);

        block.push_back(b.to_ulong());
    }

    return block;
}

vector<unsigned long> padToMultipleOf512Bits(vector<unsigned long> block)
{
    int l = block.size() * 8;

    // Equation for padding is l + 1 + k = 448 mod 512
    // Simplified to: l + 1 + k = 448
    //		  448 - 1 - l = k
    //		  447 - l = k
    // l = length of message in bits
    // k = how much zero's to add, for the new message to be a multiple of 512.
    int k = 447 - l;

    //note that 0x80 is 128 in decimal which is 10000000 (1 with 7 zeros) in binary
    unsigned long t1 = 0x80;
    block.push_back(t1);

    k = k - 7;

    for (int i = 0; i < k / 8; i++)
        block.push_back(0x00000000);

    bitset<64> strLenInBits(l);

    string strLenInBitsAsString = strLenInBits.to_string();

    bitset<8> firstEightBits(strLenInBitsAsString.substr(0, 8));
    block.push_back(firstEightBits.to_ulong());

    for (int i = 8; i < 63; i = i + 8)
    {
        bitset<8> currEightBits(strLenInBitsAsString.substr(i, 8));
        block.push_back(currEightBits.to_ulong());
    }

    return block;
}

//Create a 64-entry message schedule array w[0..63] of 32-bit words
// (The initial values in w[0..63] don't matter, so many implementations zero them here)
vector<unsigned long> resizeBlock(vector<unsigned long> input)
{
    vector<unsigned long> output(16);

    for (int i = 0; i < 64; i = i + 4)
    {
        bitset<32> bit32Word(0);

        bit32Word = (unsigned long)input[i] << 24;
        bit32Word |= (unsigned long)input[i + 1] << 16;
        bit32Word |= (unsigned long)input[i + 2] << 8;
        bit32Word |= (unsigned long)input[i + 3];

        output[i / 4] = bit32Word.to_ulong();
    }

    return output;
}

string showAsHex(unsigned long input)
{
    bitset<32> bs(input);
    unsigned n = bs.to_ulong();

    stringstream sstream;
    sstream << std::hex << std::setw(8) << std::setfill('0') << n;
    string temp;
    sstream >> temp;

    return temp;
}

string computeHash(const vector<unsigned long> block)
{
    unsigned long k[64] = {
            0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,
            0x923f82a4,0xab1c5ed5,0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,
            0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,0xe49b69c1,0xefbe4786,
            0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
            0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,
            0x06ca6351,0x14292967,0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,
            0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,0xa2bfe8a1,0xa81a664b,
            0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
            0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,
            0x5b9cca4f,0x682e6ff3,0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,
            0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
    };

    unsigned long static H0 = 0x6a09e667;
    unsigned long static H1 = 0xbb67ae85;
    unsigned long static H2 = 0x3c6ef372;
    unsigned long static H3 = 0xa54ff53a;
    unsigned long static H4 = 0x510e527f;
    unsigned long static H5 = 0x9b05688c;
    unsigned long static H6 = 0x1f83d9ab;
    unsigned long static H7 = 0x5be0cd19;

    unsigned long W[64];

    for (int t = 0; t <= 15; t++)
    {
        W[t] = block[t] & 0xFFFFFFFF;
    }

    for (int t = 16; t <= 63; t++)
    {
        W[t] = SSIG1(W[t - 2]) + W[t - 7] + SSIG0(W[t - 15]) + W[t - 16];

        W[t] = W[t] & 0xFFFFFFFF;
    }

    unsigned long temp1;
    unsigned long temp2;
    unsigned long a = H0;
    unsigned long b = H1;
    unsigned long c = H2;
    unsigned long d = H3;
    unsigned long e = H4;
    unsigned long f = H5;
    unsigned long g = H6;
    unsigned long h = H7;

    for (int t = 0; t < 64; t++)
    {
        temp1 = h + EP1(e) + CH(e, f, g) + k[t] + W[t];

        temp2 = EP0(a) + MAJ(a, b, c);

        // Do the working variables operations as per NIST.
        h = g;
        g = f;
        f = e;
        e = (d + temp1) & 0xFFFFFFFF; // Makes sure that we are still using 32 bits.
        d = c;
        c = b;
        b = a;
        a = (temp1 + temp2) & 0xFFFFFFFF; // Makes sure that we are still using 32 bits.
    }

    H0 = (H0 + a) & 0xFFFFFFFF;
    H1 = (H1 + b) & 0xFFFFFFFF;
    H2 = (H2 + c) & 0xFFFFFFFF;
    H3 = (H3 + d) & 0xFFFFFFFF;
    H4 = (H4 + e) & 0xFFFFFFFF;
    H5 = (H5 + f) & 0xFFFFFFFF;
    H6 = (H6 + g) & 0xFFFFFFFF;
    H7 = (H7 + h) & 0xFFFFFFFF;

    return showAsHex(H0) + showAsHex(H1) + showAsHex(H2) +
           showAsHex(H3) + showAsHex(H4) + showAsHex(H5) +
           showAsHex(H6) + showAsHex(H7);
}

int main2()
{
    string message = "";
    cout << "Please enter a message to be hashed: ";
    cin >> message;

    if (message.empty())
    {
        message = "abc";
        cout << "No input string was found, so the default string \"abc\" will be used.";
    }
    else if (message.size() > 55)
    {
        cout << "Your string is over 55 characters long, please use a shorter message!" << endl;
        return 1;
    }

    vector<unsigned long> block;

    block = convertToBinary(message);

    block = padToMultipleOf512Bits(block);

    block = resizeBlock(block);

    string hash = computeHash(block);

    writeTextToFile(hash, "hashed.txt");

    cout << hash << endl;

    return 0;
}

int main3()
{
    cout << "Reading message from a file to hash..." << endl;
    string message = readTextFromFile("messageToHash.txt");

    vector<unsigned long> block;

    block = convertToBinary(message);

    block = padToMultipleOf512Bits(block);

    block = resizeBlock(block);

    string hash = computeHash(block);

    writeTextToFile(hash, "hashed.txt");

    cout << hash << endl;

    return 0;
}

int main()
{
    cout << "Reading hashed message from a file..." << endl;
    string readHash = readTextFromFile("hashed-FMI.txt");

    cout << "Enter message to compare its hash with the hash from the file: ";
    string messageToHash;
    cin >> messageToHash;

    vector<unsigned long> block;

    block = convertToBinary(messageToHash);

    block = padToMultipleOf512Bits(block);

    block = resizeBlock(block);

    string hash = computeHash(block);

    if (readHash == hash)
    {
        cout << "The hash of the given text is the same as the read hash." << endl;
    }
    else
        cout << "Sorry, but the hashes are not the same!" << endl;

    return 0;
}