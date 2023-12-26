#include "../../util/StringUtil.h"
#include "../../util/MathUtil.h"
#include "../../util/Types.h"
#include "../../util/Graph.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint>
#include <vector>
#include <regex>
#include <map>
#include <set>

using namespace std;
using namespace aoc::util::types;

regex COMPONENTS_RX("([a-z]+): (.*)");

struct Input {
    map<string, set<string>> adjacencyList;
    vector<string> nodes;
    vector<vector<int>> adjacencyMatrix;
    map<string, size_t> nodeToIdx;
};

void _printInput(const Input &input) {
    cout << "Adjacency list: (" << input.adjacencyList.size() << ")" << endl;
    for (const auto &p : input.adjacencyList) {
        const auto &src = p.first;
        const auto &dsts = p.second;

        cout << src << ": ";
        for (const auto &dst : dsts) {
            cout << dst << " ";
        }
        cout << endl;
    }
    cout << endl;

    cout << "Nodes list: (" << input.nodes.size() << ")" << endl;
    for (const auto &node : input.nodes) {
        cout << node << " ";
    }
    cout << endl << endl;

    cout << "Adjacency matrix: (" << input.adjacencyMatrix.size() << ", " << input.adjacencyMatrix.front().size() << ")" << endl;
    for (const auto &row : input.adjacencyMatrix) {
        for (const auto &e : row) {
            cout << e << " ";
        }
        cout << endl;
    }
    cout << endl;
}

const Input _parseInput(istream& inputFile) {
    Input input;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        smatch componentsMatch;
        if (!regex_match(line, componentsMatch, COMPONENTS_RX)) {
            cout << "COMPONENTS_RX failed to match line: { " << line << " }" << endl;
            exit(1);
        }

        const auto src = componentsMatch[1].str();
        const auto dsts = aoc::util::string::split(componentsMatch[2].str(), ' ');
        for (const auto &dst : dsts) {
            input.adjacencyList[src].insert(dst);
            input.adjacencyList[dst].insert(src);
        }
    }

    size_t nodeIdx = 0;
    for (const auto &p : input.adjacencyList) {
        const auto &node = p.first;
        input.nodes.push_back(node);
        input.nodeToIdx[node] = nodeIdx;
        nodeIdx += 1;
    }

    input.adjacencyMatrix = vector<vector<int>>(input.nodes.size(), vector<int>(input.nodes.size(), 0));
    for (const auto &p : input.adjacencyList) {
        const auto &srcNode = p.first;
        const auto &dstNodes = p.second;

        const auto srcNodeIdx = input.nodeToIdx[srcNode];
        for (const auto &dstNode : dstNodes) {
            const auto dstNodeIdx = input.nodeToIdx[dstNode];
            input.adjacencyMatrix[srcNodeIdx][dstNodeIdx] = 1;
        }
    }

    return input;
}

void part1(const Input &input) {
    vector<vector<int>> adjacencyMatrix = input.adjacencyMatrix;
    const auto globalMinCut = aoc::util::graph::globalMinCut_StoerWagner(adjacencyMatrix);

    const auto sizeConnectedComponentA = globalMinCut.second.size();
    const auto sizeConnectedComponentB = input.nodes.size() - sizeConnectedComponentA;

    const auto sizesMultiplied = sizeConnectedComponentA * sizeConnectedComponentB;

    cout << sizesMultiplied << endl;
}

void part2(const Input &input) {
    cout << "I am finally done... Merry Christmas! <3" << endl;
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