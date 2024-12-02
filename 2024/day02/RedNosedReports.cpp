#include "../../util/BasicIncludes.h"
#include "../../util/MathUtil.h"

#include <vector>
#include <optional>

using namespace std;

constexpr size_t kMinDiff = 1;
constexpr size_t kMaxDiff = 3;

void printVector(vector<size_t> v) {
    for (const auto &e : v) {
        cout << e << " ";
    }
    cout << endl;
}

struct BadLevelAnalysisResult {
    bool isSafe = false;
    vector<size_t> firstUnsafeLevelIds = vector<size_t>();
};

const BadLevelAnalysisResult checkSafety(const vector<size_t> &report) {
        if (report.size() < 3) {
            // printVector(report);
            // cout << "SAFE" << endl;
            return BadLevelAnalysisResult {
                .isSafe = true,
            };
        }

        vector<int> diffs = vector<int>(report.size() - 1);
        // check difference threshold condition
        for (size_t i = 0; i < report.size() - 1; ++i) {
            diffs[i] = report[i + 1] - report[i];
            if (abs(diffs[i]) < kMinDiff || abs(diffs[i]) > kMaxDiff) {
                // printVector(report);
                // cout << "UNSAFE (diff thresholds)" << endl;
                return BadLevelAnalysisResult {
                    .isSafe = false,
                    .firstUnsafeLevelIds = vector<size_t>{i, i + 1},
                };
            }
        }

        // check sign monotony condition
        const int expectedSign = aoc::util::math::sgn(diffs[0]);
        if (expectedSign == 0) {
            return BadLevelAnalysisResult {
                .isSafe = false,
                .firstUnsafeLevelIds = vector<size_t>{0},
            };
        }

        for (size_t i = 0; i < diffs.size(); ++i) {
            if (expectedSign != aoc::util::math::sgn(diffs[i])) {
                // printVector(level);
                // cout << "UNSAFE (monotony)" << endl;
                return BadLevelAnalysisResult {
                    .isSafe = false,
                    .firstUnsafeLevelIds = vector<size_t>{0, i, i + 1},
                }; // we want to include the very first element here too, in case the expected sign is actually wrong
            }
        }

        return BadLevelAnalysisResult {
            .isSafe = true,
        };
}

const vector<size_t> removeLevelAtIdx(const vector<size_t> &report, const size_t idxToRemove) {
    vector<size_t> adjustedReport = report;
    adjustedReport.erase(adjustedReport.begin() + idxToRemove);

    return adjustedReport;
}

void part1(istream& inputFile) {
    vector<vector<size_t>> reports;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        istringstream iss(line);
        size_t level;
        vector<size_t> levels;
        while (iss >> level) {
            levels.push_back(level);
        }

        reports.push_back(levels);
    }

    size_t numSafe = 0;
    for (const auto &report : reports) {
        const auto &result = checkSafety(report);
        if (result.isSafe) {
            numSafe += 1;
        }
    }

    cout << numSafe << endl;
}

void part2(istream& inputFile) {
        vector<vector<size_t>> reports;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        istringstream iss(line);
        size_t level;
        vector<size_t> levels;
        while (iss >> level) {
            levels.push_back(level);
        }

        reports.push_back(levels);
    }

    size_t numSafe = 0;
    for (const auto &report : reports) {
        const auto result = checkSafety(report);
        if (result.isSafe) {
            numSafe += 1;
        } else {
            for (const auto &idxToRemove : result.firstUnsafeLevelIds) {
                const auto adjustedReport = removeLevelAtIdx(report, idxToRemove);
                const auto adjustedResult = checkSafety(adjustedReport);
                if (adjustedResult.isSafe) {
                    // we found one unsafe level that can be removed, so we are done
                    numSafe += 1;
                    break;
                }
            }
        }
    }

    cout << numSafe << endl;
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