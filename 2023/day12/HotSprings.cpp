#include "../../util/StringUtil.h"
#include "../../util/MathUtil.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint>
#include <vector>
#include <regex>

using namespace std;

constexpr size_t kUnfoldFactor = 5;

regex RECORD_RX("([?.#]+) ([0-9,]+)");

typedef vector<vector<vector<int64_t>>> mat3d_t;

struct Record {
    string springs;
    vector<size_t> damagedCounts;
};

struct Input {
    vector<Record> records;
};

void _printInput(const Input &input) {
    cout << "Records: " << endl;
    for (const auto &record : input.records) {
        cout << record.springs << " ";
        for (const auto &c : record.damagedCounts) {
            cout << c << ",";
        }
        cout << endl;
    }
    cout << endl;
}

const Input _parseInput(istream& inputFile) {
    Input input;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        smatch recordMatch;
        if (!regex_match(line, recordMatch, RECORD_RX)) {
            cout << "RECORD_RX failed to match line: { " << line << " }" << endl;
            exit(1);
        }

        input.records.emplace_back(Record {
            .springs = recordMatch[1].str(),
            .damagedCounts = aoc::util::string::parseDelimSeparatedNumbers<size_t>(recordMatch[2].str(), ','),
        });
    }

    return input;
}

size_t _getPossibleArrangements(const Record &record, mat3d_t &memoization, size_t springsIdx, size_t damagedCountsIdx, size_t currentDamagedCount) {
    // end condition - we are done parsing the springs list
    if (springsIdx >= record.springs.size()) {
        return damagedCountsIdx == record.damagedCounts.size(); // return success (1) if we processed all damage counts, otherwise failure (0)
    }

    // note: if damagedCountsIdx >= record.damagedCounts.size() we are done yet, we will let springsIdx to iterate and make sure we don't
    // encounter additional '#'s

    // take value from memoization
    auto &arrangementCount = memoization[springsIdx][damagedCountsIdx][currentDamagedCount];
    // if it is set / previously initialized, just return this count
    if (arrangementCount != -1) {
        return arrangementCount;
    }

    // otherwise, set it to 0 and we start computing it
    arrangementCount = 0;

    const auto currentSpring = record.springs[springsIdx];

    // try processing a '.' (either explicit or implicit through morphing a '?')
    if ((currentSpring == '.' || currentSpring == '?') && currentDamagedCount == 0) {
        arrangementCount += _getPossibleArrangements(record, memoization, springsIdx + 1, damagedCountsIdx, 0);
    }

    // try processing a '#' (either explicit or implicit through morphing a '?')
    if ((currentSpring == '#' || currentSpring == '?') && damagedCountsIdx < record.damagedCounts.size()) {
        // if we are almost done with the current sequence of '#'s
        if (currentDamagedCount + 1 == record.damagedCounts[damagedCountsIdx]) {
            // we could potentially finish the current sequence of #'s
            if ((springsIdx == record.springs.size() - 1) || (record.springs[springsIdx + 1] != '#')) {
                // if we are at the end of the springs list or if the next character does not continue the '#' sequence
                // we can then consider this sequence of '#'s as done and move over; we advance 2 springs forward (springIdx + 2)
                // because we assume that the current one is '#' and the next one has to be something that is not a '#'
                // (either a '.' or a '?' that converts to a '.' - otherwise, the current sequence of '#'s wouldn't have been done yet)
                arrangementCount += _getPossibleArrangements(record, memoization, springsIdx + 2, damagedCountsIdx + 1, 0);
            }
        } else {
            // else keep going building up the sequence of '#'s for the current count
            arrangementCount += _getPossibleArrangements(record, memoization, springsIdx + 1, damagedCountsIdx, currentDamagedCount + 1);
        }
    }

    return arrangementCount;
}

void part1(const Input &input) {
    size_t sumPossibleArrangements = 0;
    for (const auto &record : input.records) {
        const auto maxCount = *max_element(record.damagedCounts.begin(), record.damagedCounts.end());
        // 3d matrix for memoization of format (springs list index, damaged counts index, size of current damaged sequence)
        // note: size of damaged counts index is in range [0, record.damagedCounts.size() + 1] because we want to take into account
        // the situation in which we finish the '#' groupings before we finish the springs list - in which case we need to iterate
        // through the springs list just to make sure there are no additional '#' left (in which case, our previous assignments would have been
        // not valid, since there are leftover '#' that are not part of any of the already accounted for groupings)
        // note: size of current damaged sequence is in the range of [0, maxCount], therefore we initialize the matrix in that dimension with
        // maxCount + 1
        mat3d_t memoization(record.springs.size(), vector<vector<int64_t>>(record.damagedCounts.size() + 1, vector<int64_t>(maxCount + 1, -1)));

        sumPossibleArrangements += _getPossibleArrangements(record, memoization, 0, 0, 0);
    }

    cout << sumPossibleArrangements << endl;
}

const Record _unfold(const Record &record) {
    Record unfoldedRecord {
        .springs = record.springs,
        .damagedCounts = record.damagedCounts,
    };

    for (size_t i = 0; i < kUnfoldFactor - 1; i++) {
        unfoldedRecord.springs += "?" + record.springs;

        for (const auto &c : record.damagedCounts) {
            unfoldedRecord.damagedCounts.push_back(c);
        }
    }

    return unfoldedRecord;
}

void part2(const Input &input) {
    size_t sumPossibleArrangements = 0;
    for (const auto &record : input.records) {
        const auto unfoldedRecord = _unfold(record);

        const auto maxCount = *max_element(unfoldedRecord.damagedCounts.begin(), unfoldedRecord.damagedCounts.end());
        mat3d_t memoization(unfoldedRecord.springs.size(), vector<vector<int64_t>>(unfoldedRecord.damagedCounts.size() + 1, vector<int64_t>(maxCount + 1, -1)));

        sumPossibleArrangements += _getPossibleArrangements(unfoldedRecord, memoization, 0, 0, 0);
    }

    cout << sumPossibleArrangements << endl;
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        std::cout << "Must specify input file!" << endl;
        return 0;
    }

    std::cout << "Input file name: " << argv[1] << endl;
    ifstream inputFile(argv[1]);

    const auto input = _parseInput(inputFile);
    // _printInput(input);

    std::cout << "Part 1: " << endl;
    part1(input);

    std::cout << "Part 2: " << endl;
    part2(input);

    inputFile.close();

    return 0;
}