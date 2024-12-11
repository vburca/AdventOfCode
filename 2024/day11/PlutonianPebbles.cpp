#include "../../util/BasicIncludes.h"
#include "../../util/StringUtil.h"
#include "../../util/MathUtil.h"

#include <vector>
#include <optional>
#include <cmath>
#include <map>

using namespace std;

struct Stone {
    uint64_t value;
};

void _printStones(const vector<Stone> &stones) {
    for (const auto &stone : stones) {
        cout << stone.value << " ";
    }
    cout << endl;
}

void _printStones(const map<uint64_t, size_t> &stones) {
    for (const auto &p : stones) {
        cout << "{" << p.first << ": " << p.second << "} ";
    }
    cout << endl;
}

const vector<Stone> _blinkStone(const Stone &stone) {
    if (stone.value == 0) {
        return {{.value = 1}};
    }

    const auto numDigits = aoc::util::math::getNumberOfDigits(stone.value);
    if (numDigits % 2 == 0) {
        const uint64_t factor = pow(10, numDigits / 2);
        const auto leftStone = Stone{.value = stone.value / factor};
        const auto rightStone = Stone{.value = stone.value % factor};

        return {leftStone, rightStone};
    }

    return {{.value = stone.value * 2024ull}};
}

const vector<Stone> _processStonesNaive(const vector<Stone> &initialStones, const size_t numBlinks) {
    vector<Stone> blinkedStones = initialStones;

    for (size_t i = 0; i < numBlinks; ++i) {
        vector<Stone> newBlinkedStones;
        for (const auto &stone : blinkedStones) {
            const auto blinkedStone = _blinkStone(stone);
            newBlinkedStones.insert(newBlinkedStones.end(), blinkedStone.begin(), blinkedStone.end());
        }

        blinkedStones = newBlinkedStones;
    }

    return blinkedStones;
}

void _insertStoneWithCount(const Stone &stone, map<uint64_t, size_t> &stoneCounts, const size_t count = 1) {
    if (!stoneCounts.contains(stone.value)) {
        stoneCounts[stone.value] = count;
    } else {
        stoneCounts[stone.value] += count;
    }
}

const map<uint64_t, uint64_t> _processStones(const vector<Stone> &initialStones, const size_t numBlinks) {
    map<uint64_t, uint64_t> stoneCounts;
    for (const auto &stone : initialStones) {
        _insertStoneWithCount(stone, stoneCounts);
    }

    // _printStones(stoneCounts);

    for (size_t i = 0; i < numBlinks; ++i) {
        map<uint64_t, uint64_t> updatedStoneCounts;
        for (const auto &stoneCount : stoneCounts) {
            const auto stoneValue = stoneCount.first;
            const uint64_t count = stoneCount.second;

            const auto blinkedStones = _blinkStone({.value = stoneValue});
            for (const auto &stone : blinkedStones) {
                _insertStoneWithCount(stone, updatedStoneCounts, count);
            }
        }
        stoneCounts = updatedStoneCounts;
        // cout << "Blink: " << i + 1 << endl;
        // _printStones(stoneCounts);
    }

    return stoneCounts;
}

void part1(istream& inputFile) {
    vector<Stone> stones;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        uint64_t stoneValue;
        istringstream iss(line);
        while (iss >> stoneValue) {
            stones.push_back({.value = stoneValue});
        }
    }

    // _printStones(stones);

    const auto stoneCounts = _processStones(stones, 25);
    const auto totalCount = std::accumulate(
        std::begin(stoneCounts),
        std::end(stoneCounts),
        0,
        [](const size_t prev, const auto &stoneCount) {
            return prev + stoneCount.second;
        });

    cout << totalCount << endl;
}

void part2(istream& inputFile) {
    vector<Stone> stones;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        uint64_t stoneValue;
        istringstream iss(line);
        while (iss >> stoneValue) {
            stones.push_back({.value = stoneValue});
        }
    }

    // _printStones(stones);

    const auto stoneCounts = _processStones(stones, 75);
    const auto totalCount = std::accumulate(
        std::begin(stoneCounts),
        std::end(stoneCounts),
        0ull,
        [](const uint64_t prev, const auto &stoneCount) {
            return prev + stoneCount.second;
        });

    cout << totalCount << endl;
}

int main(int argc, char **argv) {
    if (argc <= 1)
    {
        std::cout << "Must specify input file!" << endl;
        return 0;
    }

    std::cout << "Input file name: " << argv[1] << endl;
    ifstream inputFile(argv[1]);

    std::cout << "Part 1: " << endl;
    part1(inputFile);

    inputFile.clear();
    inputFile.seekg(0, ios::beg);

    std::cout << "Part 2: " << endl;
    part2(inputFile);

    inputFile.close();

    return 0;
}