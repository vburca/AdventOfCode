#include "../../util/StringUtil.h"
#include "../../util/MathUtil.h"
#include "../../util/Types.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint>
#include <vector>
#include <utility>
#include <set>
#include <queue>

using namespace std;
using namespace aoc::util::types;

using VULL = vector<size_t>;
using VVULL = vector<VULL>;
using VVVULL = vector<VVULL>;
using VVVVULL = vector<VVVULL>;

struct Input {
    VVULL cityMap;
    size_t nRows;
    size_t nCols;
    coord_t destination;
    size_t minStraightBlocks;
    size_t maxStraightBlocks;
};

enum Orientation {
    North = 0,
    East,
    South,
    West,

    OrientationCount,
};

struct LeastLossState {
    coord_t coord;
    Orientation orientation;
    size_t straightBlocks;
    size_t pathLoss;
};

// NOTE: https://en.cppreference.com/w/cpp/container/priority_queue
// Note that the Compare parameter is defined such that it returns true if its first
// argument comes before its second argument in a weak ordering. But because the priority
// queue outputs largest elements first, the elements that "come before" are actually
// output last. That is, the front of the queue contains the "last" element according to
// the weak ordering imposed by Compare.
struct LeastLossStateCompare
{
    bool operator()(const LeastLossState& s1, const LeastLossState& s2) const
    {
        return s1.pathLoss > s2.pathLoss;
    }
};

typedef priority_queue<LeastLossState, vector<LeastLossState>, LeastLossStateCompare> loss_state_priority_queue_t;

const Input _parseInput(istream& inputFile) {
    Input input;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        vector<size_t> cityMapLine;
        for (const auto &ch : line) {
            cityMapLine.push_back(ch - '0');
        }

        input.cityMap.push_back(cityMapLine);
    }

    input.nRows = input.cityMap.size();
    input.nCols = input.cityMap[0].size();

    return input;
}

void _printInput(const Input &input) {
    cout << "City map: (" << input.nRows << ", " << input.nCols << ")" << endl;
    for (const auto &line : input.cityMap) {
        for (const auto &v : line) {
            cout << v << " ";
        }
        cout << endl;
    }
    cout << endl;
}

void _printState(const LeastLossState &state) {
    cout << "State: [ ("<< state.coord.first << ", " << state.coord.second << ") orientation:" << state.orientation << " ";
    cout << "pathloss:" << state.pathLoss << " ";
    cout << "straightBlocks:" << state.straightBlocks << endl;
}

bool _isValidNextCoord(const coord_t &nextCoord, const Input &input) {
    if (nextCoord.first < 0 || nextCoord.first >= input.nRows) {
        return false;
    }

    if (nextCoord.second < 0 || nextCoord.second >= input.nCols) {
        return false;
    }

    return true;
}

bool _isDestinationState(const LeastLossState &currentState, const Input &input) {
    if (currentState.coord == input.destination) {
        return true;
    }

    return false;
}

coord_t _getNextCoordForOrientation(const coord_t &currentCoord, const Orientation &orientation) {
    if (orientation < 0 || orientation >= OrientationCount) {
        cout << "Unknown orientation " << orientation << " for coord (" << currentCoord.first << ", " << currentCoord.second << ")" << endl;
        exit(1);
    }

    switch (orientation) {
        case North: return {currentCoord.first - 1, currentCoord.second};
        case East: return {currentCoord.first, currentCoord.second + 1};
        case South: return {currentCoord.first + 1, currentCoord.second};
        case West: return {currentCoord.first, currentCoord.second - 1};
        default: return {0, 0};
    }
}

LeastLossState _createNextState(const coord_t &nextCoord, const Orientation &nextOrientation, const LeastLossState &currentState, const Input &input) {
    LeastLossState nextState = {
        .coord = nextCoord,
        .orientation = nextOrientation,
    };

    if (nextState.orientation == currentState.orientation) {
        nextState.straightBlocks = currentState.straightBlocks + 1;
    } else {
        nextState.straightBlocks = 1;
    }

    return nextState;
}

vector<LeastLossState> _getNeighbors(const LeastLossState &currentState, const Input &input) {
    vector<LeastLossState> neighbors;

    Orientation orientation = currentState.orientation;

    // continue straight if possible
    if (currentState.straightBlocks < input.maxStraightBlocks) {
        const auto nextCoord = _getNextCoordForOrientation(currentState.coord, orientation);

        if (_isValidNextCoord(nextCoord, input)) {
            neighbors.emplace_back(_createNextState(nextCoord, orientation, currentState, input));
        }
    }

    // get left/right orientations if possible
    if (currentState.straightBlocks >= input.minStraightBlocks) {
        const auto left90Orientation = static_cast<Orientation>(aoc::util::math::positiveModulo(static_cast<int>(orientation) - 1, OrientationCount));
        const auto right90Orientation = static_cast<Orientation>(aoc::util::math::positiveModulo(static_cast<int>(orientation) + 1, OrientationCount));
        for (const auto &nextOrientation : {left90Orientation, right90Orientation}) {
            const auto nextCoord = _getNextCoordForOrientation(currentState.coord, nextOrientation);

            if (_isValidNextCoord(nextCoord, input)) {
                neighbors.emplace_back(_createNextState(nextCoord, nextOrientation, currentState, input));
            }
        }
    }

    return neighbors;
}

VVVVULL _dijkstra(const coord_t &startCoord, const Input &input) {
    // distances[coordX][coordY][orientation][straightBlocks]
    VVVVULL losses = VVVVULL(input.nRows, VVVULL(input.nCols, VVULL(OrientationCount, VULL(input.maxStraightBlocks + 1, UINT64_MAX))));

    loss_state_priority_queue_t toVisit;
    for (size_t orientationIdx = 0; orientationIdx < OrientationCount; orientationIdx++) {
        const auto orientation = static_cast<Orientation>(orientationIdx);
        losses[startCoord.first][startCoord.second][orientation][0] = 0;

        toVisit.emplace(LeastLossState {
            .coord = startCoord,
            .orientation = orientation,
            .pathLoss = 0,
            .straightBlocks = 0,
        });
    }

    while (!toVisit.empty()) {
        const auto currentState = toVisit.top();
        toVisit.pop();

        if (currentState.pathLoss > losses[currentState.coord.first][currentState.coord.second][currentState.orientation][currentState.straightBlocks]) {
            continue;
        }

        const auto neighbors = _getNeighbors(currentState, input);
        for (auto neighbor : neighbors) {
            const auto neighborCost = input.cityMap[neighbor.coord.first][neighbor.coord.second];
            const auto directLoss = losses[currentState.coord.first][currentState.coord.second][currentState.orientation][currentState.straightBlocks] + neighborCost;
            if (directLoss < losses[neighbor.coord.first][neighbor.coord.second][neighbor.orientation][neighbor.straightBlocks]) {
                losses[neighbor.coord.first][neighbor.coord.second][neighbor.orientation][neighbor.straightBlocks] = directLoss;
                neighbor.pathLoss = directLoss;
                toVisit.push(neighbor);
            }
        }
    }

    return losses;
}

void part1(const Input &input) {
    const auto destination = coord_t{input.nRows - 1, input.nCols - 1};

    auto constraintsInput = input;
    constraintsInput.minStraightBlocks = 1;
    constraintsInput.maxStraightBlocks = 3;
    const auto losses = _dijkstra({0, 0}, constraintsInput);

    size_t leastLoss = UINT64_MAX;
    for (size_t orientationIdx = 0; orientationIdx < OrientationCount; orientationIdx++) {
        for (size_t straightBlocks = constraintsInput.minStraightBlocks; straightBlocks <= constraintsInput.maxStraightBlocks; straightBlocks++) {
            leastLoss = min(losses[destination.first][destination.second][orientationIdx][straightBlocks], leastLoss);
        }
    }

    cout << leastLoss << endl;
}

void part2(const Input &input) {
    const auto destination = coord_t{input.nRows - 1, input.nCols - 1};

    auto constraintsInput = input;
    constraintsInput.minStraightBlocks = 4;
    constraintsInput.maxStraightBlocks = 10;
    const auto losses = _dijkstra({0, 0}, constraintsInput);

    size_t leastLoss = UINT64_MAX;
    for (size_t orientationIdx = 0; orientationIdx < OrientationCount; orientationIdx++) {
        for (size_t straightBlocks = constraintsInput.minStraightBlocks; straightBlocks <= constraintsInput.maxStraightBlocks; straightBlocks++) {
            leastLoss = min(losses[destination.first][destination.second][orientationIdx][straightBlocks], leastLoss);
        }
    }

    cout << leastLoss << endl;
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