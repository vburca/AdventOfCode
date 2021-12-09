#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <utility>
#include <queue>
#include <unordered_set>

using namespace std;

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

void part1(istream& inputFile)
{
    vector<vector<size_t>> heightMap;

    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        const auto n = line.size();

        vector<size_t> mapLine(n, 0);
        for (size_t i = 0; i < n; ++i)
        {
            mapLine[i] = line[i] - '0';
        }

        heightMap.push_back(mapLine);
    }

    const auto height = heightMap.size();
    const auto width = heightMap.front().size();

    size_t totalRiskLevel = 0;
    for (size_t row = 0; row < height; ++row)
    {
        for (size_t col = 0; col < width; ++col)
        {
            const auto curr = heightMap[row][col];
            const auto top = row > 0 ? heightMap[row - 1][col] : 10; // 10 is a value greater than any possible neighbor
            const auto right = col < width - 1 ? heightMap[row][col + 1] : 10;
            const auto bottom = row < height - 1 ? heightMap[row + 1][col] : 10;
            const auto left = col > 0 ? heightMap[row][col - 1] : 10;

            // check if current element is greater than any of the neighbors
            if (top <= curr) continue;
            if (right <= curr) continue;
            if (bottom <= curr) continue;
            if (left <= curr) continue;

            // it means no neighbors is greater or equal, hence this is a low point
            // add its risk level
            totalRiskLevel += (curr + 1);
        }
    }

    cout << totalRiskLevel << endl;
}

const vector<point_t> getNeighbors(const point_t& p, const size_t width, const size_t height)
{
    vector<point_t> neighbors;

    if (p.first > 0) neighbors.push_back({p.first - 1, p.second});              // top
    if (p.second < width - 1) neighbors.push_back({p.first, p.second + 1});     // right
    if (p.first < height - 1) neighbors.push_back({p.first + 1, p.second});     // bottom
    if (p.second > 0) neighbors.push_back({p.first, p.second - 1});             // left

    return neighbors;
}

size_t computeBasinSize(point_t startPoint, const vector<vector<size_t>> map)
{
    const auto width = map.front().size();
    const auto height = map.size();

    size_t basinSize = 0;
    unordered_set<point_t, point_t_hash, point_t_equal> visited;
    queue<point_t> pointsToCheck;
    pointsToCheck.push(startPoint);

    while (!pointsToCheck.empty())
    {
        const auto point = pointsToCheck.front();
        pointsToCheck.pop();

        basinSize += 1;

        const auto neighbors = getNeighbors(point, width, height);
        for (const auto& neighbor : neighbors)
        {
            const auto currentValue = map[point.first][point.second];
            const auto neighborValue = map[neighbor.first][neighbor.second];
            if (neighborValue == 9) continue; // can not be part of a basin
            if (neighborValue <= currentValue) continue; // we need to move in gradient upwards
            if (visited.find(neighbor) != visited.end()) continue; // we already visited this neighbor or are about to

            visited.insert(neighbor);
            pointsToCheck.push(neighbor);
        }
    }

    return basinSize;
}

void part2(istream& inputFile)
{
    vector<vector<size_t>> heightMap;

    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        const auto n = line.size();

        vector<size_t> mapLine(n, 0);
        for (size_t i = 0; i < n; ++i)
        {
            mapLine[i] = line[i] - '0';
        }

        heightMap.push_back(mapLine);
    }

    const auto height = heightMap.size();
    const auto width = heightMap.front().size();

    vector<size_t> basinSizes;
    for (size_t row = 0; row < height; ++row)
    {
        for (size_t col = 0; col < width; ++col)
        {
            const auto curr = heightMap[row][col];
            const auto top = row > 0 ? heightMap[row - 1][col] : 10; // 10 is a value greater than any possible neighbor
            const auto right = col < width - 1 ? heightMap[row][col + 1] : 10;
            const auto bottom = row < height - 1 ? heightMap[row + 1][col] : 10;
            const auto left = col > 0 ? heightMap[row][col - 1] : 10;

            // check if current element is greater than any of the neighbors
            if (top <= curr) continue;
            if (right <= curr) continue;
            if (bottom <= curr) continue;
            if (left <= curr) continue;

            // it means no neighbors is greater or equal, hence this is a low point
            // add its risk level
            basinSizes.push_back(computeBasinSize({row, col}, heightMap));
        }
    }

    sort(basinSizes.begin(), basinSizes.end(), greater<size_t>());

    const auto result = basinSizes[0] * basinSizes[1] * basinSizes[2];
    cout << result << endl;
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