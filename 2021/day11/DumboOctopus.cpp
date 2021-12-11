#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <unordered_set>
#include <utility>

using namespace std;

#define CAVE_SIZE_RUN 10
#define CAVE_SIZE_TEST 5
#define CAVE_SIZE CAVE_SIZE_RUN

#define NUM_ITER_RUN 100
#define NUM_ITER_TEST 2
#define NUM_ITER NUM_ITER_RUN

#define MAX_ENERGY_LEVEL 9

typedef pair<uint8_t, uint8_t> position_t;

struct position_t_equal
{
    bool operator()(const position_t& p1, const position_t& p2) const
    {
        return (p1.first == p2.first) && (p1.second == p2.second);
    }
};

struct position_t_hash
{
    size_t operator()(const position_t& p) const
    {
        return p.first ^ p.second;
    }
};

void printMap(const vector<vector<uint8_t>>& caveMap)
{
    for (const auto& line : caveMap)
    {
        for (const auto& octopusValue : line)
        {
            cout << (size_t)octopusValue;
        }
        cout << endl;
    }
    cout << endl;
}

vector<position_t> getNeighbors(const position_t current, const size_t kCaveSize)
{
    const vector<position_t> possibleNeighbors = {
        {current.first - 1, current.second - 1}, {current.first - 1, current.second}, {current.first - 1, current.second + 1},
        {current.first, current.second - 1},          /* current element, */          {current.first, current.second + 1},
        {current.first + 1, current.second - 1}, {current.first + 1, current.second}, {current.first + 1, current.second + 1}
    };

    vector<position_t> validNeighbors;
    for (const auto& neighbor : possibleNeighbors)
    {
        // check if the neighbor is outside the cave boundaries
        if (neighbor.first < 0 || neighbor.first >= kCaveSize) continue;
        if (neighbor.second < 0 || neighbor.second >= kCaveSize) continue;

        // not outside, so it is a valid neighbor
        validNeighbors.push_back(neighbor);
    }

    return validNeighbors;
}

size_t iterationIllumination(vector<vector<uint8_t>>& caveMap)
{
    const auto kCaveSize = caveMap.size();

    deque<position_t> readyToFlash;
    unordered_set<position_t, position_t_hash, position_t_equal> flashedThisIteration;

    // increase energy level of all octopuses and see which one is ready to flash
    for (size_t i = 0; i < kCaveSize; ++i)
    {
        for (size_t j = 0; j < kCaveSize; ++j)
        {
            auto& octopusValue = caveMap[i][j];
            octopusValue += 1;

            if (octopusValue > MAX_ENERGY_LEVEL)
            {
                // this octopus is ready to flash this iteration
                const auto pos = position_t{i, j};
                readyToFlash.push_back(pos);
                flashedThisIteration.insert(pos);
            }
        }
    }

    // run the flashes until no more octopus can flash
    while (!readyToFlash.empty())
    {
        // grab an octopus ready to flash
        const auto pos = readyToFlash.front();
        readyToFlash.pop_front();

        const auto validNeighbors = getNeighbors(pos, kCaveSize);
        for (const auto& neighbor : validNeighbors)
        {
            // check if this octopus is already flashing in this iteration
            if (flashedThisIteration.find(neighbor) != flashedThisIteration.end())
            {
                // this octopus already flashed (or is about to flash) this iteration
                continue;
            }

            // increase its energy
            auto& octopusValue = caveMap[neighbor.first][neighbor.second];
            octopusValue += 1;

            if (octopusValue > MAX_ENERGY_LEVEL)
            {
                // this octopus is ready to flash this iteration
                readyToFlash.push_back(neighbor);
                flashedThisIteration.insert(neighbor);
            }
        }
    }

    // set the energy value of each octopus that flashed during this iteration to 0
    for (auto it = flashedThisIteration.begin(); it != flashedThisIteration.end(); ++it)
    {
        const auto pos = *it;
        caveMap[pos.first][pos.second] = 0;
    }

    // cout << "After step " << iteration << ":" << endl;
    // printMap(caveMap);

    return flashedThisIteration.size();
}

size_t runCaveIllumination(vector<vector<uint8_t>>& caveMap, const size_t kMaxIterations, bool simultaneousFlash = false)
{
    const auto numTotalOctopuses = caveMap.size() * caveMap.size();
    size_t numFlashes = 0;
    size_t iteration = 0;

    // cout << "Before any steps:" << endl;
    // printMap(caveMap);

    while (true)
    {
        const auto numIterationFlashes = iterationIllumination(caveMap);
        numFlashes += numIterationFlashes;

        iteration += 1;

        if (!simultaneousFlash)
        {
            if (iteration >= kMaxIterations) return numFlashes;
        }
        else
        {
            // check if all octopuses flashed simultaneously this iteration
            if (numIterationFlashes == numTotalOctopuses) return iteration;
        }
    }

    return 0;
}

void part1(istream& inputFile)
{
    vector<vector<uint8_t>> caveMap(CAVE_SIZE, vector<uint8_t>(CAVE_SIZE, 0));

    size_t row = 0;
    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        for (size_t col = 0; col < CAVE_SIZE; ++col)
        {
            caveMap[row][col] = line[col] - '0';
        }

        row += 1;
    }

    const auto numFlashes = runCaveIllumination(caveMap, NUM_ITER);
    cout << numFlashes << endl;
}

void part2(istream& inputFile)
{
    vector<vector<uint8_t>> caveMap(CAVE_SIZE, vector<uint8_t>(CAVE_SIZE, 0));

    size_t row = 0;
    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        for (size_t col = 0; col < CAVE_SIZE; ++col)
        {
            caveMap[row][col] = line[col] - '0';
        }

        row += 1;
    }

    const auto simultaneousFlashIteration = runCaveIllumination(caveMap, 0, true);
    cout << simultaneousFlashIteration << endl;
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