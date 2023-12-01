#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>
#include <map>
#include <utility>
#include <algorithm>
#include <limits>
#include <set>
#include <cstdint>

using namespace std;

#define CYCLES_THRESHOLD 6

typedef tuple<int64_t, int64_t, int64_t> coords3d_t;
typedef tuple<int64_t, int64_t, int64_t, int64_t> coords4d_t;

string getCube3dKey(const int64_t x, const int64_t y, const int64_t z)
{
    ostringstream ss;
    ss << x << ":" << y << ":" << z;
    return ss.str();
}

string getCube3dKey(const coords3d_t& position)
{
    const auto [x, y, z] = position;
    return getCube3dKey(x, y, z);
}

struct Cube3D
{
    Cube3D() { }

    Cube3D(const int64_t x, const int64_t y, const int64_t z)
    {
        position = make_tuple(x, y, z);
    }

    Cube3D(const coords3d_t& pos) : position(pos) { }

    coords3d_t position { 0, 0, 0 };
    size_t activeNeighbors = 0;

    string getKey() { return getCube3dKey(position); }

    bool canBecomeActive()
    {
        if (activeNeighbors == 3)
        {
            return true;
        }

        return false;
    }

    bool canBecomeInactive()
    {
        if (activeNeighbors != 2 &&
            activeNeighbors != 3)
        {
            return true;
        }

        return false;
    }
};

string getCube4dKey(const int64_t x, const int64_t y, const int64_t z, const int64_t w)
{
    ostringstream ss;
    ss << x << ":" << y << ":" << z << ":" << w;
    return ss.str();
}

string getCube4dKey(const coords4d_t& position)
{
    const auto [x, y, z, w] = position;
    return getCube4dKey(x, y, z, w);
}

struct Cube4D
{
    Cube4D() { }

    Cube4D(const int64_t x, const int64_t y, const int64_t z, const int64_t w)
    {
        position = make_tuple(x, y, z, w);
    }

    Cube4D(const coords4d_t& pos) : position(pos) { }

    coords4d_t position { 0, 0, 0, 0 };
    size_t activeNeighbors = 0;

    string getKey() { return getCube4dKey(position); }

    bool canBecomeActive()
    {
        if (activeNeighbors == 3)
        {
            return true;
        }

        return false;
    }

    bool canBecomeInactive()
    {
        if (activeNeighbors != 2 &&
            activeNeighbors != 3)
        {
            return true;
        }

        return false;
    }
};

void printActiveCubes(map<string, Cube3D> activeCubes)
{
    map<int64_t, vector<coords3d_t>> zToXY;
    vector<int64_t> zs;

    for (const auto& [key, cube] : activeCubes)
    {
        const auto& [x, y, z] = cube.position;

        if (zToXY.find(z) == zToXY.end())
        {
            zs.emplace_back(z);
        }

        zToXY[z].push_back(cube.position);
    }

    sort(
        zs.begin(),
        zs.end());

    for (const auto& z : zs)
    {
        cout << "z = " << z << endl;

        const auto coords = zToXY[z];

        set<pair<int64_t, int64_t>> coords2d;
        int64_t x_min = numeric_limits<int64_t>::max();
        int64_t y_min = numeric_limits<int64_t>::max();
        int64_t x_max = numeric_limits<int64_t>::min();
        int64_t y_max = numeric_limits<int64_t>::min();
        for (const auto& [x, y, _] : coords)
        {
            if (x > x_max)
            {
                x_max = x;
            }

            if (x < x_min)
            {
                x_min = x;
            }

            if (y > y_max)
            {
                y_max = y;
            }
            if (y < y_min)
            {
                y_min = y;
            }

            coords2d.emplace(x, y);
        }

        for (int64_t i = x_min; i <= x_max; ++i)
        {
            for (int64_t j = y_min; j <= y_max; ++j)
            {
                if (coords2d.find(pair(i, j)) != coords2d.end())
                {
                    cout << "# ";
                }
                else
                {
                    cout << ". ";
                }
            }
            cout << endl;
        }
        cout << endl;
    }
}

void part1(istream& inputFile)
{
    map<string, Cube3D> activeCubes;

    int64_t lineId = 0;
    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        for (int64_t y = 0; y < line.size(); ++y)
        {
            // grab all the initial active cubes
            if (line[y] == '#')
            {
                Cube3D c(lineId, y, 0);
                activeCubes.emplace(c.getKey(), c);
            }
        }

        lineId += 1;
    }

    uint64_t cycle = 1;
    while (cycle <= CYCLES_THRESHOLD)
    {
        // temporary map of "touched" neighbors (by active cubes)
        map<string, Cube3D> inactiveCubes;

        // go through all the active cuves
        for (auto& [cubeKey, cube] : activeCubes)
        {
            // traverse all of its neighbors
            const auto& [x, y, z] = cube.position;

            // set its active neighbors to 0, because we will count them again
            cube.activeNeighbors = 0;

            // update the active neighbor count for itself and its inactive neighbors
            for (short int dx = -1; dx <= 1; ++dx)
            {
                for (short int dy = -1; dy <= 1; ++dy)
                {
                    for (short int dz = -1; dz <= 1; ++dz)
                    {
                        // do not count itself as neighbor
                        if (dx == 0 && dy == 0 && dz == 0)
                        {
                            continue;
                        }

                        const coords3d_t neighborPosition(x + dx, y + dy, z + dz);
                        const auto neighborKey = getCube3dKey(neighborPosition);

                        // for this neighbor, check if it is active or not
                        if (activeCubes.find(neighborKey) != activeCubes.end())
                        {
                            // if it is an active cube, count it as an active neighbor
                            cube.activeNeighbors += 1;
                        }
                        else if (inactiveCubes.find(neighborKey) != inactiveCubes.end())
                        {
                            // if it is an inactive cube, _previously_ "touched" by some other active cube, update
                            // its active neighbors count
                            inactiveCubes[neighborKey].activeNeighbors += 1;
                        }
                        else
                        {
                            // if it is a new inactive cube, add it to the map of "touched" inactive neighbors
                            Cube3D neighbor(neighborPosition);
                            neighbor.activeNeighbors += 1;

                            // add it to the temporary map
                            inactiveCubes.emplace(neighborKey, neighbor);
                        }
                    }
                }
            }
        }

        // remove active cubes that do not satisfy the rules
        for (auto it = activeCubes.begin(); it != activeCubes.end();)
        {
            if (it->second.canBecomeInactive())
            {
                activeCubes.erase(it++);
            }
            else
            {
                // reset its active neighbor count
                it->second.activeNeighbors = 0;
                ++it;
            }
        }

        // "promote" inactive cubes that satisfy the rules to being active
        for (auto& [neighborKey, neighbor] : inactiveCubes)
        {
            if (neighbor.canBecomeActive())
            {
                // reset active neighbors
                neighbor.activeNeighbors = 0;

                if (activeCubes.find(neighborKey) != activeCubes.end())
                {
                    cout << "INSERTING DUPLICATE CUBE! " << neighborKey << endl;
                }

                // add to active cubes map
                activeCubes.emplace(neighborKey, neighbor);
            }
        }

        cycle += 1;
    }

    cout << activeCubes.size() << endl;
}

void part2(istream& inputFile)
{
    map<string, Cube4D> activeCubes;

    int64_t lineId = 0;
    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        for (int64_t y = 0; y < line.size(); ++y)
        {
            // grab all the initial active cubes
            if (line[y] == '#')
            {
                Cube4D c(lineId, y, 0, 0);
                activeCubes.emplace(c.getKey(), c);
            }
        }

        lineId += 1;
    }

    uint64_t cycle = 1;
    while (cycle <= CYCLES_THRESHOLD)
    {
        // temporary map of "touched" neighbors (by active cubes)
        map<string, Cube4D> inactiveCubes;

        // go through all the active cuves
        for (auto& [cubeKey, cube] : activeCubes)
        {
            // traverse all of its neighbors
            const auto& [x, y, z, w] = cube.position;

            // set its active neighbors to 0, because we will count them again
            cube.activeNeighbors = 0;

            // update the active neighbor count for itself and its inactive neighbors
            for (short int dx = -1; dx <= 1; ++dx)
            {
                for (short int dy = -1; dy <= 1; ++dy)
                {
                    for (short int dz = -1; dz <= 1; ++dz)
                    {
                        for (short int dw = -1; dw <= 1; ++dw)
                        {
                            // do not count itself as neighbor
                            if (dx == 0 && dy == 0 && dz == 0 && dw == 0)
                            {
                                continue;
                            }

                            const coords4d_t neighborPosition(x + dx, y + dy, z + dz, w + dw);
                            const auto neighborKey = getCube4dKey(neighborPosition);

                            // for this neighbor, check if it is active or not
                            if (activeCubes.find(neighborKey) != activeCubes.end())
                            {
                                // if it is an active cube, count it as an active neighbor
                                cube.activeNeighbors += 1;
                            }
                            else if (inactiveCubes.find(neighborKey) != inactiveCubes.end())
                            {
                                // if it is an inactive cube, _previously_ "touched" by some other active cube, update
                                // its active neighbors count
                                inactiveCubes[neighborKey].activeNeighbors += 1;
                            }
                            else
                            {
                                // if it is a new inactive cube, add it to the map of "touched" inactive neighbors
                                Cube4D neighbor(neighborPosition);
                                neighbor.activeNeighbors += 1;

                                // add it to the temporary map
                                inactiveCubes.emplace(neighborKey, neighbor);
                            }
                        }
                    }
                }
            }
        }

        // remove active cubes that do not satisfy the rules
        for (auto it = activeCubes.begin(); it != activeCubes.end();)
        {
            if (it->second.canBecomeInactive())
            {
                activeCubes.erase(it++);
            }
            else
            {
                // reset its active neighbor count
                it->second.activeNeighbors = 0;
                ++it;
            }
        }

        // "promote" inactive cubes that satisfy the rules to being active
        for (auto& [neighborKey, neighbor] : inactiveCubes)
        {
            if (neighbor.canBecomeActive())
            {
                // reset active neighbors
                neighbor.activeNeighbors = 0;

                if (activeCubes.find(neighborKey) != activeCubes.end())
                {
                    cout << "INSERTING DUPLICATE CUBE! " << neighborKey << endl;
                }

                // add to active cubes map
                activeCubes.emplace(neighborKey, neighbor);
            }
        }

        cycle += 1;
    }

    cout << activeCubes.size() << endl;
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