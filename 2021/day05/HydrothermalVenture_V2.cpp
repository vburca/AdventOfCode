#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <unordered_map>
#include <tuple>

#include "VentMap.h"

using namespace std;

#define BAD_OVERLAP_THRESH 2

void part1(istream& inputFile)
{
    auto ventMap = VentMap(BAD_OVERLAP_THRESH);

    regex rx("([0-9]+),([0-9]+) -> ([0-9]+),([0-9]+)");

    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        smatch match;
        if (regex_match(line, match, rx))
        {
            stringstream x1Str(match[1].str());
            stringstream y1Str(match[2].str());
            stringstream x2Str(match[3].str());
            stringstream y2Str(match[4].str());

            size_t x1, y1, x2, y2;
            x1Str >> x1;
            y1Str >> y1;
            x2Str >> x2;
            y2Str >> y2;

            ventMap.fillPosition(x1, y1, x2, y2);
        }
        else
        {
            cout << "Regex failed for line: [" << line << "]" << endl;
        }

    }

    // ventMap.printMap(overlappedPositions);

    const size_t badOverlaps = ventMap.getOverlapCount();
    cout << badOverlaps << endl;
}

void part2(istream& inputFile)
{
    auto ventMap = VentMap(BAD_OVERLAP_THRESH);

    regex rx("([0-9]+),([0-9]+) -> ([0-9]+),([0-9]+)");

    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        smatch match;
        if (regex_match(line, match, rx))
        {
            stringstream x1Str(match[1].str());
            stringstream y1Str(match[2].str());
            stringstream x2Str(match[3].str());
            stringstream y2Str(match[4].str());

            size_t x1, y1, x2, y2;
            x1Str >> x1;
            y1Str >> y1;
            x2Str >> x2;
            y2Str >> y2;

            ventMap.fillPosition(x1, y1, x2, y2, true);
        }
        else
        {
            cout << "Regex failed for line: [" << line << "]" << endl;
        }
    }

    // ventMap.printMap(overlappedPositions);

    const size_t badOverlaps = ventMap.getOverlapCount();
    cout << badOverlaps << endl;
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