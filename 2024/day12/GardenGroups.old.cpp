#include "../../util/BasicIncludes.h"
#include "../../util/StringUtil.h"
#include "../../util/Types.h"
#include "../../util/Graph.h"

#include <vector>
#include <map>
#include <set>
#include <queue>

using namespace std;
using namespace aoc::util::types;
using namespace aoc::util::graph;

struct GardenPlot {
    char type;
    coord_t position;
    set<coord_t> neighborPositions;

    const size_t getNumNeighbors() const {
        return neighborPositions.size();
    }

    const size_t getOutsideEdges() const {
        return 4 - neighborPositions.size();
    }

    bool operator<(const GardenPlot &other) const {
        return tie(type, position) < tie(other.type, other.position);
    }
};

struct Region {
    char type;
    coord_t startPosition;
    set<GardenPlot> plots;

    const size_t getArea() const {
        return plots.size();
    }

    const size_t getPerimeter() const {
        size_t perimeter = 0;

        for (const auto &plot : plots) {
            perimeter += plot.getOutsideEdges();
        }

        return perimeter;
    }

    bool operator<(const Region &other) const {
        return tie(type, startPosition) < tie(other.type, other.startPosition);
    }
};

void _printPositions(const map<char, set<coord_t>> &typeToPositions) {
    for (const auto &p : typeToPositions) {
        cout << p.first << ": ";
        for (const auto &pos : p.second) {
            cout << "{" << pos.first << "," << pos.second << "} ";
        }
        cout << endl;
    }
}

void _printGarden(const vector<string> &garden) {
    for (const auto &line : garden) {
        cout << line << endl;
    }
}

const set<coord_t> _getNextPositions(const coord_t currentPosition, const size_t gardenH, const size_t gardenW) {
    set<coord_t> validNextPositions;

    const coord_signed_t signedCurrentPosition = currentPosition;
    for (const auto &dir : kCompasDirections4) {
        const coord_signed_t nextPosition = {signedCurrentPosition.first + dir.first, signedCurrentPosition.second + dir.second};
        if (nextPosition.first < 0 || nextPosition.first >= gardenH ||
            nextPosition.second < 0 || nextPosition.second >= gardenW) {
                continue;
        }

        validNextPositions.insert(nextPosition);
    }

    return validNextPositions;
}

const Region _fillRegionBFS(const char type, const coord_t startPosition, const vector<string> &garden) {
    GardenPlot plot = {
        .type = type,
        .position = startPosition,
    };

    map<coord_t, GardenPlot> plotDatabase;
    plotDatabase[plot.position] = plot;

    const size_t gardenH = garden.size();
    const size_t gardenW = garden.front().size();

    set<coord_t> visitedPositions;
    queue<coord_t> positionsToVisit;
    positionsToVisit.push(startPosition);

    while (!positionsToVisit.empty()) {
        const auto currentPosition = positionsToVisit.front();
        positionsToVisit.pop();

        visitedPositions.insert(currentPosition);

        const auto nextPositions = _getNextPositions(currentPosition, gardenH, gardenW);
        for (const auto &nextPosition : nextPositions) {
            if (garden[nextPosition.first][nextPosition.second] != type) {
                continue;
            }

            plotDatabase[currentPosition].neighborPositions.insert(nextPosition);

            if (!plotDatabase.contains(nextPosition)) {
                GardenPlot nextPlot = {
                    .type = type,
                    .position = nextPosition,
                };
                plotDatabase[nextPosition] = nextPlot;
            }
            plotDatabase[nextPosition].neighborPositions.insert(currentPosition);

            if (visitedPositions.contains(nextPosition)) {
                continue;
            }

            positionsToVisit.push(nextPosition);
        }
    }

    Region region = {
        .type = type,
        .startPosition = startPosition,
    };
    for (const auto &p : plotDatabase) {
        region.plots.insert(p.second);
    }

    return region;
}

const set<Region> _getRegions(const map<char, set<coord_t>> &typeToPositions, const vector<string> &garden) {
    set<Region> regions;

    for (const auto &p : typeToPositions) {
        const char type = p.first;
        set<coord_t> currentTypePositions = p.second;

        while (!currentTypePositions.empty()) {
            const auto startPosition = *(currentTypePositions.begin());
            const auto region = _fillRegionBFS(type, startPosition, garden);

            for (const auto &plot : region.plots) {
                currentTypePositions.erase(plot.position);
            }

            regions.insert(region);
        }
    }

    return regions;
}

void part1(istream& inputFile) {
    map<char, set<coord_t>> typeToPositions;
    vector<string> garden;

    size_t rowIdx = 0;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        for (size_t colIdx = 0; colIdx < line.size(); ++colIdx) {
            const auto ch = line[colIdx];
            typeToPositions[ch].insert({rowIdx, colIdx});
        }

        garden.push_back(line);
        rowIdx += 1;
    }

    // _printPositions(typeToPositions);
    // cout << endl;
    // _printGarden(garden);
    // cout << endl;

    const auto regions = _getRegions(typeToPositions, garden);

    size_t totalPrice = 0;
    for (const auto &region : regions) {
        // cout << region.type << ": a=" << region.getArea() << " p=" << region.getPerimeter() << endl;
        totalPrice += region.getArea() * region.getPerimeter();
    }

    cout << totalPrice << endl;
}

void part2(istream& inputFile) {
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