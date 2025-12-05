#include "../../util/BasicIncludes.h"
#include "../../util/MathUtil.h"
#include "../../util/StringUtil.h"
#include "../../util/Types.h"

#include <regex>

using namespace std;

const regex INTERVAL_RX("(\\d+)-(\\d+)");

void part1(istream& inputFile) {
    vector<aoc::util::types::coord_big_t> freshIntervals;

    size_t numFreshIngredients = 0;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        if (line.empty()) {
            continue;
        }

        smatch match;
        if (regex_match(line, match, INTERVAL_RX)) {
            const uint64_t first = stoull(match[1].str());
            const uint64_t second = stoull(match[2].str());

            freshIntervals.push_back({first, second});
        } else {
            istringstream iss(line);
            uint64_t ingredientId;
            iss >> ingredientId;

            for (const auto &interval : freshIntervals) {
                if (interval.first <= ingredientId && ingredientId <= interval.second) {
                    numFreshIngredients += 1;
                    break;
                }
            }
        }
    }

    cout << numFreshIngredients << endl;
}

bool compareIntervals(aoc::util::types::coord_big_t i1, aoc::util::types::coord_big_t i2) {
    return i1.first < i2.first;
}

vector<aoc::util::types::coord_big_t> mergeIntervals(const vector<aoc::util::types::coord_big_t>& intervals) {
    vector<aoc::util::types::coord_big_t> mergedIntervals;
    vector<aoc::util::types::coord_big_t> sortedIntervals = intervals;

    sort(sortedIntervals.begin(), sortedIntervals.end(), compareIntervals);

    mergedIntervals.push_back(sortedIntervals[0]);
    for (size_t i = 1; i < sortedIntervals.size(); i++) {
        auto &prevMergedInterval = mergedIntervals.back();
        const auto &currentInterval = sortedIntervals[i];

        if (currentInterval.first <= prevMergedInterval.second) {
            prevMergedInterval.second = max(currentInterval.second, prevMergedInterval.second);
        } else {
            mergedIntervals.push_back(currentInterval);
        }
    }

    return mergedIntervals;
}

void part2(istream& inputFile) {
    vector<aoc::util::types::coord_big_t> freshIntervals;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        if (line.empty()) {
            break;
        }

        smatch match;
        if (regex_match(line, match, INTERVAL_RX)) {
            const uint64_t first = stoull(match[1].str());
            const uint64_t second = stoull(match[2].str());

            freshIntervals.push_back({first, second});
        }
    }

    size_t numFreshIngredientIds = 0;
    const auto mergedIntervals = mergeIntervals(freshIntervals);
    for (const auto &interval : mergedIntervals) {
        numFreshIngredientIds += interval.second - interval.first + 1;
    }

    cout << numFreshIngredientIds << endl;
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