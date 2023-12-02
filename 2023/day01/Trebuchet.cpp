#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <utility>
#include <optional>

using namespace std;

void part1(istream& inputFile) {
    size_t sum = 0;
    while (!inputFile.eof())
    {
        vector<size_t> digits;

        string line;
        getline(inputFile, line);

        for (const auto &ch : line) {
            if (!isdigit(ch)) {
                continue;
            }

            digits.push_back(static_cast<size_t>(ch - '0'));
        }

        if (digits.size() < 1) {
            std::cout << "Not enough digits (" << digits.size() << ") found in line: " << line << endl;
            continue;
        }

        size_t calibrationValue = 10 * digits[0] + digits[digits.size() - 1];
        if (digits.size() > 1) {
            calibrationValue = (calibrationValue / 10) * 10 + digits[digits.size() - 1];
        }

        sum += calibrationValue;
    }

    std::cout << sum << endl;
}

optional<pair<size_t, size_t>> getFirstLastIdxForAlphaNumber(const string &alphaNumber, const string &line) {
    if (line.size() < alphaNumber.size()) {
        return nullopt;
    }

    size_t firstIdx = 0;
    size_t lastIdx = 0;
    bool found = false;
    for (size_t i = 0; i < line.size() - alphaNumber.size() + 1; i++) {
        if (line.substr(i, alphaNumber.size()) == alphaNumber) {
            firstIdx = found ? firstIdx : i;  // update firstIdx only if we haven't found this before
            found = true;
            lastIdx = i;
        }
    }

    if (!found) {
        return nullopt;
    }

    return make_pair(firstIdx, lastIdx);
}

void part2(istream& inputFile) {
    struct OccurenceType {
        int idx;
        size_t digit;
    };

    const unordered_map<string, size_t> kAlphaToNum = {
        {"one", 1},
        {"two", 2},
        {"three", 3},
        {"four", 4},
        {"five", 5},
        {"six", 6},
        {"seven", 7},
        {"eight", 8},
        {"nine", 9},
    };

    size_t sum = 0;
    while (!inputFile.eof())
    {
        vector<size_t> digits;

        string line;
        getline(inputFile, line);

        OccurenceType firstOccurence = {
            .idx = INT_MAX,
        };
        OccurenceType lastOccurence = {
            .idx = INT_MIN,
        };

        bool isFirstFoundAlpha = true;
        for (const auto &alphaNum : kAlphaToNum) {
            const auto firstLastPos = getFirstLastIdxForAlphaNumber(alphaNum.first, line);
            if (!firstLastPos) {
                continue;
            }

            if ((int)(*firstLastPos).first < firstOccurence.idx) {
                firstOccurence = {
                    .idx = (int)(*firstLastPos).first,
                    .digit = alphaNum.second,
                };
            }

            if ((int)(*firstLastPos).second > lastOccurence.idx) {
                lastOccurence = {
                    .idx = (int)(*firstLastPos).second,
                    .digit = alphaNum.second,
                };
            }
        }

        for (size_t idx = 0; idx < line.size(); idx++) {
            const auto &ch = line.at(idx);
            const auto digit = static_cast<size_t>(ch - '0');
            if (!isdigit(ch)) {
                continue;
            }

            if ((int)idx < firstOccurence.idx) {
                firstOccurence = {
                    .idx = (int)idx,
                    .digit = digit,
                };
            }

            if ((int)idx > lastOccurence.idx) {
                lastOccurence = {
                    .idx = (int)idx,
                    .digit = digit,
                };
            }
        }

        // std::cout << "first digit: " << firstOccurence.digit << " idx: " << firstOccurence.idx << endl;
        // std::cout << "last digit: " << lastOccurence.digit << " idx: " << lastOccurence.idx << endl;

        size_t calibrationValue = 10 * firstOccurence.digit + lastOccurence.digit;

        sum += calibrationValue;
    }

    std::cout << sum << endl;
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