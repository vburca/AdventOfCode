#include "../../util/BasicIncludes.h"
#include "../../util/MathUtil.h"
#include "../../util/StringUtil.h"
#include "../../util/Types.h"

#include <map>
#include <stack>
#include <set>
#include <unordered_set>
#include <queue>

using namespace std;
using namespace aoc::util::types;

struct State {
    string node;
    vector<string> path;
    set<string> visitedNodes;
};

const vector<vector<string>> runDFS(
        const string &startNode,
        const string &endNode,
        const unordered_map<string, vector<string>> &adjacencyList) {
    stack<State> states;

    states.push({
        .node = startNode,
        .path = {startNode},
        .visitedNodes = {startNode}});

    vector<vector<string>> paths;
    while (!states.empty()) {
        const auto currentState = states.top();
        states.pop();

        if (currentState.node == endNode) {
            paths.push_back(currentState.path);
            continue;
        }

        if (!adjacencyList.contains(currentState.node)) {
            continue;
        }

        for (const auto &neighbor : adjacencyList.at(currentState.node)) {
            if (currentState.visitedNodes.contains(neighbor)) {
                continue;
            }

            State newState = currentState;
            newState.node = neighbor;
            newState.path.push_back(neighbor);
            newState.visitedNodes.insert(neighbor);
            states.push(newState);
        }
    }

    return paths;
}

void part1(istream& inputFile) {
    unordered_map<string, vector<string>> adjacencyList;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        const auto elements = aoc::util::string::split(line, ':');
        const auto startNode = elements[0];
        auto connectedDevicesStr = elements[1];
        aoc::util::string::trim(connectedDevicesStr);
        const auto connectedDevices = aoc::util::string::split(connectedDevicesStr, ' ');
        adjacencyList[startNode] = connectedDevices;
    }

    const auto allPaths = runDFS("you", "out", adjacencyList);
    cout << allPaths.size() << endl;
}

const size_t recursiveDFS(
    const string &currentNode,
    const unordered_map<string, vector<string>> &adjacencyList,
    unordered_map<string, size_t> &memoization) {
    if (memoization.contains(currentNode)) {
        return memoization[currentNode];
    }

    size_t numPaths = 0;
    for (const auto &neighborNode : adjacencyList.at(currentNode)) {
        numPaths += recursiveDFS(neighborNode, adjacencyList, memoization);
    }
    memoization[currentNode] = numPaths;
    return numPaths;
}

const size_t recursiveDFSWithFilter(
    const unordered_map<string, vector<string>> &adjacencyList,
    const vector<string> nodesToAvoid) {
    unordered_map<string, size_t> memoization;
    memoization["out"] = 1;

    for (const auto &node : nodesToAvoid) {
        memoization[node] = 0;
    }

    return recursiveDFS("svr", adjacencyList, memoization);
}

const size_t countPathsDFS(const unordered_map<string, vector<string>> &adjacencyList) {
/*
    Count paths from "svr" to "out" that visit both "dac" and "fft" at least once,
    using inclusion-exclusion principle:

    Let:
        A = paths that avoid DAC
        B = paths that avoid FFT

    We want paths that visit both DAC and FFT, i.e., paths NOT in (A ∪ B):

        ValidPaths = AllPaths - |A ∪ B|
                   = AllPaths - (|A| + |B| - |A ∩ B|)
                   = AllPaths - |A| - |B| + |A ∩ B|

    Here:
        - numPathsTotal       = total paths without restrictions
        - numPathsNoDAC       = paths avoiding DAC (|A|)
        - numPathsNoFFT       = paths avoiding FFT (|B|)
        - numPathsNoDACNoFFT  = paths avoiding both DAC and FFT (|A ∩ B|)

    So the final count is:
        numPathsTotal + numPathsNoDACNoFFT - numPathsNoDAC - numPathsNoFFT

    This approach allows using simple memoized DFS without explicitly tracking
    visited DAC/FFT flags along every path, which is much faster for large graphs.
*/
    const auto numPathsNoDAC = recursiveDFSWithFilter(adjacencyList, {"dac"});
    const auto numPathsNoFFT = recursiveDFSWithFilter(adjacencyList, {"fft"});
    const auto numPathsNoDACNoFFT = recursiveDFSWithFilter(adjacencyList, {"dac", "fft"});
    const auto numPathsTotal = recursiveDFSWithFilter(adjacencyList, {});

    return numPathsTotal + numPathsNoDACNoFFT - numPathsNoDAC - numPathsNoFFT;
}

void part2(istream& inputFile) {
    unordered_map<string, vector<string>> adjacencyList;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        const auto elements = aoc::util::string::split(line, ':');
        const auto startNode = elements[0];
        auto connectedDevicesStr = elements[1];
        aoc::util::string::trim(connectedDevicesStr);
        const auto connectedDevices = aoc::util::string::split(connectedDevicesStr, ' ');
        adjacencyList[startNode] = connectedDevices;
    }

    const auto numPaths = countPathsDFS(adjacencyList);
    cout << numPaths << endl;
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