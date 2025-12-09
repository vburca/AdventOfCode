#include "../../util/BasicIncludes.h"
#include "../../util/MathUtil.h"
#include "../../util/StringUtil.h"
#include "../../util/Types.h"

#include <cmath>
#include <cstdlib>

using namespace std;
using namespace aoc::util::types;


struct Edge {
    coord_big_t pointA;
    coord_big_t pointB;

    const bool isVertical() const {
        return pointA.first == pointB.first;
    }
};

const uint64_t getRectangleArea(const coord_t &squareA, const coord_t &squareB) {
    return (abs(int64_t(squareA.first) - int64_t(squareB.first)) + 1) *
            (abs(int64_t(squareA.second) - int64_t(squareB.second)) + 1);
}

void part1(istream& inputFile) {
    vector<coord_t> redSquares;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        const auto coords = aoc::util::string::split(line, ',');
        redSquares.push_back({stoull(coords[0]), stoull(coords[1])});
    }

    uint64_t largestArea = 0;
    for (size_t i = 0; i < redSquares.size() - 1; i++) {
        for (size_t j = i + 1; j < redSquares.size(); j++) {
            largestArea = max(largestArea, getRectangleArea(redSquares[i], redSquares[j]));
        }
    }

    cout << largestArea << endl;
}

void part2(istream& inputFile) {
    vector<coord_big_t> redSquares;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        const auto coords = aoc::util::string::split(line, ',');
        redSquares.push_back({stoull(coords[0]), stoull(coords[1])});
    }

    vector<Edge> edges;
    for (size_t i = 0; i < redSquares.size(); i++) {
        edges.push_back({redSquares[i], redSquares[(i + 1) % redSquares.size()]});
    }

    uint64_t largestArea = 0;
    for (size_t i = 0; i < redSquares.size(); i++) {
        for (size_t j = i + 1; j < redSquares.size(); j++) {
            const auto minX = min(redSquares[i].first, redSquares[j].first);
            const auto minY = min(redSquares[i].second, redSquares[j].second);
            const auto maxX = max(redSquares[i].first, redSquares[j].first);
            const auto maxY = max(redSquares[i].second, redSquares[j].second);

            const auto area = getRectangleArea(redSquares[i], redSquares[j]);
            if (area <= largestArea) {
                continue;
            }

            bool isWithinBoundaries = true;
            for (const auto &edge : edges) {
                if (edge.isVertical()) {
                    const auto edgeX = edge.pointA.first;
                    if (edgeX > minX && edgeX < maxX) {
                        const auto edgeYMin = min(edge.pointA.second, edge.pointB.second);
                        const auto edgeYMax = max(edge.pointA.second, edge.pointB.second);

                        if (max(minY, edgeYMin) < min(maxY, edgeYMax)) {
                            isWithinBoundaries = false;
                            break;
                        }
                    }
                } else {
                    const auto edgeY = edge.pointA.second;
                    if (edgeY > minY && edgeY < maxY) {
                        const auto edgeXMin = min(edge.pointA.first, edge.pointB.first);
                        const auto edgeXMax = max(edge.pointA.first, edge.pointB.first);

                        if (max(minX, edgeXMin) < min(maxX, edgeXMax)) {
                            isWithinBoundaries = false;
                            break;
                        }
                    }
                }
            }

            if (!isWithinBoundaries) {
                continue;
            }

            largestArea = max(largestArea, area);
        }
    }

    cout << largestArea << endl;
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