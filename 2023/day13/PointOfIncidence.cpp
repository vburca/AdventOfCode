#include "../../util/StringUtil.h"
#include "../../util/MathUtil.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint>
#include <vector>
#include <optional>

using namespace std;

struct Pattern {
    vector<string> map;
    vector<string> transposeMap;
    size_t rows;
    size_t cols;
};

struct LineEqualOutput {
    bool areEqual;
    bool haveSmudge;
};

struct Input {
    vector<Pattern> patterns;
    vector<Pattern> transposePatterns;
};

void _transposePattern(Pattern &pattern) {
    pattern.transposeMap = vector(pattern.cols, string(pattern.rows, '?'));

    for (size_t row = 0; row < pattern.rows; row++) {
        for (size_t col = 0; col < pattern.cols; col++) {
            pattern.transposeMap[col][row] = pattern.map[row][col];
        }
    }
}

const Input _parseInput(istream& inputFile) {
    Input input;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        Pattern p;
        p.map.push_back(line);
        while (!inputFile.eof()) {
            getline(inputFile, line);
            if (line.empty()) {
                break;
            }

            p.map.push_back(line);
        }

        p.rows = p.map.size();
        p.cols = p.map[0].size();

        input.patterns.emplace_back(p);
    }

    for (auto &p : input.patterns) {
        _transposePattern(p);
    }

    return input;
}

void _printInput(const Input &input) {
    cout << "Patterns: " << endl;
    for (const auto &p : input.patterns) {
        for (const auto &l : p.map) {
            cout << l << endl;
        }
        cout << "(" << p.rows << ", " << p.cols << ")" << endl << endl;

        cout << "Transpose: " << endl;
        for (const auto &l : p.transposeMap) {
            cout << l << endl;
        }
        cout << "(" << p.cols << ", " << p.rows << ")" << endl << endl;
    }
}

LineEqualOutput _areLinesEqual(const string &lineUp, const string &lineDown) {
    size_t diffChars = 0;
    for (size_t i = 0; i < lineUp.size(); i++) {
        const auto charUp = lineUp[i];
        const auto charDown = lineDown[i];

        diffChars += (charUp != charDown);
        if (diffChars > 1) {
            return {
                .areEqual = false,
                .haveSmudge = false,
            };
        }
    }

    return {
        .areEqual = (diffChars == 0),
        .haveSmudge = (diffChars == 1),
    };
}

// check mirror position right below of the `horizontalIdx` row
bool _isMirrored(const vector<string> &map, const size_t horizontalIdx, const bool findSmudges = false) {
    if (horizontalIdx >= map.size() - 1) {
        return false;
    }

    int32_t topSideIdx = horizontalIdx - 0;
    int32_t botSideIdx = horizontalIdx + 1;

    bool foundSmudgeAlready = false;

    while (topSideIdx >= 0 && botSideIdx < map.size()) {
        const auto equalOutput = _areLinesEqual(map[topSideIdx], map[botSideIdx]);

        if (!findSmudges && !equalOutput.areEqual) {
            return false;
        } else if (findSmudges) {
            if (!equalOutput.areEqual && !equalOutput.haveSmudge) {
                return false;
            } else if (equalOutput.haveSmudge) {
                if (foundSmudgeAlready) {
                    return false;
                }

                foundSmudgeAlready = true;
            }
        }

        topSideIdx -= 1;
        botSideIdx += 1;
    }

    if (!findSmudges) {
        return true;
    } else {
        return foundSmudgeAlready;
    }
}

optional<size_t> _getMirrorLocationHorizontalAxis(const vector<string> &map, const bool findSmudges = false) {
    for (size_t i = 0; i < map.size() - 1; i++) {
        if (_isMirrored(map, i, findSmudges)) {
            return i;
        }
    }

    return nullopt;
}

void part1(const Input &input) {
    size_t sumMirrorPositionsRows = 0;
    size_t sumMirrorPositionsCols = 0;
    for (const auto &p : input.patterns) {
        const auto rowMirrorOpt = _getMirrorLocationHorizontalAxis(p.map);
        const auto colMirrorOpt = _getMirrorLocationHorizontalAxis(p.transposeMap);

        const auto rowMirror = rowMirrorOpt ? *rowMirrorOpt + 1 : 0;
        const auto colMirror = colMirrorOpt ? *colMirrorOpt + 1 : 0;

        sumMirrorPositionsRows += rowMirror;
        sumMirrorPositionsCols += colMirror;
    }

    const auto sumMirrorPositions = 100 * sumMirrorPositionsRows + sumMirrorPositionsCols;
    cout << sumMirrorPositions << endl;
}

void part2(const Input &input) {
    size_t sumMirrorPositionsRows = 0;
    size_t sumMirrorPositionsCols = 0;
    for (const auto &p : input.patterns) {
        const auto rowMirrorOpt = _getMirrorLocationHorizontalAxis(p.map, true);
        const auto colMirrorOpt = _getMirrorLocationHorizontalAxis(p.transposeMap, true);

        const auto rowMirror = rowMirrorOpt ? *rowMirrorOpt + 1 : 0;
        const auto colMirror = colMirrorOpt ? *colMirrorOpt + 1 : 0;

        sumMirrorPositionsRows += rowMirror;
        sumMirrorPositionsCols += colMirror;
    }

    const auto sumMirrorPositions = 100 * sumMirrorPositionsRows + sumMirrorPositionsCols;
    cout << sumMirrorPositions << endl;
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