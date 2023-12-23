#include "../../util/StringUtil.h"
#include "../../util/MathUtil.h"
#include "../../util/Types.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint>
#include <vector>
#include <regex>
#include <map>
#include <math.h>
#include <set>
#include <algorithm>
#include <cassert>
#include <iterator>

using namespace std;
using namespace aoc::util::types;

regex BRICK_RX("([0-9]+),([0-9]+),([0-9]+)~([0-9]+),([0-9]+),([0-9]+)");

struct Brick {
    size_t idx;

    coord3d_t startPosition;
    coord3d_t endPosition;

    set<coord3d_t> occupancyBlocks;

    set<size_t> bricksSupported;
    set<size_t> bricksSupportedBy;

    pair<size_t, size_t> getXInterval() const {
        return {startPosition.x, endPosition.x};
    }

    pair<size_t, size_t> getYInterval() const {
        return {startPosition.y, endPosition.y};
    }

    pair<size_t, size_t> getZInterval() const {
        return {startPosition.z, endPosition.z};
    }

    void generateOccupancyBlocks() {
        occupancyBlocks.clear();
        for (size_t x = startPosition.x; x <= endPosition.x; x++) {
            for (size_t y = startPosition.y; y <= endPosition.y; y++) {
                for (size_t z = startPosition.z; z <= endPosition.z; z++) {
                    occupancyBlocks.insert(coord3d_t{x, y, z});
                }
            }
        }
    }
};

struct Input {
    vector<Brick> bricks;
    map<size_t, set<size_t>> zCoordToBricks;

    set<size_t> brickIdsOnFloor;

    size_t minX;
    size_t maxX;

    size_t minY;
    size_t maxY;

    size_t minZ;
    size_t maxZ;

    void computeRanges() {
        minX = SIZE_T_MAX;
        maxX = 0;
        minY = SIZE_T_MAX;
        maxY = 0;
        minZ = SIZE_T_MAX;
        maxZ = 0;
        for (const auto &brick : bricks) {
            minX = min(minX, min(brick.startPosition.x, brick.endPosition.x));
            maxX = max(maxX, max(brick.startPosition.x, brick.endPosition.x));

            minY = min(minY, min(brick.startPosition.y, brick.endPosition.y));
            maxY = max(maxY, max(brick.startPosition.y, brick.endPosition.y));

            minZ = min(minZ, min(brick.startPosition.z, brick.endPosition.z));
            maxZ = max(maxZ, max(brick.startPosition.z, brick.endPosition.z));
        }
    }
};

void _printBrick(const Brick &brick) {
    cout << "Brick " << char(brick.idx + 'A') << ": { ";
    cout << "(" << brick.startPosition.x << ", " << brick.startPosition.y << ", " << brick.startPosition.z << ") ";
    cout << "(" << brick.endPosition.x << ", " << brick.endPosition.y << ", " << brick.endPosition.z << ") }";
}

void _visualizeBrickStack(const Input &input) {
    Input modifiedInput = input;
    for (auto &brick : modifiedInput.bricks) {
        brick.generateOccupancyBlocks();
    }

    cout << "Bricks (x cross section): (" << modifiedInput.bricks.size() << ")" << endl;
    for (int64_t z = modifiedInput.maxZ; z >= 0; z--) {
        for (size_t x = modifiedInput.minX; x <= modifiedInput.maxX; x++) {
            if (z == 0) {
                cout << "-";
                continue;
            }

            size_t bricksFound = 0;
            size_t brickIdxFound = 0;
            for (const auto &brick : modifiedInput.bricks) {
                if (bricksFound > 1) {
                    break;
                }

                for (const auto &occupancy : brick.occupancyBlocks) {
                    if (occupancy.x == x && occupancy.z == z) {
                        brickIdxFound = brick.idx;
                        bricksFound += 1;
                        break;
                    }
                }
            }

            if (bricksFound == 0) {
                cout << ".";
            } else if (bricksFound == 1) {
                cout << char('A' + brickIdxFound);
            } else {
                cout << "?";
            }
        }
        cout << endl;
    }
    cout << endl;

    cout << "Bricks (y cross section): (" << modifiedInput.bricks.size() << ")" << endl;
    for (int64_t z = modifiedInput.maxZ; z >= 0; z--) {
        for (size_t y = modifiedInput.minY; y <= modifiedInput.maxY; y++) {
            if (z == 0) {
                cout << "-";
                continue;
            }

            size_t bricksFound = 0;
            size_t brickIdxFound = 0;
            for (const auto &brick : modifiedInput.bricks) {
                if (bricksFound > 1) {
                    break;
                }

                for (const auto &occupancy : brick.occupancyBlocks) {
                    if (occupancy.y == y && occupancy.z == z) {
                        brickIdxFound = brick.idx;
                        bricksFound += 1;
                        break;
                    }
                }
            }

            if (bricksFound == 0) {
                cout << ".";
            } else if (bricksFound == 1) {
                cout << char('A' + brickIdxFound);
            } else {
                cout << "?";
            }
        }
        cout << endl;
    }
    cout << endl;
}

void _printInput(const Input &input) {
    cout << "Bricks: (" << input.bricks.size() << ")" << endl;
    for (const auto &brick : input.bricks) {
        _printBrick(brick);
        cout << endl;
    }
    cout << endl;

    cout << "Bricks mapping per Z axis: (" << input.zCoordToBricks.size() << ")" << endl;
    for (const auto &p : input.zCoordToBricks) {
        cout << p.first << " -> { " << endl;
        for (const auto brickIdx : p.second) {
            _printBrick(input.bricks[brickIdx]);
            cout << endl;
        }
        cout << "}" << endl;
    }
}

bool _areIntervalsIntersecting(const pair<size_t, size_t> &intervalA, const pair<size_t, size_t> intervalB) {
    return !(intervalA.second < intervalB.first || intervalB.second < intervalA.first);
}

void _dropBricks(Input &input) {
    for (auto &brick : input.bricks) {
        brick.generateOccupancyBlocks();
    }

    vector<Brick> &currentBricks = input.bricks;

    while (true) {
        bool didDropBrick = false;

        sort(currentBricks.begin(), currentBricks.end(), [](const Brick &left, const Brick &right) {
            return left.startPosition.z < right.startPosition.z;
        });

        // construct set of all occupancy blocks
        set<coord3d_t> allOccupancyBlocks;
        for (const auto &brick : currentBricks) {
            allOccupancyBlocks.insert(brick.occupancyBlocks.begin(), brick.occupancyBlocks.end());
        }

        // try to drop each brick
        for (size_t brickIdx = 0; brickIdx < currentBricks.size(); brickIdx++) {
            auto &brick = currentBricks[brickIdx];

            // create set of the occupancy blocks owned by the other bricks
            set<coord3d_t> otherOccupancyBlocks = allOccupancyBlocks;
            for (const auto &block : brick.occupancyBlocks) {
                otherOccupancyBlocks.erase(block);
            }

            // now search through all the other occupancy blocks and see if there's anything blocking our
            // current brick from being dropped
            bool canBeDropped = true;
            for (const auto &block : brick.occupancyBlocks) {
                if (block.z == 1) {
                    canBeDropped = false;
                    break;
                }

                const auto droppedBlock = coord3d_t{block.x, block.y, block.z - 1};
                if (otherOccupancyBlocks.contains(droppedBlock)) {
                    canBeDropped = false;
                    break;
                }
            }

            if (canBeDropped) {
                // drop it like it's hot
                Brick droppedBrick = {
                    .idx = brick.idx,
                    .startPosition = {brick.startPosition.x, brick.startPosition.y, brick.startPosition.z - 1},
                    .endPosition = {brick.endPosition.x, brick.endPosition.y, brick.endPosition.z - 1},
                };

                allOccupancyBlocks = otherOccupancyBlocks;
                for (const auto &block : brick.occupancyBlocks) {
                    const auto droppedBlock = coord3d_t{block.x, block.y, block.z - 1};
                    droppedBrick.occupancyBlocks.insert(droppedBlock);

                    // also update all the occupancy blocks
                    allOccupancyBlocks.insert(droppedBlock);
                }

                currentBricks[brickIdx] = droppedBrick;

                didDropBrick = true;
            }

        }

        if (!didDropBrick) {
            break;
        }
    }
}

void _dropBricksBounds(Input &input) {
    vector<Brick> &currentBricks = input.bricks;

    while (true) {
        bool didDropBrick = false;

        // try to drop each brick
        for (size_t brickIdx = 0; brickIdx < currentBricks.size(); brickIdx++) {
            auto &brick = currentBricks[brickIdx];

            // check if it's already on the ground
            if (brick.startPosition.z == 1) {
                // can't drop it anymore
                continue;
            }

            set<size_t> lowerBrickIds;
            for (const auto &p : input.zCoordToBricks) {
                if (p.first < brick.startPosition.z) {
                    lowerBrickIds.insert(p.second.begin(), p.second.end());
                } else {
                    break;
                }
            }

            // now search through the lower bricks and see if any of them is blocking our current brick
            bool canBeDropped = true;
            const Brick droppedBrick = {
                .idx = brick.idx,
                .startPosition = {brick.startPosition.x, brick.startPosition.y, brick.startPosition.z - 1},
                .endPosition = {brick.endPosition.x, brick.endPosition.y, brick.endPosition.z - 1},
            };
            for (const auto lowerBrickId : lowerBrickIds) {
                const auto &lowerBrick = input.bricks[lowerBrickId];
                if (_areIntervalsIntersecting(lowerBrick.getXInterval(), droppedBrick.getXInterval()) &&
                    _areIntervalsIntersecting(lowerBrick.getYInterval(), droppedBrick.getYInterval()) &&
                    _areIntervalsIntersecting(lowerBrick.getZInterval(), droppedBrick.getZInterval())) {
                        canBeDropped = false;
                        break;
                }
            }

            if (canBeDropped) {
                // drop it like it's hot
                const auto previousStartZ = brick.startPosition.z;
                currentBricks[brickIdx].startPosition = {brick.startPosition.x, brick.startPosition.y, brick.startPosition.z - 1};
                currentBricks[brickIdx].endPosition = {brick.endPosition.x, brick.endPosition.y, brick.endPosition.z - 1};

                // remove the pointer at the previous z value
                input.zCoordToBricks[previousStartZ].erase(brickIdx);

                // add the pointer to the new z value
                input.zCoordToBricks[currentBricks[brickIdx].startPosition.z].insert(brickIdx);

                didDropBrick = true;
            }

        }

        if (!didDropBrick) {
            break;
        }
    }
}

void _computeBrickSupports(Input &input) {
    for (auto &brick : input.bricks) {
        if (!input.zCoordToBricks.contains(brick.startPosition.z - 1)) {
            continue;
        }

        set<size_t> lowerBrickIds;
        for (const auto &p : input.zCoordToBricks) {
            if (p.first < brick.startPosition.z) {
                lowerBrickIds.insert(p.second.begin(), p.second.end());
            } else {
                break;
            }
        }

        Brick underBrick = {
            .idx = brick.idx,
            .startPosition = {brick.startPosition.x, brick.startPosition.y, brick.startPosition.z - 1},
            .endPosition = {brick.endPosition.x, brick.endPosition.y, brick.endPosition.z - 1},
        };

        for (const auto lowerBrickId : lowerBrickIds) {
            auto &lowerBrick = input.bricks[lowerBrickId];
            if (_areIntervalsIntersecting(lowerBrick.getXInterval(), underBrick.getXInterval()) &&
                _areIntervalsIntersecting(lowerBrick.getYInterval(), underBrick.getYInterval()) &&
                _areIntervalsIntersecting(lowerBrick.getZInterval(), underBrick.getZInterval())) {
                    brick.bricksSupportedBy.insert(lowerBrickId);
                    lowerBrick.bricksSupported.insert(brick.idx);
            }
        }
    }
}

void _computeBricksOnFloor(Input &input) {
    for (const auto &brick : input.bricks) {
        if (brick.bricksSupportedBy.size() == 0) {
            input.brickIdsOnFloor.insert(brick.idx);
        }
    }
}

const Input _parseInput(istream& inputFile) {
    Input input;

    size_t brickIdx = 0;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        smatch brickMatch;
        if (!regex_match(line, brickMatch, BRICK_RX)) {
            cout << "BRICK_RX unable to match for line: { " << line << " }" << endl;
            exit(1);
        }

        Brick brick = {
            .idx = brickIdx,
            .startPosition = {stoull(brickMatch[1].str()), stoull(brickMatch[2].str()), stoull(brickMatch[3].str())},
            .endPosition = {stoull(brickMatch[4].str()), stoull(brickMatch[5].str()), stoull(brickMatch[6].str())}
        };

        input.bricks.push_back(brick);

        brickIdx += 1;
    }

    // update z mapping
    for (const auto &brick : input.bricks) {
        input.zCoordToBricks[brick.startPosition.z].insert(brick.idx);
    }

    _dropBricksBounds(input);
    input.computeRanges();
    _computeBrickSupports(input);
    _computeBricksOnFloor(input);

    return input;
}

void part1(const Input &input) {
    set<size_t> brickIdsNotSafeToRemove;
    for (const auto &brick : input.bricks) {
        if (brick.bricksSupportedBy.size() == 1) {
            auto bricksSupportedIt = brick.bricksSupportedBy.begin();
            const auto brickSupportedId = *bricksSupportedIt;
            brickIdsNotSafeToRemove.insert(brickSupportedId);
        }
    }

    const auto numBricksSafeToRemove = input.bricks.size() - brickIdsNotSafeToRemove.size();

    cout << numBricksSafeToRemove << endl;
}

size_t _getNumberOfFallenBricksIfRemove(set<size_t> &removedBrickIds, const Input &input) {
    set<size_t> fallingBrickIds;
    for (const auto &brick : input.bricks) {
        if (input.brickIdsOnFloor.contains(brick.idx) || removedBrickIds.contains(brick.idx)) {
            continue;
        }

        set<size_t> remainingSupportingBrickIds = brick.bricksSupportedBy;
        for (const auto removedBrickId : removedBrickIds) {
            remainingSupportingBrickIds.erase(removedBrickId);
        }

        if (remainingSupportingBrickIds.empty()) {
            fallingBrickIds.insert(brick.idx);
        }
    }

    if (fallingBrickIds.empty()) {
        return 0;
    }

    for (const auto fallingBrickId : fallingBrickIds) {
        removedBrickIds.insert(fallingBrickId);
    }

    return fallingBrickIds.size() + _getNumberOfFallenBricksIfRemove(removedBrickIds, input);
}

void part2(const Input &input) {
    Input mutableInput = input;
    size_t sumFallenBricks = 0;
    for (auto &brick : mutableInput.bricks) {
        set<size_t> removedBrickIds = {brick.idx};
        const auto fallenBricks = _getNumberOfFallenBricksIfRemove(removedBrickIds, mutableInput);
        // cout << "Disintegrating brick " << brick.idx << " would cause " << fallenBricks << " other bricks to fall" << endl;
        sumFallenBricks += fallenBricks;
    }

    cout << sumFallenBricks << endl;
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
    // _visualizeBrickStack(input);

    std::cout << "Part 1: " << endl;
    part1(input);

    std::cout << "Part 2: " << endl;
    part2(input);

    inputFile.close();

    return 0;
}