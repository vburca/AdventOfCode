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
#include <map>
#include <stack>
#include <set>
#include <queue>

using namespace std;
using namespace aoc::util::types;

struct Input {
    vector<string> hikingTrails;

    coord_t startPosition;
    coord_t endPosition;

    size_t nRows;
    size_t nCols;
};

struct SearchState {
    coord_t position;
    size_t hikeLength;
    set<coord_t> visitedPositions;
    vector<coord_t> previousPositions;
};

struct GraphEdge {
    coord_t startPosition;
    coord_t endPosition;
    size_t length;

    bool operator<(const GraphEdge &other) const {
        return tie(startPosition, endPosition, length) < tie(other.startPosition, other.endPosition, other.length);
    }
};

void _printMatrix(const vector<vector<size_t>> &mat) {
    for (const auto &row : mat) {
        for (const auto &e : row) {
            printf("%3zu", e);
        }
        cout << endl;
    }
    cout << endl;
}

void _printEdgeMap(const map<coord_t, map<coord_t, size_t>> &edges) {
    for (const auto [src, dsts] : edges) {
        cout << "(" << src.first << ", " << src.second << "): {" << endl;
        for (const auto [dst, length] : dsts) {
            cout << "  (" << dst.first << ", " << dst.second << ") -> " << length << endl;
        }
        cout << "}" << endl;
    }
}

void _printInput(const Input &input) {
    cout << "Hiking trails: (" << input.nRows << ", " << input.nCols << ")" << endl;
    for (const auto &line : input.hikingTrails) {
        cout << line << endl;
    }
    cout << endl;

    cout << "Start position: (" << input.startPosition.first << ", " << input.startPosition.second << ")" << endl;
    cout << "End position: (" << input.endPosition.first << ", " << input.endPosition.second << ")" << endl;
}

const Input _parseInput(istream& inputFile) {
    Input input;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        input.hikingTrails.push_back(line);
    }

    input.nRows = input.hikingTrails.size();
    input.nCols = input.hikingTrails.front().size();

    const auto startPosIt = find(input.hikingTrails.front().begin(), input.hikingTrails.front().end(), '.');
    input.startPosition = {0, startPosIt - input.hikingTrails.front().begin()};

    const auto endPosIt = find(input.hikingTrails.back().begin(), input.hikingTrails.back().end(), '.');
    input.endPosition = {input.nRows - 1, endPosIt - input.hikingTrails.back().begin()};

    return input;
}

bool _isValidPosition(const coord_t &position, const Input &input) {
    return position.first >= 0 && position.first < input.nRows &&
            position.second >= 0 && position.second < input.nCols &&
            input.hikingTrails[position.first][position.second] != '#';
}

coord_t _getSlopeNextPosition(const char slopeType, const coord_t &slopePosition) {
    switch (slopeType) {
        case '>': return coord_t{slopePosition.first, slopePosition.second + 1};
        case '<': return coord_t{slopePosition.first, slopePosition.second - 1};
        case '^': return coord_t{slopePosition.first - 1, slopePosition.second};
        case 'v': return coord_t{slopePosition.first + 1, slopePosition.second};
        default: {
            cout << "Unknown slope type: " << slopeType << " at position: (" << slopePosition.first << ", " << slopePosition.second << ")" << endl;
            exit(1);
        }
    }

    return {0, 0};
}

vector<coord_t> _getNextPositions(const coord_t &currentPosition, const Input &input, const bool useSlopes = true) {
    vector<coord_t> possibleNextPositions;
    const auto tile = input.hikingTrails[currentPosition.first][currentPosition.second];

    if (useSlopes && (tile == '<' || tile == '>' || tile == '^' || tile == 'v')) {
        possibleNextPositions.push_back(_getSlopeNextPosition(tile, currentPosition));
    } else {
        possibleNextPositions.push_back({currentPosition.first - 1, currentPosition.second}); // up
        possibleNextPositions.push_back({currentPosition.first, currentPosition.second + 1}); // right
        possibleNextPositions.push_back({currentPosition.first + 1, currentPosition.second}); // down
        possibleNextPositions.push_back({currentPosition.first, currentPosition.second - 1}); // left
    }


    vector<coord_t> validNextPositions;
    for (const auto &possibleNextPosition : possibleNextPositions) {
        if (!_isValidPosition(possibleNextPosition, input)) {
            continue;
        }

        validNextPositions.push_back(possibleNextPosition);
    }

    return validNextPositions;
}

vector<vector<size_t>> _longestHikesDFS(const coord_t &startPosition, const Input &input, const bool useSlopes = true) {
    stack<SearchState> statesToVisit;

    SearchState startState = {
        .position = startPosition,
        .hikeLength = 0,
    };

    statesToVisit.push(startState);

    vector<vector<size_t>> longestHikeLengths = vector<vector<size_t>>(input.nRows, vector<size_t>(input.nCols, 0));

    while (!statesToVisit.empty()) {
        const auto currentState = statesToVisit.top();
        statesToVisit.pop();

        // cout << "Visiting: (" << currentState.position.first << ", " << currentState.position.second << ")" << endl;

        auto &previousHikeLength = longestHikeLengths[currentState.position.first][currentState.position.second];
        if (currentState.position != startPosition &&
            currentState.hikeLength <= previousHikeLength &&
            useSlopes) {
            // end early, we got through this position through a longer hike already
            continue;
        }

        previousHikeLength = max(previousHikeLength, currentState.hikeLength);

        const auto neighborPositions = _getNextPositions(currentState.position, input, useSlopes);
        for (const auto &neighborPosition : neighborPositions) {
            if (currentState.visitedPositions.contains(neighborPosition)) {
                continue;
            }

            SearchState nextState = {
                .position = neighborPosition,
                .hikeLength = currentState.hikeLength + 1,
                .previousPositions = currentState.previousPositions,
                .visitedPositions = currentState.visitedPositions,
            };
            nextState.previousPositions.push_back(currentState.position);
            nextState.visitedPositions.insert(currentState.position);

            statesToVisit.push(nextState);
        }
    }

    return longestHikeLengths;
}

void part1(const Input &input) {
    const auto longestHikes = _longestHikesDFS(input.startPosition, input);
    const auto longestHikeToExit = longestHikes[input.endPosition.first][input.endPosition.second];

    // _printMatrix(longestHikes);

    cout << longestHikeToExit << endl;
}

set<GraphEdge> _findEdgeLengthsBFS(const coord_t &startPosition, const set<coord_t> &reducedGraphPositions, const Input &input) {
    SearchState startState = {
        .position = startPosition,
        .hikeLength = 0,
    };

    queue<SearchState> toVisit;
    toVisit.push(startState);

    set<coord_t> visited;
    visited.insert(startState.position);

    set<GraphEdge> edges;
    while (!toVisit.empty()) {
        const auto currentState = toVisit.front();
        toVisit.pop();

        if (reducedGraphPositions.contains(currentState.position)) {
            edges.insert(GraphEdge {
                .startPosition = startPosition,
                .endPosition = currentState.position,
                .length = currentState.hikeLength,
            });
            continue;
        }

        const auto neighborPositions = _getNextPositions(currentState.position, input, false);
        for (const auto &neighborPosition : neighborPositions) {
            if (visited.contains(neighborPosition)) {
                continue;
            }

            SearchState nextState = {
                .position = neighborPosition,
                .hikeLength = currentState.hikeLength + 1,
            };

            visited.insert(neighborPosition);
            toVisit.push(nextState);
        }
    }

    return edges;
}

void _longestHikesReducedGraphDFSRecursive(const coord_t currentPosition,
                                            const size_t hikeLength,
                                            const map<coord_t, map<coord_t, size_t>> &edges,
                                            map<coord_t, size_t> &longestHikes,
                                            map<coord_t, bool> &visited)
{
    if (visited[currentPosition]) {
        return;
    }

    visited[currentPosition] = true;

    longestHikes[currentPosition] = max(longestHikes[currentPosition], hikeLength);

    for (const auto &p : edges.at(currentPosition)) {
        const auto nextPosition = p.first;
        const auto length = p.second;

        _longestHikesReducedGraphDFSRecursive(nextPosition, hikeLength + length, edges, longestHikes, visited);
    }

    visited[currentPosition] = false;
}

map<coord_t, size_t> _longestHikesReducedGraphDFS(const coord_t &startPosition, const Input &input, const map<coord_t, map<coord_t, size_t>> edges) {
    SearchState startState = {
        .position = startPosition,
        .hikeLength = 0,
    };

    stack<SearchState> toVisit;
    toVisit.push(startState);

    map<coord_t, size_t> longestHikeLengths;
    while (!toVisit.empty()) {
        const auto currentState = toVisit.top();
        toVisit.pop();

        // if (currentState.position == coord_t{83, 61}) {
        //     cout << currentState.position.first << ", " << currentState.position.second << " visited: " << currentState.visitedPositions.size() << endl;
        //     cout << "{ " << endl;
        //     for (const auto &v : currentState.visitedPositions) {
        //         cout << "(" << v.first << ", " << v.second << ") ";
        //     }
        //     cout << "}" << endl << endl;
        // }

        if (!longestHikeLengths.contains(currentState.position)) {
            longestHikeLengths[currentState.position] = currentState.hikeLength;
        } else {
            auto &currentLength = longestHikeLengths[currentState.position];
            currentLength = max(currentLength, currentState.hikeLength);
        }

        for (const auto &p : edges.at(currentState.position)) {
            const auto neighborPosition = p.first;
            const auto neighborEdgeLength = p.second;

            if (currentState.visitedPositions.contains(neighborPosition)) {
                continue;
            }

            SearchState nextState = {
                .position = neighborPosition,
                .hikeLength = currentState.hikeLength + neighborEdgeLength,
                .visitedPositions = currentState.visitedPositions,
            };
            nextState.visitedPositions.insert(currentState.position);

            toVisit.push(nextState);
        }
    }

    return longestHikeLengths;
}

void part2(const Input &input) {
    // construct a reduced graph that will contain:
    // * start position
    // * end position
    // * any position that has more than 2 edges (i.e. is an intersection node)
    // * the edges from any such positions will have a "weight" equal to the longest path between them

    // find intersections in the graph
    set<coord_t> reducedGraphPositions;
    for (size_t row = 0; row < input.nRows; row++) {
        for (size_t col = 0; col < input.nCols; col++) {
            if (input.hikingTrails[row][col] != '.') {
                continue;
            }

            const auto currentPosition = coord_t{row, col};
            const auto nextPositions = _getNextPositions(currentPosition, input, false);
            if (nextPositions.size() > 2) {
                reducedGraphPositions.insert(currentPosition);
            }
        }
    }

    // insert start and end positions in the reduced graph
    reducedGraphPositions.insert(input.startPosition);
    reducedGraphPositions.insert(input.endPosition);

    map<coord_t, map<coord_t, size_t>> edgeToLength;
    for (const auto &graphPosition : reducedGraphPositions) {
        auto remainingGraphPositions = reducedGraphPositions;
        remainingGraphPositions.erase(graphPosition);
        const auto edges = _findEdgeLengthsBFS(graphPosition, remainingGraphPositions, input);
        for (const auto &edge : edges) {
            edgeToLength[edge.startPosition][edge.endPosition] = edge.length;
            edgeToLength[edge.endPosition][edge.startPosition] = edge.length;
        }
    }

    cout << "Searching largest path..." << endl;

    // const auto longestHikes = _longestHikesReducedGraphDFS(input.startPosition, input, edgeToLength);
    // if (!longestHikes.contains(input.endPosition)) {
    //     cout << "Longest hike to end position: (" << input.endPosition.first << ", " << input.endPosition.second << ") not found!" << endl;
    //     exit(1);
    // }

    // const auto longestHikeToExit = longestHikes.at(input.endPosition);

    // cout << longestHikeToExit << endl;

    // map<coord_t, size_t> max_paths;
    // map<coord_t, bool>   visited;
    // visit2(edgeToLength, max_paths, input.startPosition, 0, visited);

    // cout << max_paths[input.endPosition] << endl;

    map<coord_t, size_t> longestHikes;
    map<coord_t, bool> visited;
    _longestHikesReducedGraphDFSRecursive(input.startPosition, 0, edgeToLength, longestHikes, visited);

    cout << longestHikes[input.endPosition] << endl;
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
    // part1(input);

    std::cout << "Part 2: " << endl;
    part2(input);

    inputFile.close();

    return 0;
}