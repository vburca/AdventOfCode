#include "../../util/BasicIncludes.h"
#include "../../util/StringUtil.h"
#include "../../util/Types.h"
#include "../../util/MathUtil.h"
#include "../../util/Graph.h"

#include <cstdint>
#include <vector>
#include <queue>
#include <set>

using namespace std;
using namespace aoc::util::types;
using namespace aoc::util::graph;
using namespace aoc::util::math;

using VULL = vector<size_t>;
using VVULL = vector<VULL>;
using VVVULL = vector<VVULL>;

struct LowestScorePathState {
    coord_t coord;
    kCompass4Orientation orientation;
    size_t score;
    bool isValid = false;

    bool operator<(const LowestScorePathState &other) const {
        return std::tie(coord, orientation) < std::tie(other.coord, other.orientation);
    }
};

struct LowestScorePathPrevStates {
    vector<LowestScorePathState> prevs;
};

using VPS = vector<LowestScorePathPrevStates>;
using VVPS = vector<VPS>;
using VVVPS = vector<VVPS>;

struct NeighborState {
    coord_t coord;
    kCompass4Orientation orientation;
    size_t cost;

    bool operator<(const NeighborState &other) const {
        return std::tie(coord, orientation) < std::tie(other.coord, other.orientation);
    }
};

// NOTE: https://en.cppreference.com/w/cpp/container/priority_queue
// Note that the Compare parameter is defined such that it returns true if its first
// argument comes before its second argument in a weak ordering. But because the priority
// queue outputs largest elements first, the elements that "come before" are actually
// output last. That is, the front of the queue contains the "last" element according to
// the weak ordering imposed by Compare.
struct LowestScorePathStateCompare
{
    bool operator()(const LowestScorePathState& s1, const LowestScorePathState& s2) const
    {
        return s1.score > s2.score;
    }
};

typedef priority_queue<LowestScorePathState, vector<LowestScorePathState>, LowestScorePathStateCompare> lowest_score_pq_t;

struct MazeState {
    vector<string> maze;
    coord_t startPosition;
    coord_t endPosition;

    kCompass4Orientation orientation;
    coord_t position;

    const size_t rows() const {
        return maze.size();
    }

    const size_t cols() const {
        return maze.front().size();
    }
};

const char _getCharFromOrientation(const kCompass4Orientation orientation) {
    switch (orientation) {
        case kCompass4Orientation::kNorth: return '^';
        case kCompass4Orientation::kEast: return '>';
        case kCompass4Orientation::kSouth: return 'v';
        case kCompass4Orientation::kWest: return '<';
        default: cout << "Unknown orientation= " << orientation << endl; exit(1);
    }
}

void _printPathState(const LowestScorePathState &ps) {
    cout << "pathState=[(" << ps.coord.first << "," << ps.coord.second << "), o=" << ps.orientation << " s=" << ps.score << "]" << endl;
}

void _printNeighbors(const set<NeighborState> &validNeighbors) {
    cout << "neighbors:" << endl;
    for (const auto &neighbor : validNeighbors) {
        cout << "coord=(" << neighbor.coord.first << "," << neighbor.coord.second << ") o=" << neighbor.orientation << endl;
    }
}

void _printState(const MazeState &s, bool printStartEnd = true) {
    cout << "start=(" << s.startPosition.first << "," << s.startPosition.second << ")" << endl;
    cout << "end=(" << s.endPosition.first << "," << s.endPosition.second << ")" << endl;
    cout << "pos=(" << s.position.first << "," << s.position.second << ")" << endl;
    const auto orientationVect = kCompassDirections4[s.orientation];
    cout << "orientation=(" << orientationVect.first << "," << orientationVect.second << ")" << endl;
    cout << endl;

    for (size_t i = 0; i < s.maze.size(); ++i) {
        for (size_t j = 0; j < s.maze.front().size(); ++j) {
            const coord_t currentPos = {i, j};
            if (printStartEnd && currentPos == s.startPosition) {
                cout << "S";
            } else if (printStartEnd && currentPos == s.endPosition) {
                cout << "E";
            } else {
                cout << s.maze[i][j];
            }
        }
        cout << endl;
    }
    cout << endl;
}

set<NeighborState> _getNeighbors(const LowestScorePathState &state, const MazeState &s) {
    constexpr size_t rotateCost = 1000;
    constexpr size_t moveCost = 1;
    // cout << "getting neighbors..." << endl;
    vector<pair<size_t, kCompass4Orientation>> weightedNeighborOrientations = {
        {rotateCost + moveCost, (kCompass4Orientation)positiveModulo((int)state.orientation - 1, kCompass4Orientation::kCount)},    // direction 90 degrees to the left (rotate + move cost)
        {moveCost, (kCompass4Orientation)(state.orientation)},                                                                      // direction forward (move cost)
        {rotateCost + moveCost, (kCompass4Orientation)positiveModulo((int)state.orientation + 1, kCompass4Orientation::kCount)},    // direction 90 degrees to the right (rotate + move cost)
    };

    // cout << "validating neighbors..." << endl;
    set<NeighborState> validNeighbors;

    const coord_signed_t currentCoord_Signed = state.coord;
    for (const auto &p : weightedNeighborOrientations) {
        const auto cost = p.first;
        const auto orientation = p.second;
        // cout << "validating {o=" << orientation << ", cost=" << cost << "}" << endl;
        const auto direction = kCompassDirections4[orientation];
        const coord_signed_t nextCoord = {currentCoord_Signed.first + direction.first, currentCoord_Signed.second + direction.second};

        // check bounds
        if (nextCoord.first < 0 || nextCoord.first >= s.rows() ||
            nextCoord.second < 0 || nextCoord.second >= s.cols()) {
            continue;
        }

        // check wall
        if (s.maze[nextCoord.first][nextCoord.second] == '#') {
            continue;
        }

        // this is a valid neighbor
        validNeighbors.emplace(NeighborState {
            .coord = nextCoord,
            .orientation = orientation,
            .cost = cost,
        });
    }

    return validNeighbors;
}

const pair<VVVULL, VVVPS> _dijkstra(const MazeState &s) {
//  1  function Dijkstra(Graph, source):
//  2
//  3      for each vertex v in Graph.Vertices:
//  4          dist[v] ← INFINITY
//  5          prev[v] ← UNDEFINED
//  6          add v to Q
//  7      dist[source] ← 0
//  8
//  9      while Q is not empty:
// 10          u ← vertex in Q with minimum dist[u]
// 11          remove u from Q
// 12
// 13          for each neighbor v of u still in Q:
// 14              alt ← dist[u] + Graph.Edges(u, v)
// 15              if alt < dist[v]:
// 16                  dist[v] ← alt
// 17                  prev[v] ← u
// 18
// 19      return dist[], prev[]

    // scores[coordX][coordY][orientation]
    VVVULL scores = VVVULL(s.rows(), VVULL(s.cols(), VULL(kCompass4Orientation::kCount, UINT64_MAX)));
    VVVPS prev = VVVPS(s.rows(), VVPS(s.cols(), VPS(kCompass4Orientation::kCount, LowestScorePathPrevStates{})));


    lowest_score_pq_t statesToVisit;
    scores[s.startPosition.first][s.startPosition.second][s.orientation] = 0;
    statesToVisit.emplace(LowestScorePathState {
        .coord = s.startPosition,
        .orientation = s.orientation,
        .score = 0,
        .isValid = true,
    });

    while (!statesToVisit.empty()) {
        const auto currentState = statesToVisit.top();
        statesToVisit.pop();

        // _printPathState(currentState);

        // check if we already have a fastest path to get here
        if (currentState.score > scores[currentState.coord.first][currentState.coord.second][currentState.orientation]) {
            continue;
        }

        const auto neighbors = _getNeighbors(currentState, s);
        // _printNeighbors(neighbors);
        for (auto neighbor : neighbors) {
            const auto directScore = scores[currentState.coord.first][currentState.coord.second][currentState.orientation] + neighbor.cost;
            if (directScore <= scores[neighbor.coord.first][neighbor.coord.second][neighbor.orientation]) {
                if (directScore < scores[neighbor.coord.first][neighbor.coord.second][neighbor.orientation]) {
                    prev[neighbor.coord.first][neighbor.coord.second][neighbor.orientation].prevs.clear();
                }

                scores[neighbor.coord.first][neighbor.coord.second][neighbor.orientation] = directScore;
                prev[neighbor.coord.first][neighbor.coord.second][neighbor.orientation].prevs.push_back(currentState);

                LowestScorePathState newState = {
                    .coord = neighbor.coord,
                    .orientation = neighbor.orientation,
                    .score = directScore,
                    .isValid = true,
                };
                // cout << "adding state: " << endl;
                // _printPathState(newState);
                statesToVisit.push(newState);
            }
        }

        // cout << endl;
    }

    return {scores, prev};
}

const pair<size_t, vector<LowestScorePathState>> _findLowestScorePath(const MazeState &s) {
    size_t lowestScore = UINT64_MAX;

    const auto result = _dijkstra(s);
    const auto scores = result.first;
    const auto prev = result.second;

    size_t bestEndOrientation = 0;
    for (size_t orientation = 0; orientation < kCompass4Orientation::kCount; ++orientation) {
        if (scores[s.endPosition.first][s.endPosition.second][orientation] < scores[s.endPosition.first][s.endPosition.second][bestEndOrientation]) {
            bestEndOrientation = orientation;
        }

        lowestScore = min(lowestScore, scores[s.endPosition.first][s.endPosition.second][orientation]);
    }

    vector<LowestScorePathState> reconstructedPath;
    LowestScorePathState backtrackingState = prev[s.endPosition.first][s.endPosition.second][bestEndOrientation].prevs.front();
    // size_t debugPrint = 0;
    while (!prev[backtrackingState.coord.first][backtrackingState.coord.second][backtrackingState.orientation].prevs.empty()) {
        // _printPathState(backtrackingState);
        // debugPrint += 1;
        // if (debugPrint >= 55) {
        //     exit(1);
        // }
        reconstructedPath.push_back(backtrackingState);
        backtrackingState = prev[backtrackingState.coord.first][backtrackingState.coord.second][backtrackingState.orientation].prevs.front();
    }

    reverse(reconstructedPath.begin(), reconstructedPath.end());

    return {lowestScore, reconstructedPath};
}

const set<coord_t> _findAllLowestScorePaths(const MazeState &s) {
    size_t lowestScore = UINT64_MAX;

    const auto result = _dijkstra(s);
    const auto scores = result.first;
    const auto prev = result.second;

    size_t bestEndOrientation = 0;
    for (size_t orientation = 0; orientation < kCompass4Orientation::kCount; ++orientation) {
        if (scores[s.endPosition.first][s.endPosition.second][orientation] < scores[s.endPosition.first][s.endPosition.second][bestEndOrientation]) {
            bestEndOrientation = orientation;
        }

        lowestScore = min(lowestScore, scores[s.endPosition.first][s.endPosition.second][orientation]);
    }

    // push all path endings that got the lowest score
    queue<LowestScorePathState> pathsToVisit;
    for (size_t orientation = 0; orientation < kCompass4Orientation::kCount; ++orientation) {
        if (scores[s.endPosition.first][s.endPosition.second][orientation] == lowestScore) {
            for (const auto &p : prev[s.endPosition.first][s.endPosition.second][orientation].prevs) {
                pathsToVisit.push(p);
            }
        }
    }

    set<coord_t> lowestScorePaths;
    set<LowestScorePathState> visitedScorePaths;
    while (!pathsToVisit.empty()) {
        const auto currentBacktrackingState = pathsToVisit.front();
        pathsToVisit.pop();

        if (visitedScorePaths.contains(currentBacktrackingState)) {
            continue;
        }
        visitedScorePaths.insert(currentBacktrackingState);

        // mark this position as visited
        lowestScorePaths.insert(currentBacktrackingState.coord);

        // find all the other paths that got here with the same score (through a different orientation, for example)
        const auto currentCoord = currentBacktrackingState.coord;
        const auto targetScore = scores[currentCoord.first][currentCoord.second][currentBacktrackingState.orientation];
        for (size_t orientation = 0; orientation < kCompass4Orientation::kCount; ++orientation) {
            if (scores[currentCoord.first][currentCoord.second][orientation] == targetScore) {
                for (const auto &p : prev[currentCoord.first][currentCoord.second][orientation].prevs) {
                    pathsToVisit.push(p);
                }
            }
        }
    }

    lowestScorePaths.insert(s.startPosition);
    lowestScorePaths.insert(s.endPosition);

    return lowestScorePaths;
}

void part1(istream& inputFile) {
    MazeState s;

    bool foundStart = false;
    bool foundEnd = false;
    size_t rowIdx = 0;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        if (!foundStart) {
            const auto idx = line.find('S');
            if (idx != string::npos) {
                const size_t colIdx = idx;
                s.startPosition = {rowIdx, colIdx};
                line[s.startPosition.second] = '.';
                foundStart = true;
            }
        }

        if (!foundEnd) {
            const auto idx = line.find('E');
            if (idx != string::npos) {
                const size_t colIdx = idx;
                s.endPosition = {rowIdx, colIdx};
                line[s.endPosition.second] = '.';
                foundEnd = true;
            }
        }

        s.maze.push_back(line);
        rowIdx += 1;
    }

    s.position = s.startPosition;
    s.orientation = kCompass4Orientation::kEast;

    // _printState(s);

    const auto result = _findLowestScorePath(s);
    const auto lowestScore = result.first;
    const auto lowestScorePath = result.second;

    // MazeState mazeStateWithLowestScorePath = s;
    // for (const auto &pathState : lowestScorePath) {
    //     mazeStateWithLowestScorePath.maze[pathState.coord.first][pathState.coord.second] = _getCharFromOrientation(pathState.orientation);
    // }

    // _printState(mazeStateWithLowestScorePath);

    cout << lowestScore << endl;
}

void part2(istream& inputFile) {
    MazeState s;

    bool foundStart = false;
    bool foundEnd = false;
    size_t rowIdx = 0;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        if (!foundStart) {
            const auto idx = line.find('S');
            if (idx != string::npos) {
                const size_t colIdx = idx;
                s.startPosition = {rowIdx, colIdx};
                line[s.startPosition.second] = '.';
                foundStart = true;
            }
        }

        if (!foundEnd) {
            const auto idx = line.find('E');
            if (idx != string::npos) {
                const size_t colIdx = idx;
                s.endPosition = {rowIdx, colIdx};
                line[s.endPosition.second] = '.';
                foundEnd = true;
            }
        }

        s.maze.push_back(line);
        rowIdx += 1;
    }

    s.position = s.startPosition;
    s.orientation = kCompass4Orientation::kEast;

    // _printState(s);

    const auto lowestScorePaths = _findAllLowestScorePaths(s);

    MazeState updatedMazeState = s;
    for (const auto &coord : lowestScorePaths) {
        updatedMazeState.maze[coord.first][coord.second] = 'O';
    }

    // _printState(updatedMazeState, false);

    cout << lowestScorePaths.size() << endl;
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