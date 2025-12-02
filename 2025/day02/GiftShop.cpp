#include "../../util/BasicIncludes.h"
#include "../../util/MathUtil.h"
#include "../../util/StringUtil.h"

#include <regex>

using namespace std;

bool isInvalidIDPart1(const string& s) {
    if (s.size() % 2 == 1) {
        return false;
    }

    const size_t midIdx = s.size() / 2;
    const size_t startIdx = s.size() % 2 == 0 ? midIdx : midIdx + 1;

    for (size_t i = 0; i < midIdx; i++) {
        const size_t leftIdx = i;
        const size_t rightIdx = midIdx + i;
        if (s[leftIdx] != s[rightIdx]) {
            return false;
        }
    }

    return true;
}

bool isInvalidIDPart2(const string& s) {
    for (size_t i = 1; i < s.size(); i++) {
        const string substr = s.substr(0, i);

        if (s.size() % substr.size() != 0) {
            continue;
        }

        string repeatedSubstr = "";
        for (size_t j = 0; j < s.size() / substr.size(); j++) {
            repeatedSubstr += substr;
        }

        if (repeatedSubstr == s) {
            return true;
        }
    }

    return false;
}

// This is SUPER slow
bool isInvalidIDPart2Regex(const string& s) {
    const regex REPEATING_PATTERN("^(\\d+)\\1+$");

    smatch match;
    if (regex_match(s, match, REPEATING_PATTERN)) {
        return true;
    }

    return false;
}

void part1(istream& inputFile) {
    uint64_t sumInvalidIDs = 0;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        const auto intervals = aoc::util::string::split(line, ',');
        for (const auto &interval : intervals) {
            if (interval.empty()) {
                continue;
            }
            const auto nums = aoc::util::string::split(interval, '-');

            // cout << interval << endl;
            auto currentID = nums[0];
            const auto endID = nums[1];
            while (currentID != endID) {
                if (isInvalidIDPart1(currentID)) {
                    // cout << currentID << endl;
                    sumInvalidIDs += stoull(currentID);
                }
                currentID = aoc::util::string::incrementStringNumber(currentID);
            }
            if (isInvalidIDPart1(currentID)) {
                sumInvalidIDs += stoull(currentID);
            }
            // cout << endl;
        }
    }

    cout << sumInvalidIDs << endl;
}

void part2(istream& inputFile) {
    uint64_t sumInvalidIDs = 0;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        const auto intervals = aoc::util::string::split(line, ',');
        for (const auto &interval : intervals) {
            if (interval.empty()) {
                continue;
            }
            const auto nums = aoc::util::string::split(interval, '-');

            // cout << interval << endl;
            auto currentID = nums[0];
            const auto endID = nums[1];
            while (currentID != endID) {
                if (isInvalidIDPart2(currentID)) {
                    // cout << currentID << endl;
                    sumInvalidIDs += stoull(currentID);
                }
                currentID = aoc::util::string::incrementStringNumber(currentID);
            }
            if (isInvalidIDPart2(currentID)) {
                sumInvalidIDs += stoull(currentID);
            }
            // cout << endl;
        }
    }

    cout << sumInvalidIDs << endl;
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