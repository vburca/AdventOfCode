#include "../../util/StringUtil.h"
#include "../../util/NumbersUtil.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint>
#include <vector>
#include <regex>
#include <cassert>
#include <math.h>
#include <utility>

using namespace std;

regex TIME_RX("Time: (.*)");
regex DISTANCE_RX("Distance: (.*)");

constexpr size_t kSpeedDelta = 1;

struct RaceRecord {
    size_t time;
    size_t distance;
};

// f (x) = tx - x^2
// f'(x) = -2x + t

const vector<RaceRecord> parseInput(istream& inputFile) {
    vector<RaceRecord> records;

    string timeLine;
    getline(inputFile, timeLine);
    smatch timeMatch;
    if (!regex_match(timeLine, timeMatch, TIME_RX)) {
        std::cout << "TIME_RX failed to match line: { " << timeLine << " }" << endl;
        exit(1);
    }

    vector<size_t> times = aoc::util::string::parseDelimSeparatedNumbers(aoc::util::string::trimCopy(timeMatch[1].str()), ' ');

    string distanceLine;
    getline(inputFile, distanceLine);
    smatch distanceMatch;
    if (!regex_match(distanceLine, distanceMatch, DISTANCE_RX)) {
        std::cout << "DISTANCE_RX failed to match line: { " << distanceLine << " }" << endl;
        exit(1);
    }

    vector<size_t> distances = aoc::util::string::parseDelimSeparatedNumbers(aoc::util::string::trimCopy(distanceMatch[1].str()), ' ');

    assert(times.size() == distances.size() && "Times and Distances not equal");
    for (size_t i = 0; i < times.size(); i++) {
        records.push_back({
            .time = times[i],
            .distance = distances[i],
        });
    }

    return records;
}

float _sqrtDelta(const size_t t, const size_t d) {
    return sqrt(t * t - 4 * d);
}

pair<size_t, size_t> _getSolutions(const size_t t, const size_t d) {
    const auto sqrtDelta = _sqrtDelta(t, d);
    const float x1 = static_cast<float>(-1.f * t + sqrtDelta) / -2.f;
    const float x2 = static_cast<float>(-1.f * t - sqrtDelta) / -2.f;

    float smallerX = x1;
    float largerX = x2;
    if (x2 < x1) {
        smallerX = x2;
        largerX = x1;
    }

    return make_pair(static_cast<size_t>(floor(smallerX) + 1), static_cast<size_t>(ceil(largerX) - 1));
}

void part1(const vector<RaceRecord> &records) {
    size_t prodWaysToBeat = 1.f;
    for (const auto &record : records) {
        const auto solutions = _getSolutions(record.time, record.distance);
        prodWaysToBeat *= (solutions.second - solutions.first + 1);
    }

    cout << prodWaysToBeat << endl;
}

void part2(const vector<RaceRecord> &records) {
    // construct numbers
    size_t t = records[0].time;
    size_t d = records[0].distance;
    for (size_t i = 1; i < records.size(); i++) {
        t = t * pow(10, aoc::util::numbers::getNumberOfDigits(records[i].time)) + records[i].time;
        d = d * pow(10, aoc::util::numbers::getNumberOfDigits(records[i].distance)) + records[i].distance;
    }

    const auto solutions = _getSolutions(t, d);
    const int64_t numWaysToWin = solutions.second - solutions.first + 1;

    printf("%d", numWaysToWin);
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        std::cout << "Must specify input file!" << endl;
        return 0;
    }

    std::cout << "Input file name: " << argv[1] << endl;
    ifstream inputFile(argv[1]);

    const auto records = parseInput(inputFile);

    std::cout << "Part 1: " << endl;
    part1(records);

    std::cout << "Part 2: " << endl;
    part2(records);

    inputFile.close();

    return 0;
}