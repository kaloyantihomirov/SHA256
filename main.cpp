#include <iostream>
#include <fstream>
#include <string>

using namespace std;

string readTextFromFile(ifstream & fStream, const string & fileName)
{
    fStream.open(fileName);

    string result;
    string line;

    if (fStream.is_open())
    {
        while (fStream.good()) {
            getline(fStream, line);

            if (fStream.eof())
            {
                result += line;
            }
            else
            {
                result += line + "\n";
            }
        }
    }

    fStream.close();

    return result;
}

int main()
{
    ifstream  ifstream1;
    string shoppingList = readTextFromFile(ifstream1, "shoppingList.txt");

    cout << shoppingList << endl;
}

