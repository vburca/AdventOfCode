#include "../../util/BasicIncludes.h"
#include "../../util/StringUtil.h"
#include "../../util/Types.h"

#include <vector>
#include <optional>
#include <stack>
#include <set>

using namespace std;
using namespace aoc::util::types;

const string XMAS = "XMAS";

const vector<coord_t> DIRECTIONS = {
    {-1, 0},  // up
    {-1, 1},  // up-right
    {0, 1},   // right
    {1, 1},   // down-right
    {1, 0},   // down
    {1, -1},  // down-left
    {0, -1},  // left
    {-1, -1}, // up-left
};

const vector<coord_t> FIRST_DIAG_DIRECTIONS = {
    {-1, -1}, // up-left
    {1, 1},   // down-right
};

const vector<coord_t> SECOND_DIAG_DIRECTIONS = {
    {-1, 1},  // up-right
    {1, -1},  // down-left
};

const set<vector<coord_t>> DIAGS = { FIRST_DIAG_DIRECTIONS, SECOND_DIAG_DIRECTIONS };

struct SearchState {
    size_t letterIdx = 0;
    coord_t position;
    optional<coord_t> direction;
    set<coord_t> visitedPositions;
};

const optional<coord_t> getNextPosition(const coord_t &currentPosition, const coord_t &direction, const vector<string> &letterMap) {
    const auto neighborPosition = make_pair(currentPosition.first + direction.first,
                                            currentPosition.second + direction.second);

    // check if new position is within map bounds
    if (neighborPosition.first < 0 || neighborPosition.first >= letterMap.size() ||
        neighborPosition.second < 0 || neighborPosition.second >= letterMap.front().size()) {
        return nullopt;
    }

    return neighborPosition;
}

const optional<SearchState> isDirectionValid(const SearchState &currentState, const coord_t &direction, const vector<string> &letterMap, const string &searchWord) {
    const auto neighborPositionOpt = getNextPosition(currentState.position, direction, letterMap);
    if (!neighborPositionOpt) {
        return nullopt;
    }

    const auto neighborPosition = *neighborPositionOpt;

    // check if we've already been in that position
    if (currentState.visitedPositions.contains(neighborPosition)) {
        return nullopt;
    }

    // check if this position has a letter we are interested in
    const auto nextLetterIdx = currentState.letterIdx + 1;
    const auto desiredLetter = searchWord[nextLetterIdx];
    if (letterMap[neighborPosition.first][neighborPosition.second] != desiredLetter) {
        return nullopt;
    }

    SearchState nextState = {
        .letterIdx = nextLetterIdx,
        .position = neighborPosition,
        .direction = direction,
        .visitedPositions = currentState.visitedPositions,
    };
    nextState.visitedPositions.insert(nextState.position);

    return nextState;
}

size_t countWordDFS(const vector<string> &letterMap, const string &searchWord, const vector<coord_t> &startPositions) {
    stack<SearchState> statesToVisit;

    for (const auto &startPosition : startPositions) {
        set<coord_t> visitedPositions { startPosition };
        statesToVisit.push(SearchState {
            .letterIdx = 0,
            .position = startPosition,
            .direction = nullopt,
            .visitedPositions = visitedPositions,
        });
    }

    size_t wordCount = 0;
    while (!statesToVisit.empty()) {
        const auto currentState = statesToVisit.top();
        statesToVisit.pop();

        if (currentState.letterIdx == searchWord.size() - 1) {
            wordCount += 1;
            continue;
        }

        if (!currentState.direction) {
            // find the next letter in all directions
            for (const auto &direction : DIRECTIONS) {
                const auto nextStateOpt = isDirectionValid(currentState, direction, letterMap, searchWord);
                if (!nextStateOpt) {
                    continue;
                }

                statesToVisit.push(*nextStateOpt);
            }
        } else {
            // we force a specific direction and only look in that direction
            const auto nextStateOpt = isDirectionValid(currentState, *currentState.direction, letterMap, searchWord);
            if (!nextStateOpt) {
                continue;
            }

            statesToVisit.push(*nextStateOpt);
        }
    }

    return wordCount;
}

void part1(istream& inputFile) {
    vector<string> letters;
    vector<coord_t> startPositions;

    size_t rowIdx = 0;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        letters.push_back(line);

        const auto colIds = aoc::util::string::findAllPositions(line, XMAS[0]);
        for (const auto &colIdx : colIds) {
            startPositions.emplace_back(make_pair(rowIdx, colIdx));
        }

        rowIdx += 1;
    }

    cout << countWordDFS(letters, XMAS, startPositions) << endl;
}

void part2(istream& inputFile) {
    vector<string> letters;
    vector<coord_t> startPositions;

    size_t rowIdx = 0;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        letters.push_back(line);

        const auto colIds = aoc::util::string::findAllPositions(line, XMAS[2]);
        for (const auto &colIdx : colIds) {
            startPositions.emplace_back(make_pair(rowIdx, colIdx));
        }

        rowIdx += 1;
    }

    size_t xmasCount = 0;
    for (const auto &position : startPositions) {
        bool badXmas = false;
        for (const auto &diagDirections : DIAGS) {
            if (badXmas) {
                break;
            }

            set<char> requiredLetters = {'M', 'S'};
            for (const auto &direction : diagDirections) {
                const auto nextPositionOpt = getNextPosition(position, direction, letters);
                if (!nextPositionOpt) {
                    badXmas = true;
                    break;
                }

                const auto nextPosition = *nextPositionOpt;
                const auto nextPositionLetter = letters[nextPosition.first][nextPosition.second];
                if (!requiredLetters.contains(nextPositionLetter)) {
                    badXmas = true;
                    break;
                }

                requiredLetters.erase(nextPositionLetter);
            }
        }

        if (!badXmas) {
            xmasCount += 1;
        }
    }

    cout << xmasCount << endl;
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