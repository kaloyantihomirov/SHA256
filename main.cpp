#include <iostream>
#include <fstream>
#include <string>

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

int main()
{
    string shoppingList = readTextFromFile("shoppingList.txt");

    cout << shoppingList << endl;

    writeTextToFile("Writing from main()", "example.txt");


    return 0;
}
