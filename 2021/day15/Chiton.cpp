#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <optional>
#include <queue>
#include <unordered_set>
#include <functional>

using namespace std;

#define MAP_SIZE_MULTIPLIER_PART_2 5

typedef pair<size_t, size_t> point_t;

struct point_t_equal
{
    bool operator()(const point_t& p1, const point_t& p2) const
    {
        return (p1.first == p2.first) && (p1.second == p2.second);
    }
};

struct point_t_hash
{
    size_t operator()(const point_t& p) const
    {
        return p.first ^ p.second;
    }
};

typedef pair<point_t, size_t> weighted_point_t;

struct weighted_point_t_compare
{
    bool operator()(const weighted_point_t& wp1, const weighted_point_t& wp2) const
    {
        return wp1.second > wp2.second;
    }
};

typedef priority_queue<weighted_point_t, vector<weighted_point_t>, weighted_point_t_compare> point_priority_queue_t;
typedef unordered_set<point_t, point_t_hash, point_t_equal> point_set_t;
typedef vector<vector<size_t>> matrix_t;
typedef function<size_t(const matrix_t&, const size_t, const size_t, const point_t&)> weight_func_t;

struct WeightFuncPart1
{
    size_t operator()(const matrix_t& weightTile, const size_t kMapHeight, const size_t kMapWidth, const point_t& point)
    {
        return weightTile[point.first][point.second];
    }
};

struct WeightFuncPart2
{
    size_t operator()(const matrix_t& weightTile, const size_t kMapHeight, const size_t kMapWidth, const point_t& point)
    {
        const auto tileHeight = weightTile.size();
        const auto tileWidth = weightTile.front().size();

        const auto rowOffset = point.first / tileHeight;
        const auto colOffset = point.second / tileWidth;

        const auto tileRow = point.first % tileHeight;
        const auto tileCol = point.second % tileWidth;

        const auto rawWeight = weightTile[tileRow][tileCol] + rowOffset + colOffset;
        const auto weight = rawWeight < 10 ? rawWeight : (rawWeight % 10 + 1);
        return weight;
    }
};


const vector<point_t> getNeighbors(const point_t& p, const size_t kMapHeight, const size_t kMapWidth)
{
    vector<point_t> neighbors;

    if (p.first > 0) neighbors.push_back({p.first - 1, p.second});                  // top
    if (p.second < kMapWidth - 1) neighbors.push_back({p.first, p.second + 1});     // right
    if (p.first < kMapHeight - 1) neighbors.push_back({p.first + 1, p.second});     // bottom
    if (p.second > 0) neighbors.push_back({p.first, p.second - 1});                 // left

    return neighbors;
}

// Dijkstra
const size_t getShortestPathCost(
    const matrix_t& weightTile,
    const size_t kMapHeight,
    const size_t kMapWidth,
    const point_t& startPoint,
    const point_t& endPoint,
    const weight_func_t& getWeightFunc)
{
    matrix_t distances(kMapHeight, vector<size_t>(kMapWidth, SIZE_MAX));
    vector<vector<optional<point_t>>> previous(kMapHeight, vector<optional<point_t>>(kMapWidth, nullopt));

    point_priority_queue_t priorityPointsQueue;
    point_set_t visitedPoints;

    // set distance of start point to 0
    distances[startPoint.first][startPoint.second] = 0;

    for (size_t i = 0; i < kMapHeight; ++i)
    {
        for (size_t j = 0; j < kMapWidth; ++j)
        {
            priorityPointsQueue.push({{i, j}, distances[i][j]});
        }
    }

    while (!priorityPointsQueue.empty())
    {
        const auto currentWeightedPoint = priorityPointsQueue.top();
        const auto currentPoint = currentWeightedPoint.first;
        priorityPointsQueue.pop();

        visitedPoints.insert(currentPoint);

        const auto neighbors = getNeighbors(currentPoint, kMapHeight, kMapWidth);
        for (const auto& neighbor : neighbors)
        {
            if (visitedPoints.find(neighbor) != visitedPoints.end())
            {
                continue; // we already found the shortest path to that point
            }

            // calculate new distance dist(start, current) + dist(current, neigbor)
            const auto distance = distances[currentPoint.first][currentPoint.second] + getWeightFunc(weightTile, kMapHeight, kMapWidth, neighbor);
            auto& startToNeighborDistance = distances[neighbor.first][neighbor.second];
            if (distance < startToNeighborDistance)
            {
                startToNeighborDistance = distance;
                previous[neighbor.first][neighbor.second] = currentPoint;

                priorityPointsQueue.push({neighbor, distance});
            }
        }
    }

    const auto shortestPathCost = distances[endPoint.first][endPoint.second];
    return shortestPathCost;
}

void part1(istream& inputFile)
{
    matrix_t weightTile;

    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        vector<size_t> weightLine;
        for (const auto& ch : line)
        {
            const auto riskLevel = ch - '0';
            weightLine.push_back(riskLevel);
        }

        weightTile.push_back(weightLine);
    }

    const auto kMapHeight = weightTile.size();
    const auto kMapWidth = weightTile.front().size();

    const auto startPoint = point_t{0, 0};
    const auto endPoint = point_t{kMapHeight - 1, kMapWidth - 1};

    const weight_func_t weightFunc = WeightFuncPart1();
    const auto shortestPathCost = getShortestPathCost(weightTile, kMapHeight, kMapWidth, startPoint, endPoint, weightFunc);

    cout << shortestPathCost << endl;
}

void part2(istream& inputFile)
{
    matrix_t weightTile;

    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        vector<size_t> weightLine;
        for (const auto& ch : line)
        {
            const auto riskLevel = ch - '0';
            weightLine.push_back(riskLevel);
        }

        weightTile.push_back(weightLine);
    }

    const auto kMapHeight = weightTile.size() * MAP_SIZE_MULTIPLIER_PART_2;
    const auto kMapWidth = weightTile.front().size() * MAP_SIZE_MULTIPLIER_PART_2;

    const auto startPoint = point_t{0, 0};
    const auto endPoint = point_t{kMapHeight - 1, kMapWidth - 1};

    const weight_func_t weightFunc = WeightFuncPart2();
    const auto shortestPathCost = getShortestPathCost(weightTile, kMapHeight, kMapWidth, startPoint, endPoint, weightFunc);

    cout << shortestPathCost << endl;
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

    cout << "Part 1: " << endl;
    part1(inputFile);

    inputFile.clear();
    inputFile.seekg(0, ios::beg);

    cout << "Part 2: " << endl;
    part2(inputFile);

    inputFile.close();

    return 0;
}