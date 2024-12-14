#include "../../util/BasicIncludes.h"
#include "../../util/StringUtil.h"
#include "../../util/Types.h"
#include "../../util/MathUtil.h"

#include <regex>
#include <vector>
#include <optional>

using namespace std;
using namespace aoc::util::types;

const regex ROBOT_STATE_RX("p=([0-9]+),([0-9]+) v=(-?[0-9]+),(-?[0-9]+)");

struct RobotState {
    coord_t position;
    coord_signed_t velocity;
};

void _printRobotState(const RobotState &robotState) {
    cout << "p=(" << robotState.position.first << "," << robotState.position.second
        << ") v=(" << robotState.velocity.first << "," << robotState.velocity.second << ")" << endl;
}

void _printOccupancy(const map<coord_t, size_t> &occupancyMap, const size_t mapH, const size_t mapW, bool simple = false, bool printCross = false) {
    if (simple) {
        for (const auto &p : occupancyMap) {
            cout << "(" << p.first.first << "," << p.first.second << "): " << p.second << endl;
        }
    } else {
        for (size_t y = 0; y < mapH; ++y) {
            if (y == mapH / 2 && !printCross) {
                cout << endl;
                continue;
            }
            for (size_t x = 0; x < mapW; ++x) {
                if (x == mapW / 2 && !printCross) {
                    cout << " ";
                    continue;
                }
                const coord_t pos = {x, y};
                if (occupancyMap.contains(pos)) {
                    cout << occupancyMap.at(pos) << " ";
                } else {
                    cout << ". ";
                }
            }
            cout << endl;
        }
    }
}

const RobotState _computeState(const RobotState &currentState, const size_t mapH, const size_t mapW, const size_t numSeconds) {
    coord_signed_t currPosition = currentState.position;
    coord_signed_t displacement = {numSeconds * currentState.velocity.first,
                                    numSeconds * currentState.velocity.second};
    coord_signed_t nextPosition = {aoc::util::math::positiveModulo(currPosition.first + displacement.first, mapW),
                                    aoc::util::math::positiveModulo(currPosition.second + displacement.second, mapH)};

    return RobotState {
        .position = nextPosition,
        .velocity = currentState.velocity,
    };
}

const map<coord_t, size_t> _computeOccupancy(const vector<RobotState> &robotStates, const size_t mapH, const size_t mapW, const size_t numSeconds) {
    map<coord_t, size_t> occupancyMap;

    for (const auto &robotState : robotStates) {
        const auto newRobotState = _computeState(robotState, mapH, mapW, numSeconds);
        if (!occupancyMap.contains(newRobotState.position)) {
            occupancyMap[newRobotState.position] = 0;
        }

        occupancyMap[newRobotState.position] += 1;
    }

    return occupancyMap;
}

const uint64_t _findChristmasTree(const vector<RobotState> &robotStates, const size_t mapH, const size_t mapW) {
    uint64_t numSeconds = 1;
    bool possibleChristmasTreeFound = false;
    while (!possibleChristmasTreeFound) {
        const auto occupancyMap = _computeOccupancy(robotStates, mapH, mapW, numSeconds);
        if (occupancyMap.size() == robotStates.size()) {
            // silly heuristic - assume that we will get a christmas tree pattern when none of the robots are overlapping
            _printOccupancy(occupancyMap, mapH, mapW, false, true);
            return numSeconds;
        }
        numSeconds += 1;
    }

    return numSeconds;
}

const uint64_t _robotsInQuadrant(const map<coord_t, size_t> &occupancyMap, const coord_t xs, const coord_t ys) {
    uint64_t numRobots = 0;

    for (size_t y = ys.first; y < ys.second; ++y) {
        for (size_t x = xs.first; x < xs.second; ++x) {
            const coord_t pos = {x, y};
            if (!occupancyMap.contains(pos)) {
                continue;
            }

            numRobots += occupancyMap.at(pos);
        }
    }

    return numRobots;
}

void part1(istream& inputFile) {
    vector<RobotState> robotStates;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        smatch match;
        if (regex_match(line, match, ROBOT_STATE_RX)) {
            robotStates.emplace_back(RobotState {
                .position = {stoul(match[1]), stoul(match[2])},
                .velocity = {stoi(match[3]), stoi(match[4])},
            });
        } else {
            cout << "Could not match line: " << line << endl;
            exit(1);
        }
    }

    // for (const auto &robotState : robotStates) {
    //     _printRobotState(robotState);
    // }

    // constexpr size_t kMapW = 11;    // test1.in
    // constexpr size_t kMapH = 7;     // test1.in
    constexpr size_t kMapW = 101;
    constexpr size_t kMapH = 103;
    constexpr size_t kNumSeconds = 100;

    const auto occupancyMap = _computeOccupancy(robotStates, kMapH, kMapW, kNumSeconds);
    // _printOccupancy(occupancyMap, kMapH, kMapW);

    uint64_t safetyFactor = 1;

    safetyFactor *= _robotsInQuadrant(occupancyMap, {0, kMapW / 2}, {0, kMapH / 2});
    safetyFactor *= _robotsInQuadrant(occupancyMap, {kMapW / 2 + 1, kMapW}, {0, kMapH / 2});
    safetyFactor *= _robotsInQuadrant(occupancyMap, {0, kMapW / 2}, {kMapH / 2 + 1, kMapH});
    safetyFactor *= _robotsInQuadrant(occupancyMap, {kMapW / 2 + 1, kMapW}, {kMapH / 2 + 1, kMapH});

    cout << safetyFactor << endl;
}

void part2(istream& inputFile) {
    vector<RobotState> robotStates;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        smatch match;
        if (regex_match(line, match, ROBOT_STATE_RX)) {
            robotStates.emplace_back(RobotState {
                .position = {stoul(match[1]), stoul(match[2])},
                .velocity = {stoi(match[3]), stoi(match[4])},
            });
        } else {
            cout << "Could not match line: " << line << endl;
            exit(1);
        }
    }

    constexpr size_t kMapW = 101;
    constexpr size_t kMapH = 103;
    constexpr size_t kNumSeconds = 100;

    const auto numSecondsTilChristmasTree = _findChristmasTree(robotStates, kMapH, kMapW);

    cout << numSecondsTilChristmasTree << endl;
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