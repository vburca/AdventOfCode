#include "../../util/BasicIncludes.h"
#include "../../util/MathUtil.h"
#include "../../util/StringUtil.h"

#include <algorithm>
#include <iterator>

using namespace std;

void part1_orig(istream& inputFile) {
    uint64_t totalOutputJoltage = 0;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        vector<unsigned short int> batteries;
        for (const auto &ch : line) {
            batteries.push_back(ch - '0');
        }

        vector<unsigned short int> largestBatteryToRight(batteries.size(), 0);
        largestBatteryToRight[batteries.size() - 1] = batteries.back();
        for (int i = batteries.size() - 2; i >= 0; i--) {
            largestBatteryToRight[i] = max(largestBatteryToRight[i + 1], batteries[i + 1]);
        }

        const auto maxBatteryIterator = max_element(batteries.begin(), batteries.end() - 1);
        const auto maxBatteryIdx = distance(batteries.begin(), maxBatteryIterator);

        // cout << "maxBatteryIdx= " << maxBatteryIdx << endl;

        const auto currentJoltage = 10 * batteries[maxBatteryIdx] + largestBatteryToRight[maxBatteryIdx];
        // cout << "currentJoltage= " << currentJoltage << endl << endl;
        totalOutputJoltage += currentJoltage;
    }

    cout << totalOutputJoltage << endl;
}

const size_t maxIndexOfInRange(const vector<unsigned short int> batteries, size_t startIdx, size_t endIdx) {
    int max = -1;
    size_t maxIdx = 0;

    for (size_t i = startIdx; i < endIdx; i++) {
        if (batteries[i] > max) {
            maxIdx = i;
            max = batteries[i];
        }
    }

    return maxIdx;
}

const uint64_t calculateMaxJoltage(const vector<unsigned short int> batteries, const size_t numDigits) {
    uint64_t currentJoltage = 0;

    int digitIdx = -1;
    for (int remainingDigits = numDigits - 1; remainingDigits >= 0; remainingDigits--) {
        digitIdx = maxIndexOfInRange(batteries, digitIdx + 1, batteries.size() - remainingDigits);
        currentJoltage = currentJoltage * 10 + batteries[digitIdx];
    }

    return currentJoltage;
}

void part1(istream& inputFile) {
    uint64_t totalOutputJoltage = 0;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        vector<unsigned short int> batteries;
        for (const auto &ch : line) {
            batteries.push_back(ch - '0');
        }

        const auto currentJoltage = calculateMaxJoltage(batteries, 2);
        // cout << "currentJoltage= " << currentJoltage << endl << endl;
        totalOutputJoltage += currentJoltage;
    }

    cout << totalOutputJoltage << endl;
}

void part2(istream& inputFile) {
    uint64_t totalOutputJoltage = 0;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        vector<unsigned short int> batteries;
        for (const auto &ch : line) {
            batteries.push_back(ch - '0');
        }

        const auto currentJoltage = calculateMaxJoltage(batteries, 12);
        // cout << "currentJoltage= " << currentJoltage << endl << endl;
        totalOutputJoltage += currentJoltage;
    }

    cout << totalOutputJoltage << endl;
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