#include "../../util/StringUtil.h"
#include "../../util/MathUtil.h"
#include "../../util/Types.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <algorithm>
#include <cassert>

using namespace std;
using namespace aoc::util::types;

const vector<coord_t> kDirections = {
    {-1, 0}, // North
    {0,  1}, // East
    {1,  0}, // South
    {0, -1}, // West
};

struct Input {
    vector<string> gardenMap;
    coord_t startPosition;
    set<coord_t> rockPositions;
    size_t nRows;
    size_t nCols;
};

void _printInput(const Input &input, const bool showMap = false) {
    if (showMap) {
        cout << "Garden Map: (" << input.nRows << ", " << input.nCols << ")" << endl;
        for (const auto &line : input.gardenMap) {
            cout << line << endl;
        }
        cout << endl;
    }

    cout << "Start position: (" << input.startPosition.first << ", " << input.startPosition.second << ")" << endl;
}

const Input _parseInput(istream& inputFile) {
    Input input;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        input.gardenMap.push_back(line);
    }

    input.nRows = input.gardenMap.size();
    input.nCols = input.gardenMap.front().size();

    for (size_t row = 0; row < input.nRows; row++) {
        const auto &line = input.gardenMap[row];
        for (size_t col = 0; col < input.nCols; col++) {
            const auto &tile = input.gardenMap[row][col];
            if (tile == 'S') {
                input.startPosition = {row, col};
            } else if (tile == '#') {
                input.rockPositions.emplace(coord_t{row, col});
            }
        }
    }

    return input;
}

bool _isWithinBounds(const coord_t &pos, const Input &input) {
    return pos.first >= 0 && pos.first < input.nRows &&
            pos.second >= 0 && pos.second < input.nCols;
}

bool _isOpenSpace(const coord_t &pos, const Input &input) {
    return input.gardenMap[pos.first][pos.second] != '#';
}

void _printQueue(const queue<coord_t> &q) {
    queue<coord_t> qCopy = q;

    cout << "Queue: (" << qCopy.size() << ")" << endl;
    while (!qCopy.empty()) {
        const auto e = qCopy.front();
        qCopy.pop();

        cout << "(" << e.first << ", " << e.second << ")" << " ";
    }
    cout << endl;
}

size_t _bfsFrontiers(const coord_t &startPoint, const size_t numSteps, const Input &input, const bool wrapMap = false) {
    queue<coord_signed_t> currentFrontier;
    currentFrontier.push(startPoint);

    set<coord_signed_t> visited;
    visited.insert(startPoint);

    size_t stepCounter = 0;
    while (stepCounter < numSteps) {
        queue<coord_signed_t> nextFrontier;

        // cout << "Step: " << stepCounter << endl;
        // _printQueue(currentFrontier);

        while (!currentFrontier.empty()) {
            const auto &currentPosition = currentFrontier.front();
            currentFrontier.pop();

            for (const auto &positionDelta : kDirections) {
                const auto nextPosition = coord_signed_t {
                    currentPosition.first + positionDelta.first,
                    currentPosition.second + positionDelta.second };

                if (wrapMap) {
                    const auto nextPositionWithinMapBounds = coord_signed_t {
                        aoc::util::math::positiveModulo(nextPosition.first, input.nRows),
                        aoc::util::math::positiveModulo(nextPosition.second, input.nCols) };
                    if (_isOpenSpace(nextPositionWithinMapBounds, input) &&
                        !visited.contains(nextPosition)) {
                        nextFrontier.push(nextPosition);
                        visited.insert(nextPosition);
                    }
                } else {
                    if (_isWithinBounds(nextPosition, input) &&
                        _isOpenSpace(nextPosition, input) &&
                        !visited.contains(nextPosition)) {
                        nextFrontier.push(nextPosition);
                        visited.insert(nextPosition);
                    }
                }
            }
        }

        stepCounter += 1;
        currentFrontier = nextFrontier;
        visited.clear();
    }

    if (stepCounter == numSteps) {
        return currentFrontier.size();
    }

    return 0;
}

// this is much faster than _bfsFrontiers but only works for even `numSteps` or for when we don't wrap around the map
map<coord_signed_t, map<coord_signed_t, size_t>> _bfsDistances(const coord_t &startPoint, const size_t numSteps, const Input &input, const bool wrapMap = false) {
    struct PositionData {
        coord_signed_t wrappedPosition;
        coord_signed_t mapPosition;
        size_t distance;
    };

    const PositionData startPositionData = {startPoint, {0, 0}, 0};

    queue<PositionData> currentFrontier;
    currentFrontier.push(startPositionData);

    map<coord_signed_t, map<coord_signed_t, size_t>> mapPositionToDistances;

    while (!currentFrontier.empty()) {
        const auto &currentPositionData = currentFrontier.front();
        currentFrontier.pop();

        const auto &currentMapPosition = currentPositionData.mapPosition;
        const auto &currentPosition = currentPositionData.wrappedPosition;

        if (mapPositionToDistances.contains(currentMapPosition) &&
            mapPositionToDistances.at(currentMapPosition).contains(currentPosition)) {
                continue;
            }

        auto &mapDistances = mapPositionToDistances[currentMapPosition];
        mapDistances[currentPosition] = currentPositionData.distance;

        for (const auto &positionDelta : kDirections) {
            const auto nextPosition = coord_signed_t {
                currentPosition.first + positionDelta.first,
                currentPosition.second + positionDelta.second };

            if (wrapMap) {
                coord_signed_t nextMapPosition = currentMapPosition;
                if (positionDelta == coord_t{-1, 0} && nextPosition.first < 0) {
                    nextMapPosition.first -= 1;
                } else if (positionDelta == coord_t{0, 1} && nextPosition.second > input.nCols - 1) {
                    nextMapPosition.second += 1;
                } else if (positionDelta == coord_t{1, 0} && nextPosition.first > input.nRows - 1) {
                    nextMapPosition.first += 1;
                } else if (positionDelta == coord_t{0, -1} && nextPosition.second < 0) {
                    nextMapPosition.second -= 1;
                }

                const auto nextPositionWithinMapBounds = coord_signed_t {
                    aoc::util::math::positiveModulo(nextPosition.first, input.nRows),
                    aoc::util::math::positiveModulo(nextPosition.second, input.nCols) };

                const auto nextPositionData = PositionData{nextPositionWithinMapBounds, nextMapPosition, currentPositionData.distance + 1};

                if (_isOpenSpace(nextPositionData.wrappedPosition, input) &&
                    nextPositionData.distance <= numSteps + 1) {
                    currentFrontier.push(nextPositionData);
                }
            } else {
                const auto nextPositionData = PositionData{nextPosition, currentMapPosition, currentPositionData.distance + 1};
                if (_isWithinBounds(nextPosition, input) &&
                    _isOpenSpace(nextPosition, input)) {
                    currentFrontier.push(nextPositionData);
                }
            }
        }
    }

    return mapPositionToDistances;
}

void part1(const Input &input) {
    constexpr size_t kNumSteps = 64;
    const auto mapPositionToDistances = _bfsDistances(input.startPosition, kNumSteps, input);

    size_t numGardenPlotsReached = 0;
    for (const auto &p : mapPositionToDistances) {
        const auto &mapPosition = p.first;
        const auto &mapDistances = p.second;
        for (const auto &md : mapDistances) {
            const auto &distance = md.second;
            if (distance <= kNumSteps && distance % 2 == 0) {
                numGardenPlotsReached += 1;
            }
        }
    }

    cout << numGardenPlotsReached << endl;
}

// NOTE: https://www.geeksforgeeks.org/lagrange-interpolation-formula/
int64_t _lagrangeSecondOrderInterpolation(const int64_t x, const vector<int64_t> &xs, const vector<int64_t> &ys) {
    assert(xs.size() == 3 && ys.size() == 3);

    // f(x) = ((x - x1) * (x - x2)) / ((x0 - x1) * (x0 - x2)) * y0 +
    //          ((x - x0) * (x - x2)) / ((x1 - x0) * (x1 - x2)) * y1 +
    //          ((x - x0) * (x - x1)) / ((x2 - x0) * (x2 - x1)) * y2

    const int64_t first = ((x - xs[1]) * (x - xs[2])) / ((xs[0] - xs[1]) * (xs[0] - xs[2])) * ys[0];
    const int64_t second = ((x - xs[0]) * (x - xs[2])) / ((xs[1] - xs[0]) * (xs[1] - xs[2])) * ys[1];
    const int64_t third = ((x - xs[0]) * (x - xs[1])) / ((xs[2] - xs[0]) * (xs[2] - xs[1])) * ys[2];

    return first + second + third;
}

// NOTE: Got help from the following:
// https://www.reddit.com/r/adventofcode/comments/18nevo3/comment/keasebj/?utm_source=share&utm_medium=web2x&context=3
// https://www.reddit.com/r/adventofcode/comments/18nevo3/comment/keaj1mg/?utm_source=reddit&utm_medium=web2x&context=3
void part2(const Input &input) {
    const vector<int64_t> stepsToComputeFor = {(int64_t)input.nRows / 2, (int64_t)(3 * input.nRows) / 2, (int64_t)(5 * input.nRows) / 2};
    const auto maxStepsToComputeFor = *max_element(stepsToComputeFor.begin(), stepsToComputeFor.end());

    vector<int64_t> numGardenPlotsReachedPerSteps;
    for (const auto &steps : stepsToComputeFor) {
        numGardenPlotsReachedPerSteps.push_back(_bfsFrontiers(input.startPosition, steps, input, true));
    }

    for (size_t i = 0; i < stepsToComputeFor.size(); i++) {
        cout << stepsToComputeFor[i] << " -> " << numGardenPlotsReachedPerSteps[i] << endl;
    }

    constexpr size_t kNumSteps = 26501365;
    const auto numGardenPlotsReached = _lagrangeSecondOrderInterpolation(kNumSteps, stepsToComputeFor, numGardenPlotsReachedPerSteps);

    cout << numGardenPlotsReached << endl;
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        std::cout << "Must specify input file!" << endl;
        return 0;
    }

    std::cout << "Input file name: " << argv[1] << endl;
    ifstream inputFile(argv[1]);

    const auto input = _parseInput(inputFile);
    // _printInput(input, true);

    std::cout << "Part 1: " << endl;
    part1(input);

    std::cout << "Part 2: " << endl;
    part2(input);

    inputFile.close();

    return 0;
}