#include "../../util/BasicIncludes.h"
#include "../../util/StringUtil.h"
#include "../../util/Types.h"

#include <vector>
#include <stack>
#include <optional>
#include <set>

using namespace std;
using namespace aoc::util::types;

const vector<coord_t> DIRECTIONS = {
    {-1, 0},   // up
    {0, 1},    // right
    {1, 0},    // down
    {0, -1},   // left
};

struct HikeState {
    coord_t position;
    set<coord_t> visitedPositions;
    vector<coord_t> visitedPath;
};


void _printMap(const vector<vector<size_t>> &topographicMap) {
    for (const auto &row : topographicMap) {
        for (const auto &e : row) {
            cout << e << " ";
        }
        cout << endl;
    }
}

void _printState(const HikeState &state) {
    cout << "{" << state.position.first << "," << state.position.second << "} -> [ ";
    for (const auto &pos : state.visitedPath) {
        cout << "{" << pos.first << "," << pos.second << "} ";
    }

    cout << "]" << endl;
}

const vector<coord_t> _getNextPositions(const coord_t currentPosition, const size_t mapH, const size_t mapW) {
    vector<coord_t> nextPositions;

    coord_signed_t currentPositionSigned = currentPosition;
    for (const auto &direction : DIRECTIONS) {
        const coord_signed_t nextPosition = {currentPositionSigned.first + direction.first, currentPositionSigned.second + direction.second};
        if (nextPosition.first < 0 || nextPosition.first >= mapH ||
            nextPosition.second < 0 || nextPosition.second >= mapW) {
                continue;
            }
        nextPositions.push_back(nextPosition);
    }

    return nextPositions;
}

bool _isInclineGradual(const vector<vector<size_t>> &topographicMap, const coord_t currentPosition, const coord_t nextPosition) {
    return topographicMap[nextPosition.first][nextPosition.second] - topographicMap[currentPosition.first][currentPosition.second] == 1;
}

size_t _findTrailsDFS(const vector<vector<size_t>> &topographicMap, const coord_t startPosition, bool isPart2 = false) {
    stack<HikeState> hikeStates;

    hikeStates.emplace(HikeState {
        .position = startPosition,
        .visitedPositions = {startPosition},
        .visitedPath = {startPosition},
    });

    const auto mapH = topographicMap.size();
    const auto mapW = topographicMap.front().size();

    size_t completeTrails = 0;
    set<coord_t> reachedTrailends;
    while (!hikeStates.empty()) {
        const auto currentState = hikeStates.top();
        hikeStates.pop();

        if (topographicMap[currentState.position.first][currentState.position.second] == 9) {
            reachedTrailends.insert(currentState.position);
            completeTrails += 1;
            // _printState(currentState);
            continue;
        }

        const auto possibleNextPositions = _getNextPositions(currentState.position, mapH, mapW);
        for (const auto &nextPosition : possibleNextPositions) {
            if (currentState.visitedPositions.contains(nextPosition)) {
                continue;
            }

            if (!_isInclineGradual(topographicMap, currentState.position, nextPosition)) {
                continue;
            }

            if (!isPart2) {
                if (reachedTrailends.contains(nextPosition)) {
                    continue;
                }
            }

            auto nextState = HikeState {
                .position = nextPosition,
                .visitedPositions = currentState.visitedPositions,
                .visitedPath = currentState.visitedPath,
            };
            nextState.visitedPositions.insert(nextPosition);
            nextState.visitedPath.push_back(nextPosition);

            hikeStates.push(nextState);
        }
    }

    return completeTrails;
}

void part1(istream& inputFile) {
    vector<vector<size_t>> topographicMap;
    vector<coord_t> startPositions;

    size_t rowIdx = 0;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        vector<size_t> row;
        for (size_t colIdx = 0; colIdx < line.size(); ++colIdx) {
            const char e = line[colIdx] - '0';

            if (e == 0) {
                startPositions.push_back({rowIdx, colIdx});
            }

            row.push_back(e);
        }

        topographicMap.push_back(row);
        rowIdx += 1;
    }

    // _printMap(topographicMap);

    size_t trailheadScoreSum = 0;
    for (const auto &startPosition : startPositions) {
        // cout << "trailhead={" << startPosition.first << "," << startPosition.second << "}" << endl;
        const auto trailheadScore = _findTrailsDFS(topographicMap, startPosition);
        // cout << "{" << startPosition.first << "," << startPosition.second << "} -> " << trailheadScore << endl;
        // cout << endl;

        trailheadScoreSum += trailheadScore;
    }

    cout << trailheadScoreSum << endl;
}

void part2(istream& inputFile) {
    vector<vector<size_t>> topographicMap;
    vector<coord_t> startPositions;

    size_t rowIdx = 0;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        vector<size_t> row;
        for (size_t colIdx = 0; colIdx < line.size(); ++colIdx) {
            const char e = line[colIdx] - '0';

            if (e == 0) {
                startPositions.push_back({rowIdx, colIdx});
            }

            row.push_back(e);
        }

        topographicMap.push_back(row);
        rowIdx += 1;
    }

    // _printMap(topographicMap);

    size_t trailheadScoreSum = 0;
    for (const auto &startPosition : startPositions) {
        // cout << "trailhead={" << startPosition.first << "," << startPosition.second << "}" << endl;
        const auto trailheadScore = _findTrailsDFS(topographicMap, startPosition, true);
        // cout << "{" << startPosition.first << "," << startPosition.second << "} -> " << trailheadScore << endl;
        // cout << endl;

        trailheadScoreSum += trailheadScore;
    }

    cout << trailheadScoreSum << endl;
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