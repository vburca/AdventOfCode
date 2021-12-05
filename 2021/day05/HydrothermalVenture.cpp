#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <unordered_map>
#include <tuple>

using namespace std;

#define BAD_OVERLAP_THRESH 2

typedef tuple<size_t, size_t> position_t;

struct position_t_equal : public binary_function<position_t, position_t, size_t>
{
    bool operator()(const position_t& p1, const position_t& p2) const
    {
        return (get<0>(p1) == get<0>(p2) &&
                get<1>(p1) == get<1>(p1));
    }
};

struct position_t_hash : public unary_function<position_t, size_t>
{
    size_t operator()(const position_t& p) const
    {
        return get<0>(p) ^ get<1>(p);
    }
};

typedef unordered_map<position_t, size_t, position_t_hash, position_t_equal> positions_map_t;

void fillPositions(positions_map_t& map, const size_t x1, const size_t y1, const size_t x2, const size_t y2, bool diagonals = false)
{
    if (!diagonals)
    {
        if (x1 != x2 && y1 != y2)
        {
            return;
        }
    }

    size_t x = x1;
    size_t y = y1;
    const size_t xSign = x2 >= x1 ? 1 : -1;
    const size_t ySign = y2 >= y1 ? 1 : -1;

    bool done = false;
    while (!done)
    {
        position_t pos = make_tuple(y, x); // x is along columns, y is along rows

        const auto it = map.find(pos);
        if (it == map.end())
        {
            map.insert({pos, 0});
        }

        map[pos] += 1;

        if (x == x2 && y == y2)
        {
            done = true;
        }
        else
        {
            if (x != x2)
            {
                x += xSign;
            }

            if (y != y2)
            {
                y += ySign;
            }
        }
    }
}

size_t scanPositions(const positions_map_t& map, const size_t badOverlapThreshold)
{
    size_t badOverlaps = 0;
    for (auto it = map.begin(); it != map.end(); ++it)
    {
        badOverlaps += it->second >= badOverlapThreshold ? 1 : 0;
    }

    return badOverlaps;
}

void printMap(const positions_map_t& map)
{
    if (map.empty())
    {
        return;
    }

    size_t maxX = get<0>(map.begin()->first);
    size_t maxY = get<1>(map.begin()->first);
    for (auto it = map.begin(); it != map.end(); ++it)
    {
        const size_t x = get<0>(it->first);
        const size_t y = get<1>(it->first);

        maxX = x > maxX ? x : maxX;
        maxY = y > maxY ? y : maxY;
    }

    cout << endl;
    for (size_t row = 0; row <= maxX; ++row)
    {
        for (size_t col = 0; col <= maxY; ++col)
        {
            const position_t pos = make_tuple(row, col);
            const auto it = map.find(pos);
            if (it == map.end())
            {
                cout << ".";
            }
            else
            {
                cout << it->second;
            }
        }
        cout << endl;
    }
    cout << endl;
}

void part1(istream& inputFile)
{
    positions_map_t overlappedPositions;

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

            fillPositions(overlappedPositions, x1, y1, x2, y2);
        }
        else
        {
            cout << "Regex failed for line: [" << line << "]" << endl;
        }

    }

    // printMap(overlappedPositions);

    const size_t badOverlaps = scanPositions(overlappedPositions, BAD_OVERLAP_THRESH);
    cout << badOverlaps << endl;
}

void part2(istream& inputFile)
{
    positions_map_t overlappedPositions;

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

            fillPositions(overlappedPositions, x1, y1, x2, y2, true);
        }
        else
        {
            cout << "Regex failed for line: [" << line << "]" << endl;
        }

    }

    // printMap(overlappedPositions);

    const size_t badOverlaps = scanPositions(overlappedPositions, BAD_OVERLAP_THRESH);
    cout << badOverlaps << endl;}

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