#include "../../util/BasicIncludes.h"
#include "../../util/MathUtil.h"
#include "../../util/StringUtil.h"
#include "../../util/Types.h"

#include <queue>
#include <set>

using namespace std;

const bool isSplitter(const vector<string> &tachyonMap, const aoc::util::types::coord_t pos) {
    return tachyonMap[pos.first][pos.second] == '^';
}

const uint64_t traverseTachyon(const vector<string> &tachyonMap, const aoc::util::types::coord_t startPosition) {
    const auto n = tachyonMap.size();
    const auto m = tachyonMap.front().size();

    queue<aoc::util::types::coord_t> positionsToVisit;
    positionsToVisit.push(startPosition);

    size_t numSplittersHit = 0;
    set<aoc::util::types::coord_t> visitedPositions;
    while (!positionsToVisit.empty()) {
        const auto &pos = positionsToVisit.front();
        positionsToVisit.pop();

        if (visitedPositions.contains(pos)) {
            continue;
        }

        visitedPositions.insert(pos);
        if (pos.first == n - 1) {
            continue;
        }

        aoc::util::types::coord_t newPosition = {pos.first + 1, pos.second};
        if (isSplitter(tachyonMap, newPosition)) {
            numSplittersHit += 1;

            if (newPosition.second > 0) {
                positionsToVisit.push({newPosition.first, newPosition.second - 1});
            }

            if (newPosition.second < m - 1) {
                positionsToVisit.push({newPosition.first, newPosition.second + 1});
            }
        } else {
            positionsToVisit.push(newPosition);
        }
    }

    return numSplittersHit;
}

const uint64_t traverseTachyonTimelines(const vector<string> &tachyonMap, const aoc::util::types::coord_t startPosition) {
    const auto n = tachyonMap.size();
    const auto m = tachyonMap.front().size();

    vector<vector<uint64_t>> timelineVisits(n, vector<uint64_t>(m, 0));
    queue<aoc::util::types::coord_t> positionsToVisit;
    positionsToVisit.push(startPosition);
    timelineVisits[startPosition.first][startPosition.second] += 1;

    set<aoc::util::types::coord_t> visitedPositions;
    while (!positionsToVisit.empty()) {
        const auto &pos = positionsToVisit.front();
        positionsToVisit.pop();

        if (visitedPositions.contains(pos)) {
            continue;
        }

        visitedPositions.insert(pos);
        if (pos.first == n - 1) {
            continue;
        }

        aoc::util::types::coord_t newPosition = {pos.first + 1, pos.second};
        if (isSplitter(tachyonMap, newPosition)) {
            if (newPosition.second > 0) {
                positionsToVisit.push({newPosition.first, newPosition.second - 1});
                timelineVisits[newPosition.first][newPosition.second - 1] += timelineVisits[pos.first][pos.second];
            }

            if (newPosition.second < m - 1) {
                positionsToVisit.push({newPosition.first, newPosition.second + 1});
                timelineVisits[newPosition.first][newPosition.second + 1] += timelineVisits[pos.first][pos.second];
            }
        } else {
            positionsToVisit.push(newPosition);
            timelineVisits[newPosition.first][newPosition.second] += timelineVisits[pos.first][pos.second];
        }
    }

    uint64_t numTimelines = 0;
    for (size_t i = 0; i < m; i++) {
        numTimelines += timelineVisits[n - 1][i];
    }

    return numTimelines;
}

void part1(istream& inputFile) {
    vector<string> tachyonMap;
    size_t i = 0;
    aoc::util::types::coord_t startPos;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        size_t j = line.find('S');
        if (j != std::string::npos) {
            startPos.first = i;
            startPos.second = j;
        }

        tachyonMap.push_back(line);
        i += 1;
    }

    const auto beamSplits = traverseTachyon(tachyonMap, startPos);
    cout << beamSplits << endl;
}

void part2(istream& inputFile) {
    vector<string> tachyonMap;
    size_t i = 0;
    aoc::util::types::coord_t startPos;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        size_t j = line.find('S');
        if (j != std::string::npos) {
            startPos.first = i;
            startPos.second = j;
        }

        tachyonMap.push_back(line);
        i += 1;
    }

    const auto numTimelines = traverseTachyonTimelines(tachyonMap, startPos);
    cout << numTimelines << endl;
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