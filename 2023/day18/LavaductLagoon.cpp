#include "../../util/StringUtil.h"
#include "../../util/MathUtil.h"
#include "../../util/Types.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint>
#include <vector>
#include <regex>
#include <set>
#include <queue>

using namespace std;
using namespace aoc::util::types;

regex PLAN_LINE_RX("([UDLR]) ([0-9]+) \\(#([a-z0-9]+)\\)");

enum Direction {
    Right = 0,
    Down,
    Left,
    Up,

    DirectionCount,
};

struct Instruction {
    Direction dir;
    size_t distance;
    string color;
};

struct Input {
    vector<Instruction> instructions;
    set<coord_t> polyEdges;
    vector<coord_t> polyCorners;
    size_t nRows;
    size_t nCols;
};

Direction _parseDirection(const char dirCh) {
    switch (dirCh) {
        case 'U': return Up;
        case 'R': return Right;
        case 'D': return Down;
        case 'L': return Left;
        default: {
            cout << "Unknown direction character: " << dirCh << endl;
            exit(1);
        }
    }

    return Up;
}

const coord_signed_t _directionToCoordDelta(const Direction &dir) {
    switch (dir) {
        case Up: return {-1, 0};
        case Right: return {0, 1};
        case Down: return {1, 0};
        case Left: return {0, -1};
        default: {
            cout << "Unknown direction: " << dir << endl;
            exit(1);
        }
    }

    return {0, 0};
}

Instruction _correctInstruction(const Instruction &instruction) {
    Instruction correctedInstruction;
    correctedInstruction.color = instruction.color;
    correctedInstruction.dir = static_cast<Direction>(instruction.color[instruction.color.size() - 1] - '0');

    stringstream ss;
    ss << hex << correctedInstruction.color.substr(0, correctedInstruction.color.size() - 1);
    ss >> correctedInstruction.distance;

    return correctedInstruction;
}

void _expandInput(Input &input, bool applyInputCorrection = false, bool generateEdges = false) {
    int64_t minRow = INT64_MAX;
    int64_t minCol = INT64_MAX;

    vector<coord_signed_t> tempPolyCorners;
    set<coord_signed_t> tempPolyEdges;
    coord_signed_t signedPrevCoord = {0, 0};
    tempPolyCorners.push_back(signedPrevCoord);
    tempPolyEdges.insert(signedPrevCoord);

    for (auto &instruction : input.instructions) {
        if (applyInputCorrection) {
            instruction = _correctInstruction(instruction);
        }

        auto coordDelta = _directionToCoordDelta(instruction.dir);
        coordDelta = {coordDelta.first * instruction.distance, coordDelta.second * instruction.distance};
        const coord_signed_t signedCurrentCoord = {signedPrevCoord.first + coordDelta.first, signedPrevCoord.second + coordDelta.second};

        if (generateEdges) {
            // fill in the edge between successive poly corners
            const coord_signed_t startCoord = {min(signedPrevCoord.first, signedCurrentCoord.first), min(signedPrevCoord.second, signedCurrentCoord.second)};
            const coord_signed_t endCoord = {max(signedPrevCoord.first, signedCurrentCoord.first), max(signedPrevCoord.second, signedCurrentCoord.second)};

            for (int64_t i = startCoord.first; i <= endCoord.first; i++) {
                for (int64_t j = startCoord.second; j <= endCoord.second; j++) {
                    tempPolyEdges.emplace(coord_signed_t{i, j});
                }
            }
        }

        tempPolyEdges.insert(signedCurrentCoord);
        tempPolyCorners.push_back(signedCurrentCoord);

        minRow = min(minRow, signedCurrentCoord.first);
        minCol = min(minCol, signedCurrentCoord.second);

        signedPrevCoord = signedCurrentCoord;
    }

    // shift it towards top left corner (0, 0)
    size_t maxRow = 0;
    size_t maxCol = 0;
    for (const auto &signedCurrentCoord : tempPolyEdges) {
        const coord_t currentCoord = {signedCurrentCoord.first + abs(minRow), signedCurrentCoord.second + abs(minCol)};
        input.polyEdges.insert(currentCoord);

        maxRow = max(maxRow, currentCoord.first);
        maxCol = max(maxCol, currentCoord.second);
    }

    for (const auto &signedCurrentCoord : tempPolyCorners) {
        const coord_t currentCoord = {signedCurrentCoord.first + abs(minRow), signedCurrentCoord.second + abs(minCol)};
        input.polyCorners.push_back(currentCoord);
    }

    input.nRows = maxRow + 1;
    input.nCols = maxCol + 1;
}

const Input _parseInput(istream& inputFile) {
    Input input;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        smatch instructionMatch;
        if (!regex_match(line, instructionMatch, PLAN_LINE_RX)) {
            cout << "PLAN_LINE_RX failed to match for line: { " << line << " }" << endl;
            exit(1);
        }

        input.instructions.emplace_back(Instruction {
            .dir = _parseDirection(instructionMatch[1].str()[0]),
            .distance = stoull(instructionMatch[2].str()),
            .color = instructionMatch[3].str(),
        });
    }

    return input;
}

void _printInput(const Input &input, bool printMap = true) {
    // cout << "Dig instructions: (" << input.instructions.size() << ")" << endl;
    // for (const auto &instr : input.instructions) {
    //     switch (instr.dir) {
    //         case Up: cout << "U"; break;
    //         case Right: cout << "R"; break;
    //         case Down: cout << "D"; break;
    //         case Left: cout << "L"; break;
    //         default: cout << "?";
    //     }
    //     cout << " " << instr.distance << " (#" << instr.color << ")" << endl;
    // }
    // cout << endl;

    {
        cout << "Poly corners: (" << input.polyCorners.size() << ")" << endl;
        if (printMap) {
            for (const auto &corner : input.polyCorners) {
                cout << "(" << corner.first << ", " << corner.second << ") ";
            }
            cout << endl;
        }
    }

    {
        cout << "Dig map: (" << input.nRows << ", " << input.nCols << ")" << endl;
        if (printMap) {
            ofstream bordersMapFile("bordersMap.txt");
            for (size_t i = 0; i < input.nRows; i++) {
                for (size_t j = 0; j < input.nCols; j++) {
                    if (input.polyEdges.contains(coord_signed_t{i, j})) {
                        bordersMapFile << "#";
                    } else {
                        bordersMapFile << ".";
                    }
                }
                bordersMapFile << endl;
            }
            bordersMapFile.close();
        }
    }
}

bool _isValidCoord(const coord_t &coord, const Input &input) {
    if (coord.first < 0 || coord.first >= input.nRows) {
        return false;
    }

    if (coord.second < 0 || coord.second >= input.nCols) {
        return false;
    }

    return true;
}

void _bfs(const coord_t &startCoord, set<coord_t> &visited, const Input &input) {
    queue<coord_t> toVisit;
    toVisit.push(startCoord);

    while (!toVisit.empty()) {
        const auto currentCoord = toVisit.front();
        toVisit.pop();

        if (visited.contains(currentCoord) || input.polyEdges.contains(currentCoord)) {
            continue;
        }

        visited.insert(currentCoord);

        for (const auto delta : {-1, 1}) {
            const auto neighborVertical = coord_t {currentCoord.first + delta, currentCoord.second};
            const auto neighborHorizontal = coord_t {currentCoord.first, currentCoord.second + delta};

            if (_isValidCoord(neighborVertical, input) &&
                !visited.contains(neighborVertical) &&
                !input.polyEdges.contains(neighborVertical)) {
                toVisit.push(neighborVertical);
            }

            if (_isValidCoord(neighborHorizontal, input) &&
                !visited.contains(neighborHorizontal) &&
                !input.polyEdges.contains(neighborHorizontal)) {
                toVisit.push(neighborHorizontal);
            }
        }
    }
}

set<coord_t> _getExternalCoords(const Input &input) {
    set<coord_t> visited;

    // perform BFS on all non-visited edge points
    for (size_t row = 0; row < input.nRows; row++) {
        for (size_t col = 0; col < input.nCols; col++) {
            if (row == 0 || col == 0 || row == input.nRows - 1 || col == input.nCols - 1) {
                const auto currentEdgeCoord = coord_t {row, col};
                if (visited.contains(currentEdgeCoord) || input.polyEdges.contains(currentEdgeCoord)) {
                    continue;
                }

                _bfs(currentEdgeCoord, visited, input);
            }
        }
    }

    return visited;
}

// Shoelace Formula: https://en.wikipedia.org/wiki/Shoelace_formula
// Visualization: https://new.reddit.com/r/adventofcode/comments/18l2tap/2023_day_18_the_elves_and_the_shoemaker/
size_t _computePolyArea(const Input &input) {
    int64_t interiorArea = 0;

    for (size_t i = 0; i < input.polyCorners.size() - 1; i++) {
        const auto cornerA = input.polyCorners[i];
        const auto cornerB = input.polyCorners[i + 1];

        // add half the area of the parallelogram determined by:
        // (0, 0), (cornerA.x, cornerA.y), (cornerB.x, cornerB.y), (cornerA.x + cornerB.x, cornerA.y + cornerB.y)
        // this area is calculated by the determinant of:
        // | cornerA.x  cornerB.x |
        // | cornerA.y  cornerB.y |
        // half of this area is going to be the area of a triangle that (0, 0) makes with the [cornerA, cornerB] edge
        interiorArea += ((int64_t)cornerA.first * (int64_t)cornerB.second) - ((int64_t)cornerB.first * (int64_t)cornerA.second);
    }

    return abs(interiorArea) / 2;
}

size_t _computePolyPerimeter(const Input &input) {
    size_t polyPerimeter = 0;
    for (size_t i = 0; i < input.polyCorners.size() - 1; i++) {
        const auto coordA = input.polyCorners[i];
        const auto coordB = input.polyCorners[i + 1];

        const auto edgeLength = abs((int64_t)coordA.first - (int64_t)coordB.first) + abs((int64_t)coordA.second - (int64_t)coordB.second);
        polyPerimeter += edgeLength;
    }

    return polyPerimeter;
}

// We need to calculate #interior_points (i) + #boundary_points (b) = i + b
// We will use Pick's Formula and Shoelace Formula (above):
// Pick's: A = i + b / 2 - 1
// => i = A - b / 2 + 1, where b in our case is the perimeter
// We can get A (area) from the Shoelace Formula, above
// => i + b = A + perimeter / 2 + 1
size_t _computeTotalCoverArea(const size_t interiorPolyArea, const size_t polyPerimeter) {
    return interiorPolyArea + polyPerimeter / 2 + 1;
}

void part1(const Input &input) {
    Input p1Input = input;
    _expandInput(p1Input, false, false);
    // _printInput(p1Input, false);

    const auto polyArea = _computePolyArea(p1Input);
    const auto polyPerimeter = _computePolyPerimeter(p1Input);

    const auto totalCapacity = _computeTotalCoverArea(polyArea, polyPerimeter);

    cout << totalCapacity << endl;
}

void part2(const Input &input) {
    Input p2Input = input;
    _expandInput(p2Input, true, false);
    _printInput(p2Input, false);

    const auto polyArea = _computePolyArea(p2Input);
    const auto polyPerimeter = _computePolyPerimeter(p2Input);

    const auto totalCapacity = _computeTotalCoverArea(polyArea, polyPerimeter);

    cout << totalCapacity << endl;
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        std::cout << "Must specify input file!" << endl;
        return 0;
    }

    std::cout << "Input file name: " << argv[1] << endl;
    ifstream inputFile(argv[1]);

    const auto input = _parseInput(inputFile);
    // _printInput(input);

    std::cout << "Part 1: " << endl;
    part1(input);

    std::cout << "Part 2: " << endl;
    part2(input);

    inputFile.close();

    return 0;
}