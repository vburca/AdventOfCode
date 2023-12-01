#include <iostream>
#include <fstream>
#include <cmath>
#include <unordered_set>
#include <algorithm>
#include <sstream>
#include <string>
#include <cstdint>

using namespace std;

struct Coordinate {
    int32_t x;
    int32_t y;
    uint32_t steps = 0;

    Coordinate() : x(0), y(0) {};
    Coordinate(const int32_t x, const int32_t y) : x(x), y(y) {};
};

auto coordinateHash = [](const Coordinate& c)
{
    return (size_t)((uint64_t)c.x) << 32 | (uint64_t)c.y;
};

auto coordinateEqual = [](const Coordinate& c1, const Coordinate& c2)
{
    return ((c1.x == c2.x) && (c1.y == c2.y));
};

using MapPositions = unordered_set<Coordinate, decltype(coordinateHash), decltype(coordinateEqual)>;

uint32_t getDistance(Coordinate coord)
{
    return abs(coord.x) + abs(coord.y);
}

void part1(istream &inputFile)
{
    uint32_t minDistance = numeric_limits<uint32_t>::max();

    // first wire
    MapPositions wire1Positions(100, coordinateHash, coordinateEqual);
    Coordinate wire1Position(0, 0);

    string line;
    getline(inputFile, line);
    stringstream ss(line);

    string move;
    while (getline(ss, move, ','))
    {
        char direction = move[0];
        uint32_t distance = stoi(move.substr(1));

        Coordinate step;
        switch (direction)
        {
            case 'U': step = Coordinate(0, 1);   break;
            case 'R': step = Coordinate(1, 0);   break;
            case 'D': step = Coordinate(0, -1);  break;
            case 'L': step = Coordinate(-1, 0);  break;
            default: break;
        }

        for (size_t i = 0; i < distance; ++i)
        {
            wire1Position.x += step.x;
            wire1Position.y += step.y;

            wire1Positions.insert(wire1Position);
        }
    }

    // second wire
    Coordinate wire2Position(0, 0);

    getline(inputFile, line);
    ss.clear();
    ss.str(line);

    while (getline(ss, move, ','))
    {
        char direction = move[0];
        uint32_t distance = stoi(move.substr(1));

        Coordinate step;
        switch (direction)
        {
            case 'U': step = Coordinate(0, 1);   break;
            case 'R': step = Coordinate(1, 0);   break;
            case 'D': step = Coordinate(0, -1);  break;
            case 'L': step = Coordinate(-1, 0);  break;
            default: break;
        }

        for (size_t i = 0; i < distance; ++i)
        {
            wire2Position.x += step.x;
            wire2Position.y += step.y;

            if (wire1Positions.find(wire2Position) != wire1Positions.end())
            {
                minDistance = min(minDistance, getDistance(wire2Position));
            }
        }
    }

    cout << minDistance << endl;
}

void part2(istream &inputFile)
{
    uint32_t minSteps = numeric_limits<uint32_t>::max();

    // first wire
    MapPositions wire1Positions(100, coordinateHash, coordinateEqual);
    Coordinate wire1Position(0, 0);

    string line;
    getline(inputFile, line);
    stringstream ss(line);

    string move;
    while (getline(ss, move, ','))
    {
        char direction = move[0];
        uint32_t distance = stoi(move.substr(1));

        Coordinate step;
        switch (direction)
        {
            case 'U': step = Coordinate(0, 1);   break;
            case 'R': step = Coordinate(1, 0);   break;
            case 'D': step = Coordinate(0, -1);  break;
            case 'L': step = Coordinate(-1, 0);  break;
            default: break;
        }

        for (size_t i = 0; i < distance; ++i)
        {
            wire1Position.x += step.x;
            wire1Position.y += step.y;
            wire1Position.steps += 1;

            wire1Positions.insert(wire1Position);
        }
    }

    // second wire
    Coordinate wire2Position(0, 0);

    getline(inputFile, line);
    ss.clear();
    ss.str(line);

    while (getline(ss, move, ','))
    {
        char direction = move[0];
        uint32_t distance = stoi(move.substr(1));

        Coordinate step;
        switch (direction)
        {
            case 'U': step = Coordinate(0, 1);   break;
            case 'R': step = Coordinate(1, 0);   break;
            case 'D': step = Coordinate(0, -1);  break;
            case 'L': step = Coordinate(-1, 0);  break;
            default: break;
        }

        for (size_t i = 0; i < distance; ++i)
        {
            wire2Position.x += step.x;
            wire2Position.y += step.y;
            wire2Position.steps += 1;

            auto foundPosition = wire1Positions.find(wire2Position);
            if (foundPosition != wire1Positions.end())
            {
                auto wire1Steps = foundPosition->steps;
                auto wire2Steps = wire2Position.steps;

                minSteps = min(minSteps, wire1Steps + wire2Steps);
            }
        }
    }

    cout << minSteps << endl;
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