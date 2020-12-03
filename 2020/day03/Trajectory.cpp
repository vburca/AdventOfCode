#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <utility>

using namespace std;

size_t getTreeCount(const vector<string> map, const pair<uint32_t, uint32_t> slopeAngle)
{
    const char TREE = '#';
    const char OPEN = '.';

    size_t numTrees = 0;

    pair<uint32_t, uint32_t> currentPos(0, 0);
    const size_t n = map.size();
    const size_t m = map.front().length();

    while (currentPos.first < n)
    {
        // update the current position to the next one down the slope
        currentPos.first    = (currentPos.first + slopeAngle.first);
        currentPos.second   = (currentPos.second + slopeAngle.second) % m;

        // check if where we landed
        numTrees += (currentPos.first < n && (map[currentPos.first][currentPos.second] == TREE)) ? 1 : 0;
    }

    return numTrees;
}

void part1(istream& inputFile)
{
    vector<string> map;
    while (!inputFile.eof())
    {
        // read one line from the file
        string line;
        getline(inputFile, line);

        map.push_back(line);
    }

    cout << getTreeCount(map, make_pair(1, 3)) << endl;
}

void part2(istream& inputFile)
{
    vector<string> map;
    while (!inputFile.eof())
    {
        // read one line from the file
        string line;
        getline(inputFile, line);

        map.push_back(line);
    }

    vector<pair<uint32_t, uint32_t>> slopes;
    slopes.push_back(make_pair(1, 1));
    slopes.push_back(make_pair(1, 3));
    slopes.push_back(make_pair(1, 5));
    slopes.push_back(make_pair(1, 7));
    slopes.push_back(make_pair(2, 1));

    size_t treeProduct = 1;

    for (const auto &slope : slopes)
    {
        treeProduct *= getTreeCount(map, slope);
    }

    cout << treeProduct << endl;
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