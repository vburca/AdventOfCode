#include "../../util/StringUtil.h"
#include "../../util/MathUtil.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint>
#include <vector>

using namespace std;

struct Input {
    vector<vector<int64_t>> histories;
};

void _printInput(const Input &input) {
    for (const auto &history : input.histories) {
        for (const auto &v : history) {
            cout << v << " ";
        }
        cout << endl;
    }
}

const Input _parseInput(istream& inputFile) {
    Input input;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        input.histories.push_back(aoc::util::string::parseDelimSeparatedNumbers<int64_t>(line, ' '));
    }

    return input;
}

void part1(const Input &input) {
    int64_t extrapolatedValuesSum = 0;

    for (const auto &history : input.histories) {
        bool done = false;

        vector<int64_t> extrapolationHelpers;
        vector<int64_t> currHistory = history;
        int64_t diff = 0;
        while (!done) {
            extrapolationHelpers.push_back(currHistory[currHistory.size() - 1]);

            vector<int64_t> diffs;
            done = true; // assume all the differences are going to be the same
            for (size_t i = 0; i < currHistory.size() - 1; i++) {
                diff = currHistory[i + 1] - currHistory[i];
                // the moment we find a difference that is not the same, we know that we will need to go one more level down
                if (diffs.size() > 0 && diffs[diffs.size() - 1] != diff) {
                    done = false;
                }

                diffs.push_back(diff);
            }

            if (!done) {
                currHistory = diffs;
            }
        }

        // we are done, so we have the final difference; calculate the extrapolation sum
        extrapolatedValuesSum += diff;
        for (const auto &ev : extrapolationHelpers) {
            extrapolatedValuesSum += ev;
        }
    }

    cout << extrapolatedValuesSum << endl;
}

void part2(const Input &input) {
    int64_t extrapolatedValuesSum = 0;

    for (const auto &history : input.histories) {
        bool done = false;

        vector<int64_t> extrapolationHelpers;
        vector<int64_t> currHistory = history;
        int64_t diff = 0;
        while (!done) {
            extrapolationHelpers.push_back(currHistory[0]);

            vector<int64_t> diffs;
            done = true; // assume all the differences are going to be the same
            for (size_t i = 0; i < currHistory.size() - 1; i++) {
                diff = currHistory[i + 1] - currHistory[i];
                // the moment we find a difference that is not the same, we know that we will need to go one more level down
                if (diffs.size() > 0 && diffs[diffs.size() - 1] != diff) {
                    done = false;
                }

                diffs.push_back(diff);
            }

            if (!done) {
                currHistory = diffs;
            }
        }

        // we are done, so we have the final difference; calculate the extrapolation sum
        int64_t extrapolatedValue = diff;
        reverse(extrapolationHelpers.begin(), extrapolationHelpers.end());
        for (const auto &ev : extrapolationHelpers) {
            extrapolatedValue = ev - extrapolatedValue;
        }
        extrapolatedValuesSum += extrapolatedValue;
    }

    cout << extrapolatedValuesSum << endl;
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