#include <iostream>
#include <fstream>
#include <regex>
#include <sstream>

using namespace std;

const int32_t VAL = 2020;

void part1(istream& inputFile)
{
    size_t validPwds = 0;
    regex rx("([0-9]+)-([0-9]+) ([a-z]): ([a-z]+)");

    while (!inputFile.eof())
    {
        // read one line from the file
        string line;
        getline(inputFile, line);

        smatch match;
        if (regex_match(line, match, rx))
        {
            stringstream minStr(match[1].str());
            size_t min;
            minStr >> min;

            stringstream maxStr(match[2].str());
            size_t max;
            maxStr >> max;

            char ch = match[3].str()[0];

            string pwd = match[4].str();

            size_t occurs = 0;
            for (auto const &c : pwd)
            {
                occurs += ch == c ? 1 : 0;
            }

            validPwds += (min <= occurs && occurs <= max) ? 1 : 0;
        }
    }

    cout << validPwds << endl;
}

void part2(istream& inputFile)
{
    size_t validPwds = 0;
    regex rx("([0-9]+)-([0-9]+) ([a-z]): ([a-z]+)");

    while (!inputFile.eof())
    {
        // read one line from the file
        string line;
        getline(inputFile, line);

        smatch match;
        if (regex_match(line, match, rx))
        {
            stringstream idx1Str(match[1].str());
            size_t idx1;
            idx1Str >> idx1;

            stringstream idx2Str(match[2].str());
            size_t idx2;
            idx2Str >> idx2;

            char ch = match[3].str()[0];

            string pwd = match[4].str();

            auto condition = ((pwd[idx1 - 1] == ch) != (pwd[idx2 - 1] == ch));

            validPwds += condition ? 1 : 0;
        }
    }

    cout << validPwds << endl;
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