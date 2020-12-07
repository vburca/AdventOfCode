#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <regex>
#include <queue>
#include <utility>

using namespace std;

const regex BAG_CONSTRAINTS("(.*) bags contain (.*)");
const regex BAG_COUNT("\\s?([0-9]+) (.*) bags?.?");
const string NO_EDGE = "no other bags.";
const string INTERESTED_BAG_COLOR = "shiny gold";

void printMatrix(const vector<vector<size_t>>& matrix)
{
    cout << "Print matrix ( " << matrix.size() << ", " << matrix[0].size() << " ) " << endl;

    for (const auto& row : matrix)
    {
        for (const auto& e : row)
        {
            cout << e << " ";
        }
        cout << endl;
    }
}

size_t BFS(const vector<vector<size_t>>& adjMatrix, const size_t startNodeId, unordered_set<size_t>& visitedNodeIds)
{
    queue<pair<size_t, size_t>> nodeIdsToVisit;  // (nodeId, number of current bags)
    unordered_set<size_t> visited;

    nodeIdsToVisit.emplace(startNodeId, 1);

    // number of bags that can be contained by the startNodeId bag
    size_t numberOfBags = 0;

    while (!nodeIdsToVisit.empty())
    {
        const auto nodeAndCount = nodeIdsToVisit.front();
        nodeIdsToVisit.pop();

        const auto nodeId = nodeAndCount.first;
        const auto currentBagCount = nodeAndCount.second;

        visited.insert(nodeId);
        if (nodeId != startNodeId)
        {
            // add it to the set of nodes that we flood into from the start node
            // (i.e. contains all visited nodes except the start one)
            visitedNodeIds.insert(nodeId);
        }

        // go through all the edges of this node
        for (size_t neighborNodeId = 0; neighborNodeId < adjMatrix[nodeId].size(); ++neighborNodeId)
        {
            // if there is a neighbor (i.e. edge with count > 0)
            const auto count = adjMatrix[nodeId][neighborNodeId];
            if (count > 0)
            {
                numberOfBags += currentBagCount * count;

                // add the neighbor to the queue of next nodes to check
                nodeIdsToVisit.emplace(neighborNodeId, currentBagCount * count);
            }
        }
    }

    return numberOfBags;
}

void processInput(
    istream& inputFile,
    unordered_map<string, size_t>& nodeToId,
    unordered_map<size_t, string>& idToNode,
    vector<vector<size_t>>& adjMatrix,
    vector<vector<size_t>>& adjMatrix_T)
{
    unordered_map<string, vector<pair<string, size_t>>> adjListStr;
    size_t nodeId = 0;

    // read input line by line
    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        smatch match;
        if (regex_match(line, match, BAG_CONSTRAINTS))
        {
            // get the outside bag and its insides
            string node = match[1].str();
            string edges = match[2].str();

            // create an index for each bag color
            if (nodeToId.find(node) == nodeToId.end())
            {
                nodeToId[node] = nodeId;
                idToNode[nodeId] = node;
                nodeId += 1;
            }

            // cout << line << endl;
            // cout << node << " : ";

            if (edges == NO_EDGE)
            {
                // if bag contains no other bag, set the adjacency to 0
                adjListStr[node].resize(0);
            }
            else
            {
                istringstream edgesStream(edges);
                string edge;
                // split containing list into individual bag and count
                while (getline(edgesStream, edge, ','))
                {
                    if (regex_match(edge, match, BAG_COUNT))
                    {
                        // get the bag count (count)
                        istringstream countStream(match[1].str());
                        size_t count;
                        countStream >> count;

                        // get the bag color
                        string targetNode = match[2].str();

                        // create an index for each bag color
                        if (nodeToId.find(targetNode) == nodeToId.end())
                        {
                            nodeToId[targetNode] = nodeId;
                            idToNode[nodeId] = targetNode;
                            nodeId += 1;
                        }

                        // cout << " ( " << targetNode << ", " << count << " ) ";

                        // append the (edge, count) pair to the adjacency list
                        adjListStr[node].emplace_back(targetNode, count);
                    }
                    else
                    {
                        cout << endl << "Edges line did not match regex: " << endl;
                        cout << edge << endl;
                        exit(1);
                    }
                }
            }

            // cout << endl << endl;
        }
        else
        {
            cout << endl << "Line did not match regex: " << endl;
            cout << line << endl;
            exit(1);
        }
    }

    // create an adjacency matrix and its transpose from the adjacency list
    const auto bagCount = adjListStr.size();

    adjMatrix.resize(bagCount);
    // the transpose of the adjacency matrix is the bag color graph with the directed edges reversed
    adjMatrix_T.resize(bagCount);

    for (auto& row : adjMatrix)
    {
        row.resize(bagCount);
        fill(row.begin(), row.end(), 0);
    }

    for (auto& row : adjMatrix_T)
    {
        row.resize(bagCount);
        fill(row.begin(), row.end(), 0);
    }

    for (const auto& elem : adjListStr)
    {
        const auto nodeId = nodeToId[elem.first];

        for (const auto& edgeWeight : elem.second)
        {
            const auto neighborNodeId = nodeToId[edgeWeight.first];
            const auto count = edgeWeight.second;

            adjMatrix[nodeId][neighborNodeId] = count;     // nodeId           --(count)--> neighborNodeId
            adjMatrix_T[neighborNodeId][nodeId] = count;   // neighborNodeId   --(count)--> nodeId
        }
    }
}

void part1(unordered_map<string, size_t>& nodeToId, const vector<vector<size_t>>& adjMatrix_T)
{
    // now we have adjacency lists and matrices for the graph of bag colors
    // we can start BFS-ing from the shiny gold bag color node
    const auto interestedNodeId = nodeToId[INTERESTED_BAG_COLOR];
    unordered_set<size_t> visitedNodeIds;
    BFS(adjMatrix_T, interestedNodeId, visitedNodeIds);

    cout << visitedNodeIds.size() << endl;
}

void part2(unordered_map<string, size_t>& nodeToId, const vector<vector<size_t>>& adjMatrix)
{
    // now we have adjacency lists and matrices for the graph of bag colors
    // we can start BFS-ing from the shiny gold bag color node
    const auto interestedNodeId = nodeToId[INTERESTED_BAG_COLOR];
    unordered_set<size_t> visitedNodeIds;
    size_t numberOfBagsContained = BFS(adjMatrix, interestedNodeId, visitedNodeIds);

    cout << numberOfBagsContained << endl;
}


int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        cout << "Must specify input file!" << endl;
        return 0;
    }

    cout << "Input file name: " << argv[1] << endl;
    ifstream inputFile(argv[1]);

    unordered_map<string, size_t> nodeToId;
    unordered_map<size_t, string> idToNode;
    vector<vector<size_t>> adjMatrix;
    vector<vector<size_t>> adjMatrix_T;

    processInput(inputFile, nodeToId, idToNode, adjMatrix, adjMatrix_T);

    cout << "Part 1: " << endl;
    part1(nodeToId, adjMatrix_T);

    inputFile.clear();
    inputFile.seekg(0, ios::beg);

    cout << "Part 2: " << endl;
    part2(nodeToId, adjMatrix);

    inputFile.close();

    return 0;
}