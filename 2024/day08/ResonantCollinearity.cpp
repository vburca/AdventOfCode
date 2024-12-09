#include "../../util/BasicIncludes.h"
#include "../../util/StringUtil.h"
#include "../../util/Types.h"

#include <vector>
#include <map>
#include <set>
#include <optional>

using namespace std;
using namespace aoc::util::types;

bool _isValidFrequencyAntenna(const char ch) {
    return (ch >= 'a' && ch <= 'z') ||
            (ch >= 'A' && ch <= 'Z') ||
            (ch >= '0' && ch <= '9');
}

coord_signed_t _calculateDistanceVector(const coord_t &a, const coord_t &b) {
    const coord_signed_t signedA = (coord_signed_t)a;
    const coord_signed_t signedB = (coord_signed_t)b;
    return { signedA.first - signedB.first,
            signedA.second - signedB.second };
}

bool _isWithinBounds(const coord_signed_t &p, const size_t mapH, const size_t mapW) {
    return p.first >= 0 && p.first < mapH &&
            p.second >= 0 && p.second < mapW;
}

optional<coord_t> _generateValidAntinode(const coord_t &p, const coord_signed_t &distanceVector, const size_t mapH, const size_t mapW) {
    coord_signed_t antinode;
    const coord_signed_t signedP = (coord_signed_t)p;

    antinode = {signedP.first + distanceVector.first, signedP.second + distanceVector.second};
    if (!_isWithinBounds(antinode, mapH, mapW)) {
            return nullopt;
    }

    return antinode;
}

set<coord_t> _resonateAntinode(const coord_t &srcAntinode, const coord_signed_t &distanceVector, const size_t mapH, const size_t mapW) {
    set<coord_t> resonateAntinodes;
    coord_signed_t currentSrcAntinode = srcAntinode;
    while (_isWithinBounds(currentSrcAntinode, mapH, mapW)) {
        // we will end up inserting the very first source antinode as well, but who cares, we collect all antinodes in a set anyway
        resonateAntinodes.insert(currentSrcAntinode);

        currentSrcAntinode = {currentSrcAntinode.first + distanceVector.first, currentSrcAntinode.second + distanceVector.second};
    }

    return resonateAntinodes;
}

set<coord_t> _generateValidAntinodes(const coord_t &pointA, const coord_t &pointB, const size_t mapH, const size_t mapW, bool isPart2 = false) {
    const coord_signed_t distanceVectorA = _calculateDistanceVector(pointA, pointB);
    const coord_signed_t distanceVectorB = _calculateDistanceVector(pointB, pointA);

    set<coord_t> antinodes;
    const optional<coord_t> antinodeAOpt = _generateValidAntinode(pointA, distanceVectorA, mapH, mapW);
    if (antinodeAOpt) {
        antinodes.insert(*antinodeAOpt);
        if (isPart2) {
            const auto resonatedAntinodes = _resonateAntinode(*antinodeAOpt, distanceVectorA, mapH, mapW);
            antinodes.insert(resonatedAntinodes.begin(), resonatedAntinodes.end());
        }
    }

    const auto antinodeBOpt = _generateValidAntinode(pointB, distanceVectorB, mapH, mapW);
    if (antinodeBOpt) {
        antinodes.insert(*antinodeBOpt);
        if (isPart2) {
            const auto resonatedAntinodes = _resonateAntinode(*antinodeBOpt, distanceVectorB, mapH, mapW);
            antinodes.insert(resonatedAntinodes.begin(), resonatedAntinodes.end());
        }
    }

    return antinodes;
}

void _printAntinodes(const set<coord_t> &antinodes, const vector<string> &frequencyMap) {
    auto frequencyMapWithAntinodes = frequencyMap;
    for (const auto &antinode : antinodes) {
        if (frequencyMapWithAntinodes[antinode.first][antinode.second] != '.') {
            continue;
        }

        frequencyMapWithAntinodes[antinode.first][antinode.second] = '#';
    }

    for (const auto &row : frequencyMapWithAntinodes) {
        cout << row << endl;
    }
}

void part1(istream& inputFile) {
    vector<string> frequencyMap;
    map<char, vector<coord_t>> frequencyCoords;

    size_t row = 0;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        frequencyMap.push_back(line);

        for (size_t col = 0; col < line.size(); ++col) {
            const char ch = line[col];
            if (_isValidFrequencyAntenna(ch)) {
                frequencyCoords[ch].push_back({row, col});
            }
        }

        row += 1;
    }

    // for (const auto &p : frequencyCoords) {
    //     cout << p.first << ": ";
    //     for (const auto &pos : p.second) {
    //         cout << "{" << pos.first << ", " << pos.second << "} ";
    //     }
    //     cout << endl;
    // }

    const size_t mapHeight = frequencyMap.size();
    const size_t mapWidth = frequencyMap.front().size();

    set<coord_t> antinodePositions;
    for (const auto &frequencyData : frequencyCoords) {
        const auto &frequencyAntenna = frequencyData.first;
        const auto &positions = frequencyData.second;
        for (size_t i = 0; i < positions.size() - 1; ++i) {
            const auto &antennaA = positions[i];
            for (size_t j = i + 1; j < positions.size(); ++j) {
                const auto &antennaB = positions[j];
                const auto antinodes = _generateValidAntinodes(antennaA, antennaB, mapHeight,  mapWidth);
                antinodePositions.insert(antinodes.begin(), antinodes.end());
            }
        }
    }

    // _printAntinodes(antinodePositions, frequencyMap);
    // cout << "antinode positions found:" << endl;
    // for (const auto &antinode : antinodePositions) {
    //     cout << "{" << antinode.first << ", " << antinode.second << "}" << endl;
    // }

    cout << antinodePositions.size() << endl;
}

void part2(istream& inputFile) {
    vector<string> frequencyMap;
    map<char, vector<coord_t>> frequencyCoords;

    size_t row = 0;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        frequencyMap.push_back(line);

        for (size_t col = 0; col < line.size(); ++col) {
            const char ch = line[col];
            if (_isValidFrequencyAntenna(ch)) {
                frequencyCoords[ch].push_back({row, col});
            }
        }

        row += 1;
    }

    // for (const auto &p : frequencyCoords) {
    //     cout << p.first << ": ";
    //     for (const auto &pos : p.second) {
    //         cout << "{" << pos.first << ", " << pos.second << "} ";
    //     }
    //     cout << endl;
    // }

    const size_t mapHeight = frequencyMap.size();
    const size_t mapWidth = frequencyMap.front().size();

    set<coord_t> antinodePositions;
    for (const auto &frequencyData : frequencyCoords) {
        const auto &frequencyAntenna = frequencyData.first;
        const auto &positions = frequencyData.second;
        for (size_t i = 0; i < positions.size() - 1; ++i) {
            const auto &antennaA = positions[i];
            // add the 2 antennas to the antinodes since they will each other generate an antinode at each other's location
            antinodePositions.insert(antennaA);
            for (size_t j = i + 1; j < positions.size(); ++j) {
                const auto &antennaB = positions[j];
                const auto antinodes = _generateValidAntinodes(antennaA, antennaB, mapHeight,  mapWidth, true);
                antinodePositions.insert(antinodes.begin(), antinodes.end());
                // add the 2 antennas to the antinodes since they will each other generate an antinode at each other's location
                antinodePositions.insert(antennaB);
            }
        }
    }

    // _printAntinodes(antinodePositions, frequencyMap);
    // cout << "antinode positions found:" << endl;
    // for (const auto &antinode : antinodePositions) {
    //     cout << "{" << antinode.first << ", " << antinode.second << "}" << endl;
    // }

    cout << antinodePositions.size() << endl;
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