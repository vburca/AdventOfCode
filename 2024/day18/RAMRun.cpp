#include "../../util/BasicIncludes.h"
#include "../../util/StringUtil.h"
#include "../../util/Types.h"
#include "../../util/MathUtil.h"
#include "../../util/Graph.h"

#include <vector>
#include <set>
#include <queue>

using namespace std;
using namespace aoc::util::types;
using namespace aoc::util::graph;
using namespace aoc::util::math;

size_t kMemorySpaceDim = 70;
size_t kNumBytesSimulate = 1024;

constexpr size_t kMemorySpaceDim_DEBUG = 6;
constexpr size_t kNumBytesSimulate_DEBUG = 12;

struct SearchState {
    coord_t position;
    vector<coord_t> path;
};

struct MemoryState {
    vector<coord_t> bytes;
    set<coord_t> fallenBytes;
    vector<vector<bool>> freeSpaceGrid = vector<vector<bool>>(kMemorySpaceDim + 1, vector<bool>(kMemorySpaceDim + 1, true));

    void dropBytes(const size_t numBytes) {
        for (size_t i = 0; i < numBytes; ++i) {
            const auto fallenByte = bytes[i];
            fallenBytes.insert(fallenByte);

            freeSpaceGrid[fallenByte.first][fallenByte.second] = false;
        }
    }
};

void _printMemoryState(const MemoryState &s, const bool printBytes = false, const optional<size_t> simFallenBytes = nullopt) {
    if (printBytes) {
        cout << "Bytes: " << endl;
        for (const auto &p : s.bytes) {
            cout << p.second << "," << p.first << endl;
        }
    }

    set<coord_t> fallenBytes;
    for (size_t i = 0; i < simFallenBytes; ++i) {
        fallenBytes.insert(s.bytes[i]);
    }

    cout << endl;
    for (size_t i = 0; i <= kMemorySpaceDim; ++i) {
        for (size_t j = 0; j <= kMemorySpaceDim; ++j) {
            const coord_t bytePos = {i, j};
            if (fallenBytes.contains(bytePos)) {
                cout << '#';
            } else {
                cout << '.';
            }
        }
        cout << endl;
    }
    cout << endl;
}

void _printPath(const MemoryState &s, const SearchState &searchState) {
    vector<vector<char>> displayGrid = vector<vector<char>>(s.freeSpaceGrid.size(), vector<char>(s.freeSpaceGrid.front().size(), '.'));

    for (const auto &p : s.fallenBytes) {
        displayGrid[p.first][p.second] = '#';
    }

    for (const auto &p : searchState.path) {
        if (displayGrid[p.first][p.second] != '.') {
            cout << "Path contains blocked coordinate: (" << p.first << "," << p.second << ")" << endl;
            exit(1);
        }

        displayGrid[p.first][p.second] = 'O';
    }

    for (size_t i = 0; i < displayGrid.size(); ++i) {
        for (size_t j = 0; j < displayGrid.front().size(); ++j) {
            cout << displayGrid[i][j];
        }
        cout << endl;
    }
    cout << endl;
}

const set<coord_t> _getValidNeighbors(const MemoryState &s, const coord_t currentPos) {
    set<coord_t> neighbors;

    coord_signed_t currentPosSigned = currentPos;
    for (const auto &dir : kCompassDirections4) {
        const coord_signed_t newPos = {currentPosSigned.first + dir.first, currentPosSigned.second + dir.second};
        if (newPos.first < 0 || newPos.first >= s.freeSpaceGrid.size() ||
            newPos.second < 0 || newPos.second >= s.freeSpaceGrid.front().size()) {
            continue;
        }

        neighbors.insert(newPos);
    }

    return neighbors;
}

const optional<SearchState> _shortestPathBFS(const MemoryState &s) {
    queue<SearchState> toVisit;

    const coord_t startPos = {0, 0};
    const coord_t endPos = {kMemorySpaceDim, kMemorySpaceDim};

    vector<vector<bool>> visited = vector<vector<bool>>(kMemorySpaceDim + 1, vector<bool>(kMemorySpaceDim + 1, false));

    auto startState = SearchState {
        .position = startPos,
        .path = {startPos},
    };

    toVisit.push(startState);

    optional<SearchState> shortestPathState = nullopt;
    while (!toVisit.empty()) {
        const auto currentState = toVisit.front();
        toVisit.pop();

        const auto currentPos = currentState.position;
        if (visited[currentPos.first][currentPos.second]) {
            continue;
        }
        visited[currentPos.first][currentPos.second] = true;

        if (currentPos == endPos) {
            shortestPathState = currentState;
            break;
        }

        const auto neighbors = _getValidNeighbors(s, currentPos);
        for (const auto &n : neighbors) {
            if (!s.freeSpaceGrid[n.first][n.second]) {
                continue;
            }

            if (visited[n.first][n.second]) {
                continue;
            }

            auto newState = SearchState {
                .position = n,
                .path = currentState.path,
            };
            newState.path.push_back(n);

            toVisit.push(newState);
        }
    }

    return shortestPathState;
}

const coord_t _findFirstBlockingByte(const MemoryState &s) {
    size_t startBlockIdx = 0;
    size_t endBlockIdx = s.bytes.size() - 1;

    while (startBlockIdx < endBlockIdx) {
        const size_t midDelta = (endBlockIdx - startBlockIdx) / 2;
        const size_t midBlockIdx = startBlockIdx + midDelta;
        MemoryState testState = s;
        testState.dropBytes(midBlockIdx + 1);

        const auto shortestPathOpt = _shortestPathBFS(testState);
        if (shortestPathOpt) {
            startBlockIdx = midBlockIdx + 1;
        } else {
            endBlockIdx = midBlockIdx;
        }
    }

    return s.bytes[startBlockIdx];
}

void part1(istream& inputFile) {
    MemoryState s;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        istringstream iss(line);
        coord_t pos;
        iss >> pos.second; // row
        iss.ignore();
        iss >> pos.first;  // col

        s.bytes.push_back(pos);
    }

    // _printMemoryState(s, false, kNumBytesSimulate);
    s.dropBytes(kNumBytesSimulate);

    const auto shortestPathStateOpt = _shortestPathBFS(s);
    if (!shortestPathStateOpt) {
        cout << "Did not find a shortest path!" << endl;
        exit(1);
    }
    const auto shortestPathState = *shortestPathStateOpt;
    // _printPath(s, shortestPathState);

    const auto numSteps = shortestPathState.path.size() - 1;
    cout << numSteps << endl;
}

void part2(istream& inputFile) {
    MemoryState s;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        istringstream iss(line);
        coord_t pos;
        iss >> pos.second; // row
        iss.ignore();
        iss >> pos.first;  // col

        s.bytes.push_back(pos);
    }

    const auto firstBlockingByte = _findFirstBlockingByte(s);

    // print it reversed because the problem treats x along columns, y along rows
    cout << firstBlockingByte.second << "," << firstBlockingByte.first << endl;
}

int main(int argc, char **argv) {
    if (argc <= 1)
    {
        std::cout << "Must specify input file!" << endl;
        return 0;
    }

    std::cout << "Input file name: " << argv[1] << endl;
    ifstream inputFile(argv[1]);

    string filenameStr(argv[1]);
    if (filenameStr.find("day18") == string::npos) {
        kMemorySpaceDim = kMemorySpaceDim_DEBUG;
        kNumBytesSimulate = kNumBytesSimulate_DEBUG;
    }

    std::cout << "Part 1: " << endl;
    part1(inputFile);

    inputFile.clear();
    inputFile.seekg(0, ios::beg);

    std::cout << "Part 2: " << endl;
    part2(inputFile);

    inputFile.close();

    return 0;
}