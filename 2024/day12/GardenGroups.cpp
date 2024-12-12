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

struct Region {
    char type;
    coord_t startPosition;

    size_t area;
    size_t perimeter;

    vector<coord_t> topEdgePositions;
    vector<coord_t> rightEdgePositions;
    vector<coord_t> bottomEdgePositions;
    vector<coord_t> leftEdgePositions;

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
    for (const auto &dir : kCompassDirections4) {
        const coord_signed_t nextPosition = {signedCurrentPosition.first + dir.first, signedCurrentPosition.second + dir.second};
        if (nextPosition.first < 0 || nextPosition.first >= gardenH ||
            nextPosition.second < 0 || nextPosition.second >= gardenW) {
                continue;
        }

        validNextPositions.insert(nextPosition);
    }

    return validNextPositions;
}

const size_t _countSides(const vector<coord_t> &edgePositions, bool isHorizontal = false) {
    map<size_t, vector<size_t>> edgeCoordToEdgeStretchIdx;
    for (const auto &pos : edgePositions) {
        if (isHorizontal) {
            if (!edgeCoordToEdgeStretchIdx.contains(pos.first)) {
                edgeCoordToEdgeStretchIdx[pos.first] = vector<size_t>();
            }
            edgeCoordToEdgeStretchIdx[pos.first].push_back(pos.second);
        } else {
            if (!edgeCoordToEdgeStretchIdx.contains(pos.second)) {
                edgeCoordToEdgeStretchIdx[pos.second] = vector<size_t>();
            }
            edgeCoordToEdgeStretchIdx[pos.second].push_back(pos.first);
        }
    }

    // for (const auto &p : edgeCoordToEdgeStretchIdx) {
    //     cout << p.first << " -> { ";
    //     for (const auto &idx : p.second) {
    //         cout << idx << " ";
    //     }
    //     cout << " }" << endl;
    // }

    size_t numEdges = 0;
    for (const auto &p : edgeCoordToEdgeStretchIdx) {
        numEdges += 1;
        auto edgeStretchIndices = p.second;
        sort(edgeStretchIndices.begin(), edgeStretchIndices.end());
        for (size_t i = 0; i < edgeStretchIndices.size() - 1; ++i) {
            if (edgeStretchIndices[i + 1] - edgeStretchIndices[i] != 1) {
                numEdges += 1;
            }
        }
    }

    return numEdges;
}

const optional<coord_t> _getNextPosition(const kCompass4Orientation orientation, coord_signed_t currentPosition, const char type, const vector<string> &garden, const size_t gardenH, const size_t gardenW) {
    const auto dir = kCompassDirections4[orientation];
    const coord_signed_t nextPosition = {currentPosition.first + dir.first, currentPosition.second + dir.second};

    if (nextPosition.first < 0 || nextPosition.first >= gardenH ||
        nextPosition.second < 0 || nextPosition.second >= gardenW) {
        return nullopt;
    }

    if (garden[nextPosition.first][nextPosition.second] != type) {
        return nullopt;
    }

    return nextPosition;
}

const Region _fillRegionBFS(const char type, const coord_signed_t startPosition, const vector<string> &garden, vector<vector<bool>> &visited) {
    Region region = {
        .type = type,
        .startPosition = startPosition,
    };

    const size_t gardenH = garden.size();
    const size_t gardenW = garden.front().size();

    queue<coord_t> positionsToVisit;
    positionsToVisit.push(startPosition);

    while (!positionsToVisit.empty()) {
        const auto currentPosition = positionsToVisit.front();
        positionsToVisit.pop();

        if (visited[currentPosition.first][currentPosition.second]) {
            continue;
        }

        visited[currentPosition.first][currentPosition.second] = true;
        region.area += 1;

        set<coord_t> nextPositions;
        for (size_t orientation = 0; orientation < kCompass4Orientation::kCount; ++orientation) {
            const auto nextPositionOpt = _getNextPosition((kCompass4Orientation)orientation, currentPosition, type, garden, gardenH, gardenW);
            if (nextPositionOpt) {
                nextPositions.insert(*nextPositionOpt);
                continue;
            }

            const kCompass4Orientation compassOrientation = (kCompass4Orientation)orientation;
            if (compassOrientation == kCompass4Orientation::kNorth) {
                region.topEdgePositions.push_back(currentPosition);
            }

            if (compassOrientation == kCompass4Orientation::kEast) {
                region.rightEdgePositions.push_back(currentPosition);
            }

            if (compassOrientation == kCompass4Orientation::kSouth) {
                region.bottomEdgePositions.push_back(currentPosition);
            }

            if (compassOrientation == kCompass4Orientation::kWest) {
                region.leftEdgePositions.push_back(currentPosition);
            }
        }

        for (const auto &nextPosition : nextPositions) {
            if (garden[nextPosition.first][nextPosition.second] != type) {
                continue;
            }

            if (visited[nextPosition.first][nextPosition.second]) {
                continue;
            }

            positionsToVisit.push(nextPosition);
        }
    }

    return region;
}

const set<Region> _getRegions(const map<char, set<coord_t>> &typeToPositions, const vector<string> &garden) {
    set<Region> regions;

    vector<vector<bool>> visitedPositions = vector<vector<bool>>(garden.size(), vector<bool>(garden.front().size(), false));

    for (const auto &p : typeToPositions) {
        const char type = p.first;
        const auto currentTypePositions = p.second;

        for (const auto &startPosition : currentTypePositions) {
            if (visitedPositions[startPosition.first][startPosition.second]) {
                continue;
            }

            const auto region = _fillRegionBFS(type, startPosition, garden, visitedPositions);
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
    for (auto &region : regions) {
        const auto perimeter = region.topEdgePositions.size() + region.rightEdgePositions.size() + region.bottomEdgePositions.size() + region.leftEdgePositions.size();
        // cout << region.type << ": a=" << region.area
        //     << " p(top)=" << region.topEdgePositions.size()
        //     << " p(right)=" << region.rightEdgePositions.size()
        //     << " p(bottom)=" << region.bottomEdgePositions.size()
        //     << " p(left)=" << region.leftEdgePositions.size()
        //     << " p= " << perimeter << endl;

        totalPrice += region.area * perimeter;
    }

    cout << totalPrice << endl;
}

void part2(istream& inputFile) {
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
        // cout << "region=" << region.type << endl;
        // cout << "topSides:" << endl;
        const auto topSides = _countSides(region.topEdgePositions, true);

        // cout << "rightSides:" << endl;
        const auto rightSides = _countSides(region.rightEdgePositions, false);

        // cout << "bottomSides:" << endl;
        const auto bottomSides = _countSides(region.bottomEdgePositions, true);

        // cout << "leftSides:" << endl;
        const auto leftSides = _countSides(region.leftEdgePositions, false);

        const auto totalSides = topSides + rightSides + bottomSides + leftSides;
        // cout << region.type << ": a=" << region.area
        //     << " sides(top)=" << topSides
        //     << " sides(right)=" << rightSides
        //     << " sides(bottom)=" << bottomSides
        //     << " sides(left)=" << leftSides
        //     << " totalSides=" << totalSides << endl << endl;

        totalPrice += region.area * totalSides;
    }

    cout << totalPrice << endl;
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