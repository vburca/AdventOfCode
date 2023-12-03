#include "../../util/StringUtil.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint>
#include <vector>
#include <regex>
#include <utility>
#include <unordered_map>
#include <tuple>
#include <optional>
#include <set>

using namespace std;

struct SchematicCoord {
    size_t x;
    size_t y;

    bool operator==(const SchematicCoord &other) const {
        return (x == other.x) && (y == other.y);
    }
};

namespace std {
template<>
struct hash<SchematicCoord> {
    size_t operator()(const SchematicCoord &sc) const {
        return hash<size_t>()(sc.x) ^ hash<size_t>()(sc.y);
    }
};
}

struct SchematicMappings {
    unordered_map<size_t, vector<SchematicCoord>> partNumberToCoords;
    unordered_map<SchematicCoord, size_t> coordToPartNumber;
    unordered_map<SchematicCoord, char> coordToSymbol;
};

bool isSymbol(const char ch) {
    if (ch == '.') {
        return false;
    }

    if (ch >= '0' && ch <= '9') {
        return false;
    }

    return true;
}

vector<SchematicCoord> getNeighborCoords(const SchematicCoord &coord, const size_t maxX, const size_t maxY) {
    const auto xMinus1 = max((size_t)0, coord.x - 1);
    const auto xPlus1 = min(maxX, coord.x + 1);
    const auto yMinus1 = max((size_t)0, coord.y - 1);
    const auto yPlus1 = min(maxY, coord.y + 1);

    return {
        {.x = xMinus1, .y = yMinus1},   // top left
        {.x = xMinus1, .y = coord.y},   // top
        {.x = xMinus1, .y = yPlus1},     // top right
        {.x = coord.x, .y = yPlus1},    // right
        {.x = xPlus1, .y = yPlus1},     // bottom right
        {.x = xPlus1, .y = coord.y},    // bottom
        {.x = xPlus1, .y = yMinus1},   // bottom left
        {.x = coord.x, .y = yMinus1},   // left
    };
}

vector<optional<size_t>> getNeighborPartNumbers(const vector<SchematicCoord> &neighborCoords, const SchematicMappings &mappings) {
    vector<optional<size_t>> neighborPartNumbers;

    for (const auto &neighborCoord : neighborCoords) {
        if (mappings.coordToPartNumber.find(neighborCoord) == mappings.coordToPartNumber.end()) {
            // no number found at this neighbor coord
            neighborPartNumbers.push_back(nullopt);
            continue;
        }

        neighborPartNumbers.push_back(mappings.coordToPartNumber.at(neighborCoord));
    }

    return neighborPartNumbers;
}

// neighborA | neighborB | neighborC
vector<size_t> _getNonOverlappingNeighborsFrom(optional<size_t> neighborA, optional<size_t> neighborB, optional<size_t> neighborC) {
    vector<size_t> nonOverlappingNeighbors;

    if (!neighborB) {
        if (neighborA) {
            nonOverlappingNeighbors.push_back(*neighborA);
        }

        if (neighborC) {
            nonOverlappingNeighbors.push_back(*neighborC);
        }

        return nonOverlappingNeighbors;
    }

    nonOverlappingNeighbors.push_back(*neighborB);

    return nonOverlappingNeighbors;
}

vector<size_t> getNonOverlappingNeighbors(const vector<optional<size_t>> neighborPartNumbers) {
    vector<size_t> nonOverlappingNeighbors;

    // non overlapping top
    const auto nonOverlappingNeighborsTop = _getNonOverlappingNeighborsFrom(neighborPartNumbers.at(0), neighborPartNumbers.at(1), neighborPartNumbers.at(2));
    for (const auto neighbor : nonOverlappingNeighborsTop) {
        nonOverlappingNeighbors.push_back(neighbor);
    }

    // non overlapping right
    const auto neighborRight = neighborPartNumbers.at(3);
    if (neighborRight) {
        nonOverlappingNeighbors.push_back(*neighborRight);
    }

    // non overlapping bottom
    const auto nonOverlappingNeighborsBottom = _getNonOverlappingNeighborsFrom(neighborPartNumbers.at(6), neighborPartNumbers.at(5), neighborPartNumbers.at(4));
    for (const auto neighbor : nonOverlappingNeighborsBottom) {
        nonOverlappingNeighbors.push_back(neighbor);
    }

    // non overlapping left
    const auto neighborLeft = neighborPartNumbers.at(7);
    if (neighborLeft) {
        nonOverlappingNeighbors.push_back(*neighborLeft);
    }

    return nonOverlappingNeighbors;
}

void _printMappings(const SchematicMappings &mappings) {
    std::cout << "partNumberToCoords:" << endl;
    std::cout << "{" << endl;
    for (const auto &p : mappings.partNumberToCoords) {
        std::cout << "\t" << p.first << " -> " << "[ ";
        for (const auto &coord : p.second) {
            std::cout << "( " << coord.x << ", " << coord.y << " ) ";
        }
        std::cout << "]" << endl;
    }
    std::cout << "}" << endl;

    std::cout << "coordToPartNumber:" << endl;
    std::cout << "{" << endl;
    for (const auto &p : mappings.coordToPartNumber) {
        std::cout << "\t (" << p.first.x << ", " << p.first.y << ")" << " -> " << p.second << endl;
    }
    std::cout << "}" << endl;

    std::cout << "symbolToCoord:" << endl;
    std::cout << "{" << endl;
    for (const auto &p : mappings.coordToSymbol) {
        std::cout << "\t (" << p.first.x << ", " << p.first.y << ")" << " -> " << p.second << endl;
    }
    std::cout << "}" << endl;
}

const SchematicMappings getPartNumberCoordMappings(const vector<string> &schematic) {
    SchematicMappings mappings;

    for (size_t x = 0; x < schematic.size(); x++) {
        size_t partNumber = 0;
        vector<SchematicCoord> coords;
        bool buildingPartNumber = false;

        const auto &line = schematic.at(x);
        for (size_t y = 0; y < line.size(); y++) {
            const char ch = line.at(y);
            const SchematicCoord currentCoord = {.x = x, .y = y};

            if (isdigit(ch)) {
                // we found a digit
                const size_t digit = ch - '0';
                if (buildingPartNumber) {
                    partNumber = partNumber * 10 + digit;
                } else {
                    partNumber = digit;
                    buildingPartNumber = true;
                }
                coords.push_back(currentCoord);
            } else {
                // we found a non-digit
                if (buildingPartNumber) {
                    // we finished building a part number
                    buildingPartNumber = false;
                    if (mappings.partNumberToCoords.find(partNumber) == mappings.partNumberToCoords.end()) {
                        mappings.partNumberToCoords[partNumber] = coords;
                    } else {
                        mappings.partNumberToCoords[partNumber].insert(mappings.partNumberToCoords[partNumber].end(), coords.begin(), coords.end());
                    }

                    // reset vars for part numbers
                    partNumber = 0;
                    coords.clear();
                }

                if (isSymbol(ch)) {
                    mappings.coordToSymbol[currentCoord] = ch;
                }
            }
        }

        // if we reach the end of a line and are currently building a part number,
        // it means we need to finish the part number and store it
        if (buildingPartNumber) {
            // we finished building a part number
            buildingPartNumber = false;
            if (mappings.partNumberToCoords.find(partNumber) == mappings.partNumberToCoords.end()) {
                mappings.partNumberToCoords[partNumber] = coords;
            } else {
                mappings.partNumberToCoords[partNumber].insert(mappings.partNumberToCoords[partNumber].end(), coords.begin(), coords.end());
            }

            // reset vars for part numbers
            partNumber = 0;
            coords.clear();
        }
    }

    for (const auto &p : mappings.partNumberToCoords) {
        for (const auto &coord : p.second) {
            mappings.coordToPartNumber[coord] = p.first;
        }
    }

    return mappings;
}

vector<size_t> getEnginePartNumbers(const vector<string> &schematic) {
    vector<size_t> enginePartNumbers;

    const auto maxX = schematic.size();
    const auto maxY = schematic.at(0).size();

    const auto schematicMappings = getPartNumberCoordMappings(schematic);
    // _printMappings(schematicMappings);

    for (const auto &coordToSymbol : schematicMappings.coordToSymbol) {
        const auto neighborCoords = getNeighborCoords(coordToSymbol.first, maxX, maxY);
        const auto neighborPartNumbers = getNeighborPartNumbers(neighborCoords, schematicMappings);
        const auto nonOverlappingNeighbors = getNonOverlappingNeighbors(neighborPartNumbers);

        for (const auto partNumber : nonOverlappingNeighbors) {
            enginePartNumbers.push_back(partNumber);
        }
    }

    return enginePartNumbers;
}

vector<size_t> getGearRatios(const vector<string> &schematic) {
    vector<size_t> gearRatios;

    const auto maxX = schematic.size();
    const auto maxY = schematic.at(0).size();

    const auto schematicMappings = getPartNumberCoordMappings(schematic);
    // _printMappings(schematicMappings);

    for (const auto &coordToSymbol : schematicMappings.coordToSymbol) {
        // only check for gear symbols
        if (coordToSymbol.second != '*') {
            continue;
        }

        const auto neighborCoords = getNeighborCoords(coordToSymbol.first, maxX, maxY);
        const auto neighborPartNumbers = getNeighborPartNumbers(neighborCoords, schematicMappings);
        const auto nonOverlappingNeighbors = getNonOverlappingNeighbors(neighborPartNumbers);

        // only check for gear symbols that have exactly 2 part number neighbors
        if (nonOverlappingNeighbors.size() != 2) {
            continue;
        }

        size_t gearRatio = 1;
        for (const auto partNumber : nonOverlappingNeighbors) {
            gearRatio *= partNumber;
        }

        gearRatios.push_back(gearRatio);
    }

    return gearRatios;
}

void part1(istream& inputFile) {
    vector<string> schematic;

    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        schematic.push_back(line);
    }

    const auto enginePartNumbers = getEnginePartNumbers(schematic);

    size_t sumOfPartNumbers = 0;
    for (const auto &enginePartNumber : enginePartNumbers) {
        sumOfPartNumbers += enginePartNumber;
    }

    std::cout << sumOfPartNumbers << endl;
}

void part2(istream& inputFile) {
    vector<string> schematic;

    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        schematic.push_back(line);
    }

    const auto gearRatios = getGearRatios(schematic);

    size_t sumOfGearRatios = 0;
    for (const auto &gearRatio : gearRatios) {
        sumOfGearRatios += gearRatio;
    }

    std::cout << sumOfGearRatios << endl;
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