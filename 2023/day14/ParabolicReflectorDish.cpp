#include "../../util/StringUtil.h"
#include "../../util/MathUtil.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint>
#include <vector>
#include <map>
#include <optional>
#include <sstream>
#include <iterator>
#include <set>
#include <functional>

using namespace std;

constexpr size_t kCycles = 1000000000;

struct Position {
    size_t row;
    size_t col;
};

struct Input {
    vector<string> platform;    // always oriented with North at the top
    vector<Position> roundRocks;
    vector<Position> squareRocks;
    map<size_t, vector<Position>> colToRoundRocks;
    map<size_t, vector<Position>> colToSquareRocks;

    size_t nRows;
    size_t nCols;

    size_t getHash() const {
        vector<size_t> hashValues;

        // we only care about the round rocks because they are the only ones that move
        // and define the state of the platform

        // order is important when we generate the hash so we want to go column by column,
        // then row by row (sorted) and iterate through all the round rocks
        for (const auto &p : colToRoundRocks) {
            for (const auto &roundRock : p.second) {
                hashValues.push_back(roundRock.row);
                hashValues.push_back(roundRock.col);
            }
        }

        stringstream hashSS;
        copy(hashValues.begin(), hashValues.end(), ostream_iterator<size_t>(hashSS, ""));

        return hash<string>{}(hashSS.str());
    }
};

const Input _parseInput(istream& inputFile) {
    Input input;

    size_t row = 0;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        input.platform.push_back(line);
        for (size_t col = 0; col < line.size(); col++) {
            const auto ch = line[col];
            if (ch == 'O') {
                Position p = {
                    .row = row,
                    .col = col,
                };

                input.roundRocks.emplace_back(p);

                input.colToRoundRocks[col].push_back(p);
            } else if (ch == '#') {
                Position p = {
                    .row = row,
                    .col = col,
                };

                input.squareRocks.emplace_back(p);

                input.colToSquareRocks[col].push_back(p);
            }
        }

        row += 1;
    }

    input.nRows = input.platform.size();
    input.nCols = input.platform[0].size();

    return input;
}

void _printInput(const Input &input) {
    cout << "Platform: (" << input.nRows << ", " << input.nCols << ")" << endl;
    for (const auto &line : input.platform) {
        cout << line << endl;
    }
    cout << endl;

    cout << "Hash: " << endl << input.getHash() << endl << endl;

    cout << "Round Rocks: (" << input.roundRocks.size() << ")" << endl;
    for (const auto &p : input.roundRocks) {
        cout << "(" << p.row << ", " << p.col << ") ";
    }
    cout << endl << endl;

    cout << "Square Rocks: (" << input.squareRocks.size() << ")" << endl;
    for (const auto &p : input.squareRocks) {
        cout << "(" << p.row << ", " << p.col << ") ";
    }
    cout << endl << endl;

    cout << "Col to Round Rocks: " << endl;
    for (const auto &p : input.colToRoundRocks) {
        cout << p.first << " -> { ";
        for (const auto &pos : p.second) {
            cout << "(" << pos.row << ", " << pos.col << ") ";
        }
        cout << "}" << endl;
    }
    cout << endl;

    cout << "Col to Square Rocks: " << endl;
    for (const auto &p : input.colToSquareRocks) {
        cout << p.first << " -> { ";
        for (const auto &pos : p.second) {
            cout << "(" << pos.row << ", " << pos.col << ") ";
        }
        cout << "}" << endl;
    }
    cout << endl;
}

void _printRocks(const vector<Position> &roundRocks, const vector<Position> &squareRocks, const size_t nRows, const size_t nCols) {
    // DEBUG VISUALIZATION OF CYCLES
    vector<string> platform = vector(nRows, string(nCols, '.'));
    for (const auto &p : roundRocks) {
        platform[p.row][p.col] = 'O';
    }
    for (const auto &p : squareRocks) {
        platform[p.row][p.col] = '#';
    }

    for (const auto &line : platform) {
        cout << line << endl;
    }
}

optional<size_t> _getClosestSquareRockAbove(const Position &roundRockPos, const vector<Position> &squareRocks) {
    // goal: find largest (in row position) element in the list of square rocks that is smaller (in row position) than the given round rock
    if (squareRocks.empty()) {
        return nullopt;
    }

    // first, find the first square rock that is larger (in row position) than the given round rock
    vector<Position>::const_iterator it = upper_bound(squareRocks.begin(), squareRocks.end(), roundRockPos, [](const Position &a, const Position &b) {
        return a.row < b.row;
    });

    // if no such square rock exists, return null
    if (it == squareRocks.begin()) {
        return nullopt;
    } else {
        // move back iterator once to get to the largest (in row position) square rock that is smaller (in row position) than the given round rock
        it--;
    }

    return it->row;
}

vector<Position> _rollNorth(const Input &input) {
    vector<Position> newRoundRockPositions;
    for (const auto &p : input.colToRoundRocks) {
        const auto col = p.first;
        const auto roundRocks = p.second;
        const vector<Position> squareRocks = input.colToSquareRocks.contains(col) ? input.colToSquareRocks.at(col) : vector<Position>();

        map<size_t, size_t> squareRockRowToRoundRocksAffected;
        for (const auto &roundRockPos : roundRocks) {
            const auto squareRockOffsetOpt = _getClosestSquareRockAbove(roundRockPos, squareRocks);
            const auto squareRockOffset = squareRockOffsetOpt ? *squareRockOffsetOpt + 1 : 0;

            const auto roundRocksAffectedBySquareRock = squareRockRowToRoundRocksAffected.contains(squareRockOffset) ? squareRockRowToRoundRocksAffected.at(squareRockOffset) : 0;
            const auto roundRockRollRow = squareRockOffset + roundRocksAffectedBySquareRock;
            squareRockRowToRoundRocksAffected[squareRockOffset] += 1;

            newRoundRockPositions.push_back(Position {
                .row = roundRockRollRow,
                .col = col,
            });
        }
    }

    return newRoundRockPositions;
}

size_t _computeLoad(const vector<Position> &roundRockPositions, const size_t nRows) {
    size_t totalLoad = 0;
    for (const auto &newPos : roundRockPositions) {
        totalLoad += (nRows - newPos.row);
    }

    return totalLoad;
}

void part1(const Input &input) {
    vector<Position> newRoundRockPositions = _rollNorth(input);

    const auto totalLoad = _computeLoad(newRoundRockPositions, input.nRows);

    cout << totalLoad << endl;
}

Input _rotateInputCW(const Input &input, const vector<Position> &newRoundRockPositions) {
    Input rotatedInput = {
        .platform = input.platform,
        .nRows = input.nCols,
        .nCols = input.nRows,
    };

    // swap coords of rocks
    for (const auto &newPosition : newRoundRockPositions) {
        Position rotatedPosition = {
            .row = newPosition.col,
            .col = input.nRows - 1 - newPosition.row,
        };

        rotatedInput.roundRocks.push_back(rotatedPosition);
        rotatedInput.colToRoundRocks[rotatedPosition.col].push_back(rotatedPosition);
    }

    for (const auto &newPosition : input.squareRocks) {
        Position rotatedPosition = {
            .row = newPosition.col,
            .col = input.nRows - 1 - newPosition.row,
        };

        rotatedInput.squareRocks.push_back(rotatedPosition);
        rotatedInput.colToSquareRocks[rotatedPosition.col].push_back(rotatedPosition);
    }

    // sort rocks
    sort(rotatedInput.roundRocks.begin(), rotatedInput.roundRocks.end(), [](const Position &p1, const Position &p2) {
        return p1.row < p2.row;
    });

    sort(rotatedInput.squareRocks.begin(), rotatedInput.squareRocks.end(), [](const Position &p1, const Position &p2) {
        return p1.row < p2.row;
    });

    // sort based on the new rows
    for (auto &p : rotatedInput.colToRoundRocks) {
        sort(p.second.begin(), p.second.end(), [](const Position &p1, const Position &p2) {
            return p1.row < p2.row;
        });
    }

    for (auto &p : rotatedInput.colToSquareRocks) {
        sort(p.second.begin(), p.second.end(), [](const Position &p1, const Position &p2) {
            return p1.row < p2.row;
        });
    }

    return rotatedInput;
}

void part2(const Input &input) {
    Input currentOrientationInput = input; // start with the north orientation
    vector<size_t> previouslySeenLoads;
    map<size_t, size_t> previouslySeenInputHashToIteration;

    size_t periodStart = 0;
    size_t periodLength = 0;
    for (size_t i = 0; i < kCycles; i++) { // DEBUG: repeating after 152
        // Roll in all 4 directions by rotating the positions clockwise and rolling north / upwards always
        for (size_t rollId = 0; rollId < 4; rollId++) {
            const auto newRoundRockPositions = _rollNorth(currentOrientationInput);
            currentOrientationInput = _rotateInputCW(currentOrientationInput, newRoundRockPositions);
        }

        const auto inputHash = currentOrientationInput.getHash();
        if (previouslySeenInputHashToIteration.contains(inputHash)) {
            periodStart = previouslySeenInputHashToIteration.at(inputHash);
            periodLength = i - periodStart;
            break;
        }

        previouslySeenInputHashToIteration[inputHash] = i;
        const auto currentCycleLoad = _computeLoad(currentOrientationInput.roundRocks, currentOrientationInput.nRows);
        previouslySeenLoads.push_back(currentCycleLoad);
    }

    size_t totalLoad = previouslySeenLoads[(((kCycles - 1) - periodStart) % periodLength) + periodStart];

    cout << totalLoad << endl;
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