#include "../../util/StringUtil.h"
#include "../../util/MathUtil.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint>
#include <vector>
#include <map>
#include <optional>
#include <queue>
#include <set>

using namespace std;

typedef pair<size_t, size_t> coord_t;

constexpr char kStart = 'S';

enum Direction {
    North = 0,
    East,
    South,
    West,
};

struct DirectionInformation {
    Direction oppositeDirection;
    coord_t deltaPos;
    string validPipes;
};

const map<Direction, DirectionInformation> kPossibleDirections = {
    { North, {
        .oppositeDirection = South,
        .deltaPos = make_pair(-1, 0),
        .validPipes = "S|LJ",
    }},
    { East, {
        .oppositeDirection = West,
        .deltaPos = make_pair(0, +1),
        .validPipes = "SF-L",
    }},
    { South, {
        .oppositeDirection = North,
        .deltaPos = make_pair(+1, 0),
        .validPipes = "SF7|",
    }},
    { West, {
        .oppositeDirection = East,
        .deltaPos = make_pair(0, -1),
        .validPipes = "SJ7-",
    }},
};

struct Input {
    vector<string> map;
    size_t numRows;
    size_t numCols;
    coord_t startPos;
    set<coord_t> hallucinatedComponents;
};

struct BFSOutput {
    vector<vector<size_t>> visitedAtStep;
    set<coord_t> visitedPos;
};

void _printMat(const vector<vector<size_t>> &mat) {
    for (const auto &r : mat) {
        for (const auto &e : r) {
            cout << e << " ";
        }
        cout << endl;
    }
    cout << endl;
}

void _printPosSet(const set<coord_t> &s) {
    for (const auto &p : s) {
        cout << "(" << p.first << ", " << p.second << ") ";
    }
    cout << endl << "(size = " << s.size() << ")" << endl << endl;
}

void _printInput(const Input &input) {
    for (const auto &line : input.map) {
        cout << line << endl;
    }

    cout << "( " << input.numRows << ", " << input.numCols << " )" << endl;
    cout << "Start position: ( " << input.startPos.first << ", " << input.startPos.second << " )" << endl;

    if (!input.hallucinatedComponents.empty()) {
        cout << "Hallucinated positions: " << endl;
        _printPosSet(input.hallucinatedComponents);
    }
    cout << endl;
}

const Input _parseInput(istream& inputFile) {
    Input input;

    bool foundStart = false;
    size_t startRow = 0;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        input.map.push_back(line);

        if (!foundStart) {
            const auto startCol = line.find(kStart);
            if (startCol < line.size()) {
                input.startPos = make_pair<>(startRow, startCol);
                foundStart = true;
                continue;
            }

            startRow += 1;
        }
    }

    input.numRows = input.map.size();
    input.numCols = input.map[0].size();

    return input;
}

bool _isValidPos(const coord_t &pos, const Input &input) {
    if (pos.first < 0 || pos.first >= input.numRows ||
        pos.second < 0 || pos.second >= input.numCols) {
        return false;
    }

    return true;
}

set<coord_t> _edgeFillBFS(set<coord_t> nonLoopPos, const Input &input) {
    // find all the non loop positions that are on the edge of the map
    queue<coord_t> posToVisit;
    for (const auto &p : nonLoopPos) {
        if (p.first == 0 || p.second == 0 ||
            p.first == input.numRows - 1 || p.second == input.numCols - 1) {
            posToVisit.push(p);
        }
    }

    // no non-loop positions are on the edge of the map, so the loop encloses everything in the map
    if (posToVisit.empty()) {
        return nonLoopPos;
    }


    // start BFS fill from the non loop positions that are on the edge of the map
    set<coord_t> visitedPos;
    set<coord_t> nonOutsideAndNonLoopPos = nonLoopPos;
    while (!posToVisit.empty()) {
        const auto currPos = posToVisit.front();
        posToVisit.pop();

        if (!_isValidPos(currPos, input)) {
            continue;
        }

        if (visitedPos.contains(currPos)) {
            continue;
        }

        // current element is part of the outside position fill, so remove it from the
        // "non outside and non loop positions" set
        nonOutsideAndNonLoopPos.erase(currPos);

        visitedPos.insert(currPos);

        for (const auto &possibleDirection : kPossibleDirections) {
            const auto directionInfo = possibleDirection.second;

            const auto nextRow = currPos.first + directionInfo.deltaPos.first;
            const auto nextCol = currPos.second + directionInfo.deltaPos.second;
            const auto nextPos = make_pair(nextRow, nextCol);

            // if the generated position is not valid (outside of map) OR
            // if it is NOT a non loop position (i.e. it is a loop position),
            // move on, we are not interested in this position
            if (!_isValidPos(nextPos, input) || !nonLoopPos.contains(nextPos)) {
                continue;
            }

            posToVisit.push(nextPos);
        }
    }

    return nonOutsideAndNonLoopPos;
}

const BFSOutput _traverseMapBFS(const Input &input) {
    BFSOutput output = {
        .visitedAtStep = vector(input.numRows, vector<size_t>(input.numCols)),
    };

    // queue of (position coordonate, step count for this visit)
    queue<pair<coord_t, size_t>> toVisit;
    toVisit.push(make_pair(input.startPos, 0));

    while (!toVisit.empty()) {
        const auto p = toVisit.front();
        toVisit.pop();

        const auto currPos = p.first;
        const auto stepCount = p.second;

        // outside map boundaries
        if (!_isValidPos(currPos, input)) {
            continue;
        }

        // already visited position
        if (output.visitedPos.contains(make_pair(currPos.first, currPos.second))) {
            continue;
        }

        output.visitedAtStep[currPos.first][currPos.second] = stepCount;
        output.visitedPos.emplace(currPos.first, currPos.second);

        const auto currentPipe = input.map[currPos.first][currPos.second];

        // cout << "CURRENT STATE: " << endl;
        // cout << "Current Pipe: " << currentPipe << "( " << currPos.first << ", " << currPos.second << " )" << endl;
        // _printMat(visitedAtStep);
        // cout << endl << " ================ " << endl << endl;

        for (const auto &possibleDirection : kPossibleDirections) {
            const auto direction = possibleDirection.first;
            const auto directionInfo = possibleDirection.second;

            if (!directionInfo.validPipes.contains(currentPipe)) {
                continue;
            }

            const auto nextRow = currPos.first + directionInfo.deltaPos.first;
            const auto nextCol = currPos.second + directionInfo.deltaPos.second;
            const auto nextPos = make_pair(nextRow, nextCol);

            if (!_isValidPos(nextPos, input)) {
                continue;
            }

            // check destination validity
            const auto destinationPipe = input.map[nextPos.first][nextPos.second];
            const auto destinationInfo = kPossibleDirections.at(directionInfo.oppositeDirection);
            if (!destinationInfo.validPipes.contains(destinationPipe)) {
                continue;
            }

            toVisit.push(make_pair(nextPos, stepCount + 1));
        }
    }

    return output;
}

void part1(const Input &input) {
    const auto bfsOutput = _traverseMapBFS(input);

    size_t maxCount = 0;
    for (const auto &row : bfsOutput.visitedAtStep) {
        for (const auto count : row) {
            maxCount = max(maxCount, count);
        }
    }

    cout << maxCount << endl;
}

vector<string> _expandPipe(const char pipe) {
    switch (pipe) {
        case '|':
            return { "|.",
                     "|." };
        case '-':
            return { "--",
                     ".." };
        case 'L':
            return { "L-",
                     ".." };
        case 'J':
            return { "J.",
                     ".." };
        case '7':
            return { "7.",
                     "|." };
        case 'F':
            return { "F-",
                     "|." };
        case '.':
            return { "..",
                     ".." };
        case 'S':
            return { "S-",
                     "|." };
    }

    return { "..",
             ".." };
}

Input _expandMap(const Input &input) {
    Input expandedMap = {
        .map = vector(input.numRows * 2, string(input.numCols * 2, '?')),
        .numRows = input.numRows * 2,
        .numCols = input.numCols * 2,
        .startPos = make_pair(input.startPos.first * 2, input.startPos.second * 2),
    };

    for (size_t row = 0; row < input.numRows; row++) {
        for (size_t col = 0; col < input.numCols; col++) {
            const auto pipe = input.map[row][col];
            const auto expandedPipe = _expandPipe(pipe);

            for (size_t i = 0; i < expandedPipe.size(); i++) {
                const auto r = expandedPipe[i];
                for (size_t j = 0; j < r.size(); j++) {
                    const auto expandedMapRow = row * 2 + i;
                    const auto expandedMapCol = col * 2 + j;
                    expandedMap.map[expandedMapRow][expandedMapCol] = expandedPipe[i][j];

                    if (i != 0 || j != 0) {
                        expandedMap.hallucinatedComponents.emplace(expandedMapRow, expandedMapCol);
                    }
                }
            }
        }
    }

    return expandedMap;
}

void part2(const Input &input) {
    // expand the resolution of the input map to deal with "squeezing" - i.e. components / walls / sides of the loop
    // that touch each other but allow "walk" in between them
    Input expandedInput = _expandMap(input);
    // _printInput(expandedInput);

    // find the positions that are part of the loop
    const auto bfsOutput = _traverseMapBFS(expandedInput);

    // get the set of all the non loop positions
    set<coord_t> nonLoopPos;
    for (size_t i = 0; i < expandedInput.numRows; i++) {
        for (size_t j = 0; j < expandedInput.numCols; j++) {
            const coord_t currPos = make_pair(i, j);
            if (bfsOutput.visitedPos.contains(currPos)) {
                // this is a loop position, ignore it
                continue;
            }

            // not a loop position, add it to the set of interesting positions
            nonLoopPos.insert(currPos);
        }
    }

    // filter the non loop positions and remove the outside loop ones through BFS fill (from an edge, non-loop position)
    const auto internalLoopPos = _edgeFillBFS(nonLoopPos, expandedInput);

    // now we need to see how many positions from the internal loop positions of the expanded map
    // are not hallucinated by our resolution expansion
    set<coord_t> realInternalLoopPos;
    for (const auto &p : internalLoopPos) {
        if (!expandedInput.hallucinatedComponents.contains(p)) {
            realInternalLoopPos.insert(p);
        }
    }

    cout << realInternalLoopPos.size() << endl;
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