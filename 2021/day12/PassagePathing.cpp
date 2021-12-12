#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <regex>
#include <deque>

using namespace std;

struct NodeState
{
    string nodeName;
    unordered_set<string> visitedSmallNodes;
    vector<string> path;
    bool visitedSmallCaveTwice;
};

const bool isStart(const string& caveName)
{
    return caveName == "start";
}

const bool isEnd(const string& caveName)
{
    return caveName == "end";
}

const bool isSmallCave(const string& caveName)
{
    for (const auto& ch : caveName)
    {
        if (!islower(ch)) return false;
    }

    return true;
}

const size_t getNumPaths(const unordered_map<string, vector<string>>& neighborMap, const bool allowSmallCaveTwice = false)
{
    const auto startNode = "start";
    deque<NodeState> nextNodes;
    nextNodes.push_back(NodeState{
        .nodeName = startNode,
        .visitedSmallNodes = {},
        .path = {},
        .visitedSmallCaveTwice = false});

    size_t numPaths = 0;
    while (!nextNodes.empty())
    {
        auto currentNodeState = nextNodes.front();
        nextNodes.pop_front();

        if (isSmallCave(currentNodeState.nodeName))
        {
            // add small cave as visited
            currentNodeState.visitedSmallNodes.insert(currentNodeState.nodeName);
        }

        // update path so far
        currentNodeState.path.push_back(currentNodeState.nodeName);

        if (isEnd(currentNodeState.nodeName))
        {
            // we got to the end, count this path
            numPaths += 1;
        }
        else
        {
            // not the end
            // get neighbors
            const auto neighbors = neighborMap.at(currentNodeState.nodeName);
            for (const auto& neighbor : neighbors)
            {
                if (isStart(neighbor))
                {
                    // ignore start node
                    continue;
                }

                bool addItNext = true;
                bool visitedSmallCaveTwice = currentNodeState.visitedSmallCaveTwice;
                if (isSmallCave(neighbor))
                {
                    // if it's a small cave, check if this small cave has been visited in the current path already
                    const auto& it = currentNodeState.visitedSmallNodes.find(neighbor);
                    if (it != currentNodeState.visitedSmallNodes.end())
                    {
                        if (!allowSmallCaveTwice)
                        {
                            // we already visited this small cave on this path, we can not visit it again
                            addItNext = false;
                        }
                        else
                        {
                            // for part 2, where we allow at most one small cave per path to be visited at most twice
                            if (currentNodeState.visitedSmallCaveTwice)
                            {
                                // if we already visited some other small cave twice in this path, we can not do it again
                                addItNext = false;
                            }
                            else
                            {
                                // try to visit it twice
                                visitedSmallCaveTwice = true;
                            }
                        }
                    }
                }

                // we know that we can visit this cave next
                if (addItNext)
                {
                    nextNodes.push_back(NodeState{
                        .nodeName = neighbor,
                        .visitedSmallNodes = currentNodeState.visitedSmallNodes,
                        .path = currentNodeState.path,
                        .visitedSmallCaveTwice = visitedSmallCaveTwice});
                }
            }
        }
    }

    return numPaths;
}

void part1(istream& inputFile)
{
    const regex rx("([a-zA-Z]+)-([a-zA-Z]+)");

    unordered_map<string, vector<string>> neighborMap;

    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        smatch match;
        if (regex_match(line, match, rx))
        {
            const auto nodeA = match[1];
            const auto nodeB = match[2];

            const auto& itA = neighborMap.find(nodeA);
            if (itA == neighborMap.end())
            {
                neighborMap.insert({nodeA, vector<string>()});
            }

            const auto& itB = neighborMap.find(nodeB);
            if (itB == neighborMap.end())
            {
                neighborMap.insert({nodeB, vector<string>()});
            }

            neighborMap[nodeA].push_back(nodeB);
            neighborMap[nodeB].push_back(nodeA);
        }
        else
        {
            cout << "Regex failed for line: " << line << endl;
            return;
        }
    }

    const auto numPaths = getNumPaths(neighborMap);
    cout << numPaths << endl;
}

void part2(istream& inputFile)
{
    const regex rx("([a-zA-Z]+)-([a-zA-Z]+)");

    unordered_map<string, vector<string>> neighborMap;

    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        smatch match;
        if (regex_match(line, match, rx))
        {
            const auto nodeA = match[1];
            const auto nodeB = match[2];

            const auto& itA = neighborMap.find(nodeA);
            if (itA == neighborMap.end())
            {
                neighborMap.insert({nodeA, vector<string>()});
            }

            const auto& itB = neighborMap.find(nodeB);
            if (itB == neighborMap.end())
            {
                neighborMap.insert({nodeB, vector<string>()});
            }

            neighborMap[nodeA].push_back(nodeB);
            neighborMap[nodeB].push_back(nodeA);
        }
        else
        {
            cout << "Regex failed for line: " << line << endl;
            return;
        }
    }

    const auto numPaths = getNumPaths(neighborMap, true);
    cout << numPaths << endl;}


int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        cout << "Must specify input file!" << endl;
        return 0;
    }

    cout << "Input file name: " << argv[1] << endl;
    ifstream inputFile(argv[1]);

    cout << "Part 1: " << endl;
    part1(inputFile);

    inputFile.clear();
    inputFile.seekg(0, ios::beg);

    cout << "Part 2: " << endl;
    part2(inputFile);

    inputFile.close();

    return 0;
}