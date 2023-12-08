#include "../../util/StringUtil.h"
#include "../../util/MathUtil.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint>
#include <vector>
#include <regex>
#include <map>
#include <array>
#include <queue>
#include <set>
#include <optional>

using namespace std;

regex MAP_RX("([A-Z0-9]+) = \\(([A-Z0-9]+), ([A-Z0-9]+)\\)");

const string kStart = "AAA";
const string kEnd = "ZZZ";

struct Instructions {
    queue<size_t> directions;
    map<string, array<string, 2>> wastelandMap;
    vector<string> startNodes;
};

struct NodePath {
    string startNode;
    string currentNode;
    map<string, size_t> visitedNodes;
    size_t loopStart;
    size_t loopSize;
    size_t DEBUG_headSteps;
    size_t DEBUG_loopSteps;
    size_t DEBUG_tailSteps;
    size_t DEBUG_zLocation;
    bool foundEnd;
    bool foundLoop = false;
    size_t DEBUG_zPeriod;
};

const Instructions _parseInput(istream& inputFile) {
    Instructions instructions;

    bool processedDirections = false;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        if (line.size() == 0) {
            continue;
        }

        if (!processedDirections) {
            for (const auto dir : line) {
                switch (dir) {
                    case 'L':
                        instructions.directions.push(0); break;
                    case 'R':
                        instructions.directions.push(1); break;
                }
            }

            processedDirections = true;
            continue;
        }

        smatch mapMatch;
        if (!regex_match(line, mapMatch, MAP_RX)) {
            cout << "MAP_RX failed for line: { " << line << " }" << endl;
            exit(1);
        }

        const auto srcNode = mapMatch[1].str();
        instructions.wastelandMap[srcNode] = { mapMatch[2].str(), mapMatch[3].str() };
        if (srcNode.at(srcNode.size() - 1) == 'A') {
            instructions.startNodes.push_back(srcNode);
        }

    }

    return instructions;
}

const size_t _getNextDirection(queue<size_t> &directions) {
    const auto dir = directions.front();
    directions.pop();
    directions.push(dir);

    return dir;
}

void part1(const Instructions &instructions) {
    queue<size_t> directions = instructions.directions;

    if (!instructions.wastelandMap.contains(kStart)) {
        cout << "Map does not contain start node: " << kStart << endl;
        cout << "Skipping this part" << endl;
        return;
    }

    size_t steps = 0;
    string currentNode = kStart;
    while(true) {
        if (currentNode == kEnd) {
            break;
        }

        const auto dir = _getNextDirection(directions);

        currentNode = instructions.wastelandMap.at(currentNode)[dir];
        steps += 1;
    }

    cout << steps << endl;
}

const uint64_t _lcmLoopSizes(const vector<NodePath> &currentPaths) {
    vector<size_t> loopSizes;
    for (const auto &path : currentPaths) {
        loopSizes.push_back(path.loopSize);
    }

    return aoc::util::math::lcm(loopSizes);
}

void part2Debug(const Instructions &instructions) {
    vector<NodePath> currentPaths;
    for (const auto &startNode : instructions.startNodes) {
        NodePath np = {
            .startNode = startNode,
            .currentNode = startNode,
        };
        np.visitedNodes[startNode] = 0;

        currentPaths.push_back(np);
    }

    for (auto &nodePath : currentPaths) {
        size_t steps = 0;
        queue<size_t> directions = instructions.directions;

        // cout << "Path for: " << nodePath.startNode << endl;
        while (true) {
            // cout << nodePath.currentNode << " -> ";
            const auto dir = _getNextDirection(directions);

            if (nodePath.foundEnd) {
                nodePath.DEBUG_tailSteps += 1;
                nodePath.DEBUG_zPeriod += 1;
            }

            const auto nextNode = instructions.wastelandMap.at(nodePath.currentNode)[dir];
            // cout << nextNode << endl;
            if (nextNode[nextNode.size() - 1] == 'Z') {
                if (nodePath.foundEnd) {
                    break;
                }

                nodePath.DEBUG_zLocation = steps;
                nodePath.foundEnd = true;
            }

            if (nodePath.foundEnd && nodePath.visitedNodes.contains(nextNode)) {
                // cout << "!!! Found " << nextNode << endl;
                nodePath.foundLoop = true;
                nodePath.loopStart = nodePath.visitedNodes[nextNode];
                nodePath.loopSize = steps - nodePath.loopStart;

                nodePath.DEBUG_headSteps = nodePath.loopStart;
                nodePath.DEBUG_loopSteps = steps - nodePath.loopStart;
                // break;
            }

            steps += 1;
            nodePath.visitedNodes[nextNode] = steps;
            nodePath.currentNode = nextNode;
        }
    }

    for (const auto &np : currentPaths) {
        cout << np.startNode << " -> " << "headSteps: " << np.DEBUG_headSteps << ", loopSteps: " << np.DEBUG_loopSteps << ", tailSteps: " << np.DEBUG_tailSteps << ", zLocation: " << np.DEBUG_zLocation << ", zPeriod: " << np.DEBUG_zPeriod << endl;
        cout << endl;
    }
}

void part2(const Instructions &instructions) {
    vector<NodePath> currentPaths;
    for (const auto &startNode : instructions.startNodes) {
        NodePath np = {
            .startNode = startNode,
            .currentNode = startNode,
        };
        np.visitedNodes[startNode] = 0;

        currentPaths.push_back(np);
    }

    for (auto &nodePath : currentPaths) {
        size_t steps = 0;
        queue<size_t> directions = instructions.directions;

        while (true) {
            const auto dir = _getNextDirection(directions);

            const auto nextNode = instructions.wastelandMap.at(nodePath.currentNode)[dir];
            if (nextNode[nextNode.size() - 1] == 'Z') {
                nodePath.foundLoop = true;
                nodePath.loopSize = steps + 1;
                break;
            }

            steps += 1;
            nodePath.visitedNodes[nextNode] = steps;
            nodePath.currentNode = nextNode;
        }
    }

    // for (const auto &np : currentPaths) {
    //     cout << np.startNode << " -> " << "loopStart: " << np.loopStart << ", loopSize: " << np.loopSize << endl;
    // }

    const auto requiredSteps = _lcmLoopSizes(currentPaths);
    cout << requiredSteps << endl;
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        std::cout << "Must specify input file!" << endl;
        return 0;
    }

    std::cout << "Input file name: " << argv[1] << endl;
    ifstream inputFile(argv[1]);

    const auto instructions = _parseInput(inputFile);

    std::cout << "Part 1: " << endl;
    part1(instructions);

    std::cout << "Part 2: " << endl;
    part2(instructions);

    inputFile.close();

    return 0;
}