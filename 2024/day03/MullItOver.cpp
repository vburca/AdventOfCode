#include "../../util/BasicIncludes.h"
#include "../../util/MathUtil.h"

#include <vector>
#include <regex>

using namespace std;

const regex MUL_RX("mul\\(([0-9]+),([0-9]+)\\)");
const regex MUL_RX_AUX("(do\\(\\)|don't\\(\\)|mul\\(([0-9]+),([0-9]+)\\))");


void part1(istream& inputFile) {
    uint64_t result = 0;

    vector<pair<uint64_t, uint64_t>> mulNumbers;

    smatch match;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        string remainingLine = line;
        while (regex_search(remainingLine, match, MUL_RX)) {
            stringstream num1Str(match[1].str());
            stringstream num2Str(match[2].str());
            uint64_t num1;
            uint64_t num2;
            num1Str >> num1;
            num2Str >> num2;

            // cout << "(" << num1 << ", " << num2 << ")" << endl;

            mulNumbers.emplace_back(make_pair(num1, num2));

            remainingLine = match.suffix(); // assign the line portion remaining, after the match
        }
    }

    for (const auto &nums : mulNumbers) {
        result += nums.first * nums.second;
    }

    cout << result << endl;
}

void part2_orig(istream& inputFile) {
    uint64_t result = 0;

    vector<pair<uint64_t, uint64_t>> mulNumbers;

    bool isEnabled = true;
    smatch match;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        string remainingLine = line;
        while (regex_search(remainingLine, match, MUL_RX)) {
            // find any previous do or don't
            const string prefixStr = match.prefix().str();
            const auto previousDoPos = prefixStr.rfind("do()");
            const auto previousDontPos = prefixStr.rfind("don't()");

            if (previousDoPos != string::npos && previousDontPos != string::npos) {
                isEnabled = (previousDoPos > previousDontPos);
            } else if (previousDoPos != string::npos) {
                isEnabled = true;
            } else if (previousDontPos != string::npos) {
                isEnabled = false;
            }

            // cout << "prefixStr= " << prefixStr << endl;
            // cout << "previousDoPos= " << previousDoPos << endl;
            // cout << "previousDontPos= " << previousDontPos << endl;
            // cout << "isEnabled= " << isEnabled << endl;

            if (isEnabled) {
                stringstream num1Str(match[1].str());
                stringstream num2Str(match[2].str());
                uint64_t num1;
                uint64_t num2;
                num1Str >> num1;
                num2Str >> num2;

                // cout << "(" << num1 << ", " << num2 << ")" << endl;

                mulNumbers.emplace_back(make_pair(num1, num2));
            }

            remainingLine = match.suffix(); // assign the line portion remaining, after the match
            // cout << endl;
        }
    }

    for (const auto &nums : mulNumbers) {
        result += nums.first * nums.second;
    }

    cout << result << endl;
}

void part2(istream& inputFile) {
    uint64_t result = 0;

    vector<pair<uint64_t, uint64_t>> mulNumbers;

    bool isEnabled = true;
    smatch match;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        string remainingLine = line;
        while (regex_search(remainingLine, match, MUL_RX_AUX)) {
            if (match[0] == "do()") {
                isEnabled = true;
                remainingLine = match.suffix();
                continue;
            } else if (match[0] == "don't()") {
                isEnabled = false;
                remainingLine = match.suffix();
                continue;
            }

            if (isEnabled) {
                stringstream num1Str(match[2].str());
                stringstream num2Str(match[3].str());
                uint64_t num1;
                uint64_t num2;
                num1Str >> num1;
                num2Str >> num2;

                // cout << "(" << num1 << ", " << num2 << ")" << endl;

                mulNumbers.emplace_back(make_pair(num1, num2));
            }

            remainingLine = match.suffix(); // assign the line portion remaining, after the match
            // cout << endl;
        }
    }

    for (const auto &nums : mulNumbers) {
        result += nums.first * nums.second;
    }

    cout << result << endl;
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