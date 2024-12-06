#include "../../util/BasicIncludes.h"
#include "../../util/StringUtil.h"
#include "../../util/Types.h"

#include <vector>
#include <optional>
#include <set>

using namespace std;
using namespace aoc::util::types;

const vector<coord_t> ORIENTATIONS = {
    {-1, 0},  // up
    {0, 1},   // right
    {1, 0},   // down
    {0, -1},  // left
};

struct State {
    coord_t position;
    size_t orientationIdx;

    bool operator==(const State &other) const {
        return tie(position, orientationIdx) == tie(other.position, other.orientationIdx);
    }

    bool operator<(const State &other) const {
        return tie(position, orientationIdx) < tie(other.position, other.orientationIdx);
    }
};

struct TraversalResult {
    set<State> visitedStates;
    bool isLoop;

    const set<coord_t> getVisitedPositions() const {
        set<coord_t> visitedPositions;
        for (const auto &visitedState : visitedStates) {
            visitedPositions.insert(visitedState.position);
        }

        return visitedPositions;
    }
};

bool _isGuard(const char &ch) {
    const string guardOrientations = "^>v<";
    return guardOrientations.contains(ch);
}

const optional<size_t> _getOrientationIdx(const char &ch) {
    if (!_isGuard(ch)) {
        return nullopt;
    }

    switch (ch) {
        case '^': return 0;
        case '>': return 1;
        case 'v': return 2;
        case '<': return 3;
        default: return nullopt;
    }

    return nullopt;
}

bool _isWithinArea(const coord_t position, const size_t h, const size_t w) {
    return position.first >= 0 && position.first < h &&
            position.second >= 0 && position.second < w;
}

State _getNextValidPosition(const vector<string> &guardMap, const State &state) {
    size_t currentOrientationIdx = state.orientationIdx;

    coord_t currentOrientation = ORIENTATIONS[currentOrientationIdx];
    coord_t nextPosition = { state.position.first + currentOrientation.first, state.position.second + currentOrientation.second };

    const size_t kH = guardMap.size();
    const size_t kW = guardMap.front().size();

    while (_isWithinArea(nextPosition, kH, kW) && guardMap[nextPosition.first][nextPosition.second] == '#') {
        currentOrientationIdx = (currentOrientationIdx + 1) % ORIENTATIONS.size();
        currentOrientation = ORIENTATIONS[currentOrientationIdx];
        nextPosition = { state.position.first + currentOrientation.first, state.position.second + currentOrientation.second };
    }

    return { nextPosition, currentOrientationIdx };
}

const TraversalResult _traverseMap(const vector<string> &guardMap, const coord_t start, const size_t orientationIdx) {
    const size_t kH = guardMap.size();
    const size_t kW = guardMap.front().size();

    State nextState = {
        .position = start,
        .orientationIdx = orientationIdx };
    set<State> visitedStates;
    while (_isWithinArea(nextState.position, kH, kW)) {
        if (visitedStates.contains(nextState)) {
            // found loop
            return { .visitedStates = visitedStates,
                        .isLoop = true };
        }

        visitedStates.insert(nextState);
        // cout << "Visiting { " << nextState.position.first << ", " << nextState.position.second << " }" << endl;

        nextState = _getNextValidPosition(guardMap, nextState);
    }

    return { .visitedStates = visitedStates,
                .isLoop = false };
}


void part1(istream& inputFile) {
    vector<string> guardMap;

    bool foundStart = false;
    coord_t guardStart;
    size_t guardOrientationIdx;
    size_t row = 0;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        guardMap.push_back(line);

        if (!foundStart) {
            for (size_t col = 0; col < line.size(); col++) {
                const auto ch = line[col];
                const auto orientationIdxOpt = _getOrientationIdx(ch);
                if (!orientationIdxOpt) {
                    continue;
                }

                guardStart = { row, col };
                guardOrientationIdx = *orientationIdxOpt;
                foundStart = true;
                // cout << "Found guard: { " << guardStart.first << ", " << guardStart.second << " }" << endl;
            }
        }

        row += 1;
    }

    const auto result = _traverseMap(guardMap, guardStart, guardOrientationIdx);
    cout << result.getVisitedPositions().size() << endl;
}

void part2(istream& inputFile) {
    vector<string> guardMap;

    bool foundStart = false;
    coord_t guardStart;
    size_t guardOrientationIdx;
    size_t row = 0;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        guardMap.push_back(line);

        if (!foundStart) {
            for (size_t col = 0; col < line.size(); col++) {
                const auto ch = line[col];
                const auto orientationIdxOpt = _getOrientationIdx(ch);
                if (!orientationIdxOpt) {
                    continue;
                }

                guardStart = { row, col };
                guardOrientationIdx = *orientationIdxOpt;
                foundStart = true;
                // cout << "Found guard: { " << guardStart.first << ", " << guardStart.second << " }" << endl;
            }
        }

        row += 1;
    }

    const auto initialTraversal = _traverseMap(guardMap, guardStart, guardOrientationIdx);
    const auto initialVisitedPositions = initialTraversal.getVisitedPositions();
    size_t numPositionsThatCausedLoop = 0;
    vector<string> alteredGuardMap = guardMap;
    for (const auto &visitedPosition : initialVisitedPositions) {
        // do not place obstacle on the start position
        if (visitedPosition.first == guardStart.first && visitedPosition.second == guardStart.second) {
            continue;
        }

        // put obstactle at this initially visited position
        alteredGuardMap[visitedPosition.first][visitedPosition.second] = '#';

        const auto alteredTraversal = _traverseMap(alteredGuardMap, guardStart, guardOrientationIdx);
        numPositionsThatCausedLoop += alteredTraversal.isLoop;

        // unset the obstacle
        alteredGuardMap[visitedPosition.first][visitedPosition.second] = '.';
    }

    cout << numPositionsThatCausedLoop << endl;
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