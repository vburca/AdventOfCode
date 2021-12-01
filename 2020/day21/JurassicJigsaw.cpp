#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

void part1(istream& inputFile)
{
    return;
    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);
    }
}

void part2(istream& inputFile)
{
    return;
    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);
    }
}


int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        cout << "Must specify input file!" << endl;
        return 0;
    }

    cout << "Input file name: " << argv[1] << endl;
    ifstream inputFile(argv[1]);

    cout << "Part 1: " << endl;
    part1(inputFile);

    inputFile.clear();
    inputFile.seekg(0, ios::beg);

    cout << "Part 2: " << endl;
    part2(inputFile);

    inputFile.close();

    return 0;
}