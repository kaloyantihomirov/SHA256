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

// Converts the ASCII string to its binary representation.
vector<unsigned long> convertToBinary(const string);

// Pads the message to make sure the length is a multiple of 512.
vector<unsigned long> padToMultipleOf512Bits(const vector<unsigned long>);

// Changes the n 8-bit segments, representing every ASCII character to 32-bit words.
vector<unsigned long> resize_block(vector<unsigned long>);

// The actual hash computing.
string compute_hash(const vector<unsigned long>);

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

int main()
{
    string str = "abc";

    vector<unsigned long> binary = convertToBinary(str);

    for (int i = 0; i < binary.size(); i++)
    {
        cout << binary[i];
    }
    cout << endl;

    vector<unsigned long> padded = padToMultipleOf512Bits(binary);

    for (int i = 0; i < padded.size(); i++)
    {
        cout << padded[i];
    }

    return 0;
}
