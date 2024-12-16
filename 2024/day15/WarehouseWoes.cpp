#include "../../util/BasicIncludes.h"
#include "../../util/StringUtil.h"
#include "../../util/Types.h"
#include "../../util/MathUtil.h"
#include "../../util/Graph.h"

#include <vector>
#include <optional>
#include <set>
#include <bits/stdc++.h>

using namespace std;
using namespace aoc::util::types;
using namespace aoc::util::graph;

struct State {
    coord_t robotPosition;
    vector<string> warehouse;
    vector<coord_signed_t> robotMoves;
    size_t currentRobotDirIdx = 0;

    const size_t w() const {
        return warehouse.front().size();
    }

    const size_t h() const {
        return warehouse.size();
    }
};

void _printMove(const coord_signed_t &move) {
    if (move == kCompassDirections4[kCompass4Orientation::kNorth]) {
        cout << '^';
    } else if (move == kCompassDirections4[kCompass4Orientation::kEast]) {
        cout << '>';
    } else if (move == kCompassDirections4[kCompass4Orientation::kSouth]) {
        cout << 'v';
    } else if (move == kCompassDirections4[kCompass4Orientation::kWest]) {
        cout << '<';
    } else {
        cout << "Unknown move: ()" << move.first << "," << move.second << ")" << endl;
        exit(1);
    }
}

void _printState(const State &s, bool printMoves = false) {
    for (size_t i = 0; i < s.h(); ++i) {
        for (size_t j = 0; j < s.w(); ++j) {
            if (s.robotPosition == make_pair(i, j)) {
                cout << '@';
            } else {
                cout << s.warehouse[i][j];
            }
        }
        cout << endl;
    }

    cout << endl;

    if (printMoves) {
        for (const auto &move : s.robotMoves) {
            _printMove(move);
        }

        cout << endl;
    }
}

const uint64_t _computeGPS(const State &s, bool isPart2 = false) {
    uint64_t sumGPS = 0;
    for (size_t i = 0; i < s.h(); ++i) {
        for (size_t j = 0; j < s.w(); ++j) {
            if ((s.warehouse[i][j] != 'O' && !isPart2) ||
                (s.warehouse[i][j] != '[' && isPart2)) {
                continue;
            }
            sumGPS += 100 * i + j;
        }
    }

    return sumGPS;
}

void _pushMovePart1(State &s, const coord_signed_t &move, const coord_t &nextPosition) {
    coord_t scanPosition = nextPosition;
    char warehouseSpot = s.warehouse[scanPosition.first][scanPosition.second];
    while (warehouseSpot == 'O') {
        scanPosition = {scanPosition.first + move.first, scanPosition.second + move.second};
        warehouseSpot = s.warehouse[scanPosition.first][scanPosition.second];
    }

    if (warehouseSpot == '#') {
        // we can not push the boxes
        return;
    }

    // we can push the boxes, so push them, moving backwards to where we currently are
    s.robotPosition = nextPosition;
    s.warehouse[nextPosition.first][nextPosition.second] = '.';
    s.warehouse[scanPosition.first][scanPosition.second] = 'O';
}

enum VerticalScanState {
    kMoreBoxes = 0,
    kBlocked,
    kPush,
};

struct VerticalScanResult {
    VerticalScanState state;
    set<coord_t> scannedBoxes;
};

const VerticalScanResult _verticalScan(const State &s, const coord_signed_t &move, const coord_t &currentPosition) {
    VerticalScanResult result;

    coord_t verticalNeighborToCheck = {currentPosition.first + move.first, currentPosition.second + move.second};
    if (s.warehouse[verticalNeighborToCheck.first][verticalNeighborToCheck.second] == '#') {
        result.state = kBlocked;
        return result;
    }

    if (s.warehouse[verticalNeighborToCheck.first][verticalNeighborToCheck.second] == '.') {
        result.state = kPush;
        return result;
    }

    // we know that there is a box above; we need to insert both its halves into the scanned boxes positions,
    // for future iterations of the scan
    result.state = kMoreBoxes;
    result.scannedBoxes.insert(verticalNeighborToCheck);
    if (s.warehouse[verticalNeighborToCheck.first][verticalNeighborToCheck.second] == '[') {
        // other half of the box is to the right
        result.scannedBoxes.insert({verticalNeighborToCheck.first, verticalNeighborToCheck.second + 1});
        return result;
    }

    if (s.warehouse[verticalNeighborToCheck.first][verticalNeighborToCheck.second] == ']') {
        // other half of the box is to the right
        result.scannedBoxes.insert({verticalNeighborToCheck.first, verticalNeighborToCheck.second - 1});
        return result;
    }

    cout << "Unknown vertical neighbor at: (" << verticalNeighborToCheck.first << "," << verticalNeighborToCheck.second << ")= "
        << s.warehouse[verticalNeighborToCheck.first][verticalNeighborToCheck.second] << endl;
    exit(1);

    return result;
}

void _pushMovePart2(State &s, const coord_signed_t &move, const coord_t &nextPosition) {
    if (move == kCompassDirections4[kCompass4Orientation::kEast] ||
        move == kCompassDirections4[kCompass4Orientation::kWest]) {
        // perform regular push

        // find the next free space
        coord_t scanPosition = nextPosition;
        char warehouseSpot = s.warehouse[scanPosition.first][scanPosition.second];
        while (warehouseSpot == '[' || warehouseSpot == ']') {
            scanPosition = {scanPosition.first + move.first, scanPosition.second + move.second};
            warehouseSpot = s.warehouse[scanPosition.first][scanPosition.second];
        }

        if (warehouseSpot == '#') {
            // we can not push the boxes
            return;
        }

        // we can push the boxes, so push them, moving backwards to where we currently are
        s.robotPosition = nextPosition;
        s.warehouse[nextPosition.first][nextPosition.second] = '.';
        if (move == kCompassDirections4[kCompass4Orientation::kEast]) {
            s.warehouse[scanPosition.first][scanPosition.second] = ']';
        } else if (move == kCompassDirections4[kCompass4Orientation::kWest]) {
            s.warehouse[scanPosition.first][scanPosition.second] = '[';
        }

        // swap the box edges
        coord_t boxPosition = {s.robotPosition.first + move.first, s.robotPosition.second + move.second};
        while (boxPosition != scanPosition) {
            if (s.warehouse[boxPosition.first][boxPosition.second] == '[') {
                s.warehouse[boxPosition.first][boxPosition.second] = ']';
            } else {
                s.warehouse[boxPosition.first][boxPosition.second] = '[';
            }

            boxPosition = {boxPosition.first + move.first, boxPosition.second + move.second};
        }

        return;
    }

    vector<set<coord_t>> boxesToPush;

    set<coord_t> currentBoxPositionsToScanFrom = {s.robotPosition};
    VerticalScanState aggregatedScanResult = kMoreBoxes;
    while (aggregatedScanResult == kMoreBoxes) {
        set<coord_t> nextBoxPositionsToScanFrom;
        bool allReadyToPush = true;
        for (const auto &boxPosition : currentBoxPositionsToScanFrom) {
            auto verticalScanResult = _verticalScan(s, move, boxPosition);

            // if we are blocked on pushing this box, we can stop processing
            if (verticalScanResult.state == kBlocked) {
                allReadyToPush = false;
                aggregatedScanResult = kBlocked;
                // we immediately stop
                return;
            }

            // if there is nothing above this box, we can push this "branch" vertically
            if (verticalScanResult.state == kPush) {
                continue;
            }

            // if there are other boxes on top of this box, we have to continue
            if (verticalScanResult.state == kMoreBoxes) {
                allReadyToPush = false;
                for (const auto &nextBoxPos : verticalScanResult.scannedBoxes) {
                    nextBoxPositionsToScanFrom.insert(nextBoxPos);
                }
            }
        }

        if (aggregatedScanResult == kBlocked) {
            // WE SHOULD NOT HAVE GOTTEN HERE
            return;
        }

        aggregatedScanResult = allReadyToPush ? kPush : kMoreBoxes;

        if (aggregatedScanResult == kMoreBoxes) {
            boxesToPush.push_back(nextBoxPositionsToScanFrom);
            currentBoxPositionsToScanFrom = nextBoxPositionsToScanFrom;
        }
    }

    if (aggregatedScanResult == kBlocked) {
        // at some point, some box can not be pushed so we can not move
        // WE SHOULD NOT HAVE GOTTEN HERE
        return;
    }

    if (aggregatedScanResult == kPush) {
        // we can start pushing
        // moving the boxes in the direction of the move
        for (int32_t i = boxesToPush.size() - 1; i >= 0; --i) {
            const auto boxPositionsToPush = boxesToPush[i];
            for (const auto &boxPos : boxPositionsToPush) {
                const coord_t pushedBoxPos = {boxPos.first + move.first, boxPos.second + move.second};
                // move box position in the direction of the move
                s.warehouse[pushedBoxPos.first][pushedBoxPos.second] = s.warehouse[boxPos.first][boxPos.second];
                // clear their previous location
                s.warehouse[boxPos.first][boxPos.second] = '.';
            }
        }

        // now we move the robot in the direction of the move
        s.robotPosition = nextPosition;
    }
}

void _attemptMove(State &s, const coord_signed_t &move, bool isPart2 = false) {
    coord_t nextPosition = {s.robotPosition.first + move.first, s.robotPosition.second + move.second};
    char warehouseSpot = s.warehouse[nextPosition.first][nextPosition.second];
    if (warehouseSpot == '#') {
        return;
    }

    if (warehouseSpot == '.') {
        s.robotPosition = nextPosition;
        return;
    }

    // next position is a box so we need to figure out if we can push it
    // find next position past the box, in the direction of movement / push

    if (!isPart2) {
        _pushMovePart1(s, move, nextPosition);
    } else {
        _pushMovePart2(s, move, nextPosition);
    }
}

void _moveRobot(State &s, bool isPart2 = false) {
    for (const auto &move : s.robotMoves) {
        _attemptMove(s, move, isPart2);
        // cout << "Move ";
        // _printMove(move);
        // cout << ":" << endl;
        // _printState(s);
    }
}

State _rescaleState(State &s) {
    State rescaledS;
    rescaledS.robotPosition = {s.robotPosition.first, s.robotPosition.second * 2};
    rescaledS.robotMoves = s.robotMoves;
    rescaledS.warehouse = vector<string>(s.h(), "");

    for (size_t i = 0; i < s.h(); ++i) {
        rescaledS.warehouse[i].resize(s.w() * 2);
        for (size_t j = 0; j < s.w(); ++j) {
            const auto location = s.warehouse[i][j];
            if (location == '#') {
                rescaledS.warehouse[i][j * 2] = '#';
                rescaledS.warehouse[i][j * 2 + 1] = '#';
            } else if (location == 'O') {
                rescaledS.warehouse[i][j * 2] = '[';
                rescaledS.warehouse[i][j * 2 + 1] = ']';
            } else if (location == '.') {
                rescaledS.warehouse[i][j * 2] = '.';
                rescaledS.warehouse[i][j * 2 + 1] = '.';
            }
        }
    }

    return rescaledS;
}

void part1(istream& inputFile) {
    State s;
    s.warehouse = vector<string>();
    s.robotMoves = vector<coord_signed_t>();

    bool readWarehouse = true;
    bool foundRobotStart = false;
    size_t rowIdx = 0;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        if (line.empty()) {
            readWarehouse = false;
            continue;
        }

        if (readWarehouse) {
            if (!foundRobotStart) {
                const auto idx = line.find('@');
                if (idx != string::npos) {
                    const size_t colIdx = idx;
                    s.robotPosition = {rowIdx, colIdx};
                    line[s.robotPosition.second] = '.';
                    foundRobotStart = true;
                }
            }

            s.warehouse.push_back(line);
            rowIdx += 1;
        } else {
            for (const char ch : line) {
                switch (ch) {
                    case '^' : s.robotMoves.push_back(kCompassDirections4[kCompass4Orientation::kNorth]); break;
                    case '>' : s.robotMoves.push_back(kCompassDirections4[kCompass4Orientation::kEast]); break;
                    case 'v' : s.robotMoves.push_back(kCompassDirections4[kCompass4Orientation::kSouth]); break;
                    case '<' : s.robotMoves.push_back(kCompassDirections4[kCompass4Orientation::kWest]); break;
                    default: cout << "Unknown move: " << ch << endl; exit(1);
                }
            }
        }
    }

    // _printState(s);

    _moveRobot(s);

    // _printState(s);

    const auto totalGPS = _computeGPS(s);

    cout << totalGPS << endl;
}

void part2(istream& inputFile) {
    State s;
    s.warehouse = vector<string>();
    s.robotMoves = vector<coord_signed_t>();

    bool readWarehouse = true;
    bool foundRobotStart = false;
    size_t rowIdx = 0;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        if (line.empty()) {
            readWarehouse = false;
            continue;
        }

        if (readWarehouse) {
            if (!foundRobotStart) {
                const auto idx = line.find('@');
                if (idx != string::npos) {
                    const size_t colIdx = idx;
                    s.robotPosition = {rowIdx, colIdx};
                    line[s.robotPosition.second] = '.';
                    foundRobotStart = true;
                }
            }

            s.warehouse.push_back(line);
            rowIdx += 1;
        } else {
            for (const char ch : line) {
                switch (ch) {
                    case '^' : s.robotMoves.push_back(kCompassDirections4[kCompass4Orientation::kNorth]); break;
                    case '>' : s.robotMoves.push_back(kCompassDirections4[kCompass4Orientation::kEast]); break;
                    case 'v' : s.robotMoves.push_back(kCompassDirections4[kCompass4Orientation::kSouth]); break;
                    case '<' : s.robotMoves.push_back(kCompassDirections4[kCompass4Orientation::kWest]); break;
                    default: cout << "Unknown move: " << ch << endl; exit(1);
                }
            }
        }
    }

    s = _rescaleState(s);

    // _printState(s);

    _moveRobot(s, true);

    // _printState(s);

    const auto totalGPS = _computeGPS(s, true);

    cout << totalGPS << endl;
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