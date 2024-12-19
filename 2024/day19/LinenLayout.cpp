#include "../../util/BasicIncludes.h"
#include "../../util/StringUtil.h"
#include "../../util/Types.h"
#include "../../util/MathUtil.h"
#include "../../util/Graph.h"

#include <vector>
#include <set>
#include <queue>

using namespace std;
using namespace aoc::util::string;
using namespace aoc::util::types;
using namespace aoc::util::graph;
using namespace aoc::util::math;

struct TowelState {
    vector<string> towels;
    vector<string> patterns;
};

void _printState(const TowelState &s) {
    for (size_t i = 0; i < s.towels.size(); ++i) {
        cout << s.towels[i];
        if (i < s.towels.size() - 1) {
            cout << ", ";
        }
    }
    cout << endl;

    for (const auto &pattern : s.patterns) {
        cout << pattern << endl;
    }
    cout << endl;
}

const uint64_t _countArrangements(const string &pattern, size_t startIdx, const TowelState &s, vector<int64_t> &countCache) {
    if (startIdx >= pattern.size()) {
        // we successfully matched the entirety of the pattern
        return 1;
    }

    if (countCache[startIdx] != -1) {
        return countCache[startIdx];
    }

    uint64_t count = 0;
    for (const auto &towel : s.towels) {
        if (towel.size() > (pattern.size() - startIdx)) {
            // current towel is too large for the remaining pattern
            continue;
        }

        // try to match the current towel with the portion of the pattern that matches its length
        const string patternSubstr = pattern.substr(startIdx, towel.size());
        if (patternSubstr == towel) {
            count += _countArrangements(pattern, startIdx + towel.size(), s, countCache);
        }
    }

    countCache[startIdx] = count;

    return count;
}

void part1(istream& inputFile) {
    TowelState s;

    bool towelsRead = false;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        if (line.empty()) {
            continue;
        }

        if (!towelsRead) {
            stringstream ss(line);
            string towel;
            while (getline(ss, towel, ',')) {
                ltrim(towel);
                s.towels.push_back(towel);
            }

            towelsRead = true;
        } else {
            s.patterns.push_back(line);
        }
    }

    // _printState(s);

    size_t validPatterns = 0;
    for (const auto &pattern : s.patterns) {
        vector<int64_t> countCache = vector<int64_t>(pattern.size(), -1);
        const auto numArrangements = _countArrangements(pattern, 0, s, countCache);
        validPatterns += numArrangements > 0 ? 1 : 0;
        // if (numArrangements > 0) {
        //     cout << pattern << endl;
        // }
    }

    cout << validPatterns << endl;
}

void part2(istream& inputFile) {
    TowelState s;

    bool towelsRead = false;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        if (line.empty()) {
            continue;
        }

        if (!towelsRead) {
            stringstream ss(line);
            string towel;
            while (getline(ss, towel, ',')) {
                ltrim(towel);
                s.towels.push_back(towel);
            }

            towelsRead = true;
        } else {
            s.patterns.push_back(line);
        }
    }

    // _printState(s);

    uint64_t totalArrangements = 0;
    for (const auto &pattern : s.patterns) {
        vector<int64_t> countCache = vector<int64_t>(pattern.size(), -1);
        const auto numArrangements = _countArrangements(pattern, 0, s, countCache);
        totalArrangements += numArrangements;
        // if (numArrangements > 0) {
        //     cout << pattern << endl;
        // }
    }

    cout << totalArrangements << endl;
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