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
vector<unsigned long> convert_to_binary(const string);

// Pads the messages to make sure they are a multiple of 512 bits.
vector<unsigned long> pad_to_512bits(const vector<unsigned long>);

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

vector<unsigned long> convert_to_binary(const string input)
{
    vector<unsigned long> block;

    for (int i = 0; i < input.size(); i++)
    {
        bitset<8> b(input.c_str()[i]);
//        cout << b << endl;
//        cout << b.to_ulong() << endl;
        block.push_back(b.to_ulong());
    }

    return block;
}

int main()
{
    vector<unsigned long> binary = convert_to_binary("abc");

    for (int i = 0; i < binary.size(); i++)
    {
        cout << binary[i] << '\t';
    }

    return 0;
}

