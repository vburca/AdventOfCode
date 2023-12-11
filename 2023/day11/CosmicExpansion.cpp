#include "../../util/StringUtil.h"
#include "../../util/MathUtil.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint>
#include <vector>
#include <utility>

using namespace std;

typedef pair<size_t, size_t> coord_t;

struct Input {
    vector<string> map;
    vector<coord_t> galaxyCoords;
    vector<size_t> emptyRows;
    vector<size_t> emptyCols;
    size_t numRows;
    size_t numCols;
};

void _printInput(const Input &input) {
    cout << "Map: " << endl;
    for (const auto &line : input.map) {
        cout << line << endl;
    }
    cout << endl;

    cout << "Map size: (" << input.numRows << ", " << input.numCols << ")" << endl << endl;

    cout << "Galaxy coords: " << endl;
    for (size_t i = 0; i < input.galaxyCoords.size(); i++) {
        const auto galaxyCoords = input.galaxyCoords[i];
        cout << "Galaxy " << i << " coords: (" << galaxyCoords.first << ", " << galaxyCoords.second << ")" << endl;
    }
    cout << endl;

    cout << "Empty rows: " << endl << "[ ";
    for (const auto &e : input.emptyRows) {
        cout << e << " ";
    }
    cout << "]" << endl << endl;

    cout << "Empty cols: " << endl << "[ ";
    for (const auto &e : input.emptyCols) {
        cout << e << " ";
    }
    cout << "]" << endl << endl;
}

const Input _parseInput(istream& inputFile) {
    Input input;

    size_t row = 0;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        input.map.push_back(line);

        bool foundGalaxy = false;
        for (size_t col = 0; col < line.size(); col++) {
            if (line[col] == '#') {
                input.galaxyCoords.emplace_back(row, col);
                foundGalaxy = true;
            }
        }

        if (!foundGalaxy) {
            input.emptyRows.push_back(row);
        }

        row += 1;
    }

    input.numRows = input.map.size();
    input.numCols = input.map[0].size();

    for (size_t col = 0; col < input.numCols; col++) {
        bool isEmptyCol = true;
        for (size_t row = 0; row < input.numRows; row++) {
            if (input.map[row][col] == '#') {
                isEmptyCol = false;
                break;
            }
        }

        if (isEmptyCol) {
            input.emptyCols.push_back(col);
        }
    }

    // sort(input.galaxyCoords.begin(), input.galaxyCoords.end(), [](const coord_t &l, const coord_t &r) {
    //     return l.first < l.second;
    // });

    return input;
}

size_t _getExpansionsCount(const size_t start, const size_t end, const Input &input, bool forCols = false) {
    vector<size_t> emptyLines = input.emptyRows;
    if (forCols) {
        emptyLines = input.emptyCols;
    }

    const auto minId = min(start, end);
    const auto maxId = max(start, end);

    size_t expansionsCount = 0;
    for (const auto emptyId : emptyLines) {
        if (emptyId > maxId) {
            break;
        }

        if (emptyId > minId && emptyId < maxId) {
            expansionsCount += 1;
        }
    }

    return expansionsCount;
}

size_t _getDistanceBetween(coord_t src, coord_t dst, const Input &input, const size_t expansionMultiplier = 1) {
    const auto expansionFactorRows = _getExpansionsCount(src.first, dst.first, input, false);
    const auto expansionFactorCols = _getExpansionsCount(src.second, dst.second, input, true);

    // cout << "Expansion rows: " << expansionFactorRows << endl;
    // cout << "Expansion cols: " << expansionFactorCols << endl;

    const auto dr = abs(static_cast<int32_t>(dst.first) - static_cast<int32_t>(src.first));
    const auto dc = abs(static_cast<int32_t>(dst.second) - static_cast<int32_t>(src.second));

    // cout << "dr: " << dr << endl;
    // cout << "dc: " << dc << endl;

    const auto deltaRows = dr + expansionFactorRows * (expansionMultiplier - 1);
    const auto deltaCols = dc + expansionFactorCols * (expansionMultiplier - 1);

    const auto distance = deltaRows + deltaCols;

    return distance;
}

void part1(const Input &input) {
    size_t sumShortestPaths = 0;

    for (size_t i = 0; i < input.galaxyCoords.size(); i++) {
        for (size_t j = i + 1; j < input.galaxyCoords.size(); j++) {
            const auto d = _getDistanceBetween(input.galaxyCoords[i], input.galaxyCoords[j], input, 1);

            // cout << "Between galaxy " << i << " and galaxy " << j << ": " << d << endl;

            sumShortestPaths += d;
        }
    }

    cout << sumShortestPaths << endl;
}

void part2(const Input &input) {
    size_t sumShortestPaths = 0;

    for (size_t i = 0; i < input.galaxyCoords.size(); i++) {
        for (size_t j = i + 1; j < input.galaxyCoords.size(); j++) {
            const auto d = _getDistanceBetween(input.galaxyCoords[i], input.galaxyCoords[j], input, 1000000);

            // cout << "Between galaxy " << i << " and galaxy " << j << ": " << d << endl;

            sumShortestPaths += d;
        }
    }

    cout << sumShortestPaths << endl;
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        std::cout << "Must specify input file!" << endl;
        return 0;
    }

    std::cout << "Input file name: " << argv[1] << endl;
    ifstream inputFile(argv[1]);

    const auto input = _parseInput(inputFile);
    _printInput(input);

    std::cout << "Part 1: " << endl;
    part1(input);

    std::cout << "Part 2: " << endl;
    part2(input);

    inputFile.close();

    return 0;
}