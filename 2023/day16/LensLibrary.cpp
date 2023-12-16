#include "../../util/StringUtil.h"
#include "../../util/MathUtil.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint>
#include <vector>
#include <utility>
#include <queue>
#include <set>
#include <tuple>

using namespace std;

typedef pair<size_t, size_t> coord_t;

enum Direction {
    North,
    East,
    South,
    West,
};

struct PositionData {
    coord_t coord;
    Direction dir;

    bool operator==(const PositionData &other) const {
        return tie(coord, dir) == tie(other.coord, other.dir);
    }

    bool operator<(const PositionData &other) const {
        return tie(coord, dir) < tie(other.coord, other.dir);
    }
};

struct BFSOutput {
    set<coord_t> visitedCoords;
    set<PositionData> visitedEdgeExitPositions;
};

struct Input {
    vector<string> grid;
    size_t nRows;
    size_t nCols;
};

const Input _parseInput(istream& inputFile) {
    Input input;

    string step;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        input.grid.push_back(line);
    }

    input.nRows = input.grid.size();
    input.nCols = input.grid[0].size();

    return input;
}

void _printInput(const Input &input) {
    cout << "Grid: (" << input.nRows << ", " << input.nCols << ")" << endl;
    for (const auto &l : input.grid) {
        cout << l << endl;
    }
    cout << endl;
}

Direction _getDirectionFromCoordDelta(const coord_t &coordDelta) {
    if (coordDelta == coord_t{-1, 0}) {
        return North;
    }

    if (coordDelta == coord_t{1, 0}) {
        return South;
    }

    if (coordDelta == coord_t{0, -1}) {
        return West;
    }

    if (coordDelta == coord_t{0, 1}) {
        return East;
    }

    cout << "Unknown direction for coord delta: (" << coordDelta.first << ", " <<coordDelta.second << ")" << endl;
    exit(1);
}

vector<coord_t> _getCoordDeltas(const Direction &dir, const char currentTile) {
    if (currentTile == '.') {
        switch (dir) {
            case North: return {{-1, 0}};
            case East: return {{0, 1}};
            case South: return {{1, 0}};
            case West: return {{0, -1}};
        };
    }

    if (currentTile == '\\') {
        switch (dir) {
            case North: return {{0, -1}};
            case East: return {{1, 0}};
            case South: return {{0, 1}};
            case West: return {{-1, 0}};
        };
    }

    if (currentTile == '/') {
        switch (dir) {
            case North: return {{0, 1}};
            case East: return {{-1, 0}};
            case South: return {{0, -1}};
            case West: return {{1, 0}};
        };
    }

    if (currentTile == '|') {
        switch (dir) {
            case North: return {{-1, 0}};
            case East: return {{-1, 0}, {1, 0}};
            case South: return {{1, 0}};
            case West: return {{-1, 0}, {1, 0}};
        };
    }

    if (currentTile == '-') {
        switch (dir) {
            case North: return {{0, -1}, {0, 1}};
            case East: return {{0, 1}};
            case South: return {{0, -1}, {0, 1}};
            case West: return {{0, -1}};
        };
    }

    cout << "Unknown coord delta for tile: " << currentTile << " and direction: " << dir << endl;
    exit(1);
}

vector<PositionData> _getNextPositions(const PositionData &currentPosition, const Input &input) {
    const auto currentTile = input.grid[currentPosition.coord.first][currentPosition.coord.second];

    vector<PositionData> nextPositions;
    const auto coordDeltas = _getCoordDeltas(currentPosition.dir, currentTile);
    for (const auto &delta : coordDeltas) {
        nextPositions.emplace_back(PositionData {
            .coord = { currentPosition.coord.first + delta.first, currentPosition.coord.second + delta.second },
            .dir = _getDirectionFromCoordDelta(delta),
        });
    }

    return nextPositions;
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

BFSOutput _bfs(const PositionData &startPosition, const Input &input) {
    BFSOutput output;

    queue<PositionData> positionsToVisit;

    set<PositionData> visitedPositions;

    positionsToVisit.push(startPosition);

    while (!positionsToVisit.empty()) {
        const auto currentPosition = positionsToVisit.front();
        positionsToVisit.pop();

        if (visitedPositions.contains(currentPosition)) {
            continue;
        }

        visitedPositions.insert(currentPosition);
        output.visitedCoords.insert(currentPosition.coord);

        const auto potentialNextPositions = _getNextPositions(currentPosition, input);
        for (const auto &potentialNextPosition : potentialNextPositions) {
            if (!_isValidCoord(potentialNextPosition.coord, input)) {
                output.visitedEdgeExitPositions.insert(currentPosition);
                continue;
            }

            positionsToVisit.push(potentialNextPosition);
        }
    }

    return output;
}

void part1(const Input &input) {
    const PositionData startPosition = {
        .coord = {0, 0},
        .dir = East,
    };

    const auto energizeOutput = _bfs(startPosition, input);
    const auto totalEnergizedTiles = energizeOutput.visitedCoords.size();

    cout << totalEnergizedTiles << endl;
}

void part2(const Input &input) {
    set<PositionData> visitedEdgeExitPositions;

    size_t maxEnergizedTiles = 0;
    for (size_t i = 0; i < input.nRows; i++) {
        const PositionData startPositionToEast = {
            .coord = {i, 0},
            .dir = East,
        };

        if (!visitedEdgeExitPositions.contains(startPositionToEast)) {
            auto energizeOutput = _bfs(startPositionToEast, input);
            maxEnergizedTiles = max(maxEnergizedTiles, energizeOutput.visitedCoords.size());
            visitedEdgeExitPositions.merge(energizeOutput.visitedEdgeExitPositions);
        }

        const PositionData startPositionToWest = {
            .coord = {i, input.nCols - 1},
            .dir = West,
        };

        if (!visitedEdgeExitPositions.contains(startPositionToWest)) {
            auto energizeOutput = _bfs(startPositionToWest, input);
            maxEnergizedTiles = max(maxEnergizedTiles, energizeOutput.visitedCoords.size());
            visitedEdgeExitPositions.merge(energizeOutput.visitedEdgeExitPositions);
        }
    }

    for (size_t i = 0; i < input.nCols; i++) {
        const PositionData startPositionToSouth = {
            .coord = {0, i},
            .dir = South,
        };

        if (!visitedEdgeExitPositions.contains(startPositionToSouth)) {
            auto energizeOutput = _bfs(startPositionToSouth, input);
            maxEnergizedTiles = max(maxEnergizedTiles, energizeOutput.visitedCoords.size());
            visitedEdgeExitPositions.merge(energizeOutput.visitedEdgeExitPositions);
        }

        const PositionData startPositionToNorth = {
            .coord = {input.nRows - 1, i},
            .dir = North,
        };

        if (!visitedEdgeExitPositions.contains(startPositionToNorth)) {
            auto energizeOutput = _bfs(startPositionToNorth, input);
            maxEnergizedTiles = max(maxEnergizedTiles, energizeOutput.visitedCoords.size());
            visitedEdgeExitPositions.merge(energizeOutput.visitedEdgeExitPositions);
        }
    }

    cout << maxEnergizedTiles << endl;
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