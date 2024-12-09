#include "../../util/BasicIncludes.h"
#include "../../util/StringUtil.h"
#include "../../util/Types.h"

#include <vector>
#include <queue>
#include <optional>

using namespace std;
using namespace aoc::util::types;

struct MemoryBlockInterval {
    size_t startIdx;
    size_t endIdx;
    optional<size_t> id = nullopt;

    const size_t size() const {
        return endIdx - startIdx + 1;
    }
};

struct MemoryLayout {
    vector<MemoryBlockInterval> memoryIntervals;
    vector<MemoryBlockInterval> compactMemoryIntervals;
};

void _printMemoryBlocks(const vector<MemoryBlockInterval> &memoryIntervals) {
    for (const auto &memoryInterval : memoryIntervals) {
        for (size_t i = memoryInterval.startIdx; i <= memoryInterval.endIdx; ++i) {
            if (memoryInterval.id) {
                cout << *(memoryInterval.id);
            } else {
                cout << '.';
            }
        }
    }

    cout << endl;
}

MemoryLayout _createMemory(const string &diskMap) {
    MemoryLayout memoryLayout;

    bool isFileBlock = true;
    size_t currentFileId = 0;
    size_t currentMemoryBlockIdx = 0;
    for (const char ch : diskMap) {
        const size_t blockSize = ch - '0';

        if (isFileBlock) {
            memoryLayout.memoryIntervals.emplace_back(MemoryBlockInterval {
                .startIdx = currentMemoryBlockIdx,
                .endIdx = currentMemoryBlockIdx + blockSize - 1,
                .id = currentFileId,
            });

            currentFileId += 1;
            isFileBlock = false;
        } else {
            memoryLayout.memoryIntervals.emplace_back(MemoryBlockInterval {
                .startIdx = currentMemoryBlockIdx,
                .endIdx = currentMemoryBlockIdx + blockSize - 1,
            });

            isFileBlock = true;
        }

        currentMemoryBlockIdx = memoryLayout.memoryIntervals.back().endIdx + 1;
    }

    return memoryLayout;
}

const optional<size_t> _getNextFreeMemoryBlockInterval(const MemoryLayout &memoryLayout, size_t currentMemoryBlockIntervalIdx, optional<size_t> requiredSizeOpt = nullopt) {
    for (size_t i = currentMemoryBlockIntervalIdx; i < memoryLayout.compactMemoryIntervals.size(); ++i) {
        const auto &memoryBlock = memoryLayout.compactMemoryIntervals[i];
        if (memoryBlock.id) {
            continue;
        }

        if (!requiredSizeOpt) {
            return i;
        }

        if (memoryBlock.size() >= *requiredSizeOpt) {
            return i;
        }
    }

    return memoryLayout.compactMemoryIntervals.size();
}

const optional<size_t> _getNextFileMemoryBlockIntervalToMove(const MemoryLayout &memoryLayout, size_t currentMemoryBlockIntervalIdx) {
    for (size_t i = currentMemoryBlockIntervalIdx; i >= 0; --i) {
        const auto &memoryBlock = memoryLayout.compactMemoryIntervals[i];
        if (memoryBlock.id) {
            return i;
        }
    }

    return nullopt;
}

const uint64_t _getSumOfN(const uint64_t n) {
    return (n * (n + 1)) / 2;
}

const uint64_t _computeMemoryChecksum(const vector<MemoryBlockInterval> &memoryIntervals) {
    uint64_t checksum = 0;
    for (const auto &memoryInterval : memoryIntervals) {
        if (!memoryInterval.id) {
            continue;
        }

        checksum += *memoryInterval.id * (_getSumOfN(memoryInterval.endIdx) - _getSumOfN(memoryInterval.startIdx - 1));
    }

    return checksum;
}

void _compactMemory(MemoryLayout &memoryLayout) {
    memoryLayout.compactMemoryIntervals = memoryLayout.memoryIntervals;
    auto &compactMemoryIntervals = memoryLayout.compactMemoryIntervals;

    optional<size_t> currentFileMemoryIntervalIdxOpt = _getNextFileMemoryBlockIntervalToMove(memoryLayout, memoryLayout.compactMemoryIntervals.size() - 1);
    size_t currentFileMemoryIntervalIdx = *currentFileMemoryIntervalIdxOpt;
    optional<size_t> currentFreeMemoryIntervalIdxOpt = _getNextFreeMemoryBlockInterval(memoryLayout, 0);
    size_t currentFreeMemoryIntervalIdx = *currentFreeMemoryIntervalIdxOpt;

    // cout << endl << "--- START DEBUG" << endl;

    // size_t DEBUG_IDX = 0;
    while (currentFreeMemoryIntervalIdx < currentFileMemoryIntervalIdx) {
        auto &freeMemoryInterval = compactMemoryIntervals[currentFreeMemoryIntervalIdx];
        auto &fileMemoryInterval = compactMemoryIntervals[currentFileMemoryIntervalIdx];

        if (fileMemoryInterval.size() >= freeMemoryInterval.size()) {
            // fully fill this free memory interval with the current file blocks
            freeMemoryInterval.id = fileMemoryInterval.id;

            // move out the file memory blocks that we were able to fit in the free memory interval
            fileMemoryInterval.endIdx -= freeMemoryInterval.size();
            // create free interval for the number of blocks we freed up
            MemoryBlockInterval newFreeInterval = MemoryBlockInterval {
                .startIdx = fileMemoryInterval.endIdx + 1,
                .endIdx = (fileMemoryInterval.endIdx + 1) + freeMemoryInterval.size() - 1,
            };
            // insert the new free interval after the current file interval
            if (currentFileMemoryIntervalIdx == compactMemoryIntervals.size() - 1) {
                compactMemoryIntervals.insert(compactMemoryIntervals.end(), newFreeInterval);
            } else {
                compactMemoryIntervals.insert(compactMemoryIntervals.begin() + currentFileMemoryIntervalIdx + 1, newFreeInterval);
            }

            // clean up the file memory if it's empty
            if (fileMemoryInterval.size() == 0) {
                compactMemoryIntervals.erase(compactMemoryIntervals.begin() + currentFileMemoryIntervalIdx);
                // find the next file interval we need to move
                currentFileMemoryIntervalIdxOpt = _getNextFileMemoryBlockIntervalToMove(memoryLayout, currentFileMemoryIntervalIdx - 1);
                currentFileMemoryIntervalIdx = *currentFileMemoryIntervalIdxOpt;
            }

            // find the next free interval and continue
            currentFreeMemoryIntervalIdxOpt = _getNextFreeMemoryBlockInterval(memoryLayout, currentFreeMemoryIntervalIdx);
            currentFreeMemoryIntervalIdx = *currentFreeMemoryIntervalIdxOpt;
        } else {
            // partially fill the free memory interval by splitting it into 2 (one part fully filled with the file blocks,
            // the other part with the remaining free blocks)
            const auto movedFileInterval = MemoryBlockInterval {
                .startIdx = freeMemoryInterval.startIdx,
                .endIdx = freeMemoryInterval.startIdx + fileMemoryInterval.size() - 1,
                .id = fileMemoryInterval.id,
            };

            // free up the file blocks that we moved out
            fileMemoryInterval.id = nullopt;

            // shrink the free interval by the number of elements we moved into it
            freeMemoryInterval.startIdx += fileMemoryInterval.size();
            // insert the new file interval in the memory layout
            compactMemoryIntervals.insert(compactMemoryIntervals.begin() + currentFreeMemoryIntervalIdx, movedFileInterval);
            // move the free memory interval index by 1, since we added the file interval before it
            currentFreeMemoryIntervalIdx += 1;

            // find the next file interval to move and continue
            currentFileMemoryIntervalIdxOpt = _getNextFileMemoryBlockIntervalToMove(memoryLayout, currentFileMemoryIntervalIdx - 1);
            currentFileMemoryIntervalIdx = *currentFileMemoryIntervalIdxOpt;
        }

        // _printMemoryBlocks(compactMemoryIntervals);

        // DEBUG_IDX += 1;
        // if (DEBUG_IDX == 100) {
        //     break;
        // }
    }

    // cout << "--- END DEBUG" << endl << endl;
}

void _compactMemoryWholeFiles(MemoryLayout &memoryLayout) {
    memoryLayout.compactMemoryIntervals = memoryLayout.memoryIntervals;
    auto &compactMemoryIntervals = memoryLayout.compactMemoryIntervals;

    // cout << endl << "--- START DEBUG" << endl;
    // size_t DEBUG_IDX = 0;

    optional<size_t> currentFileMemoryIntervalIdxOpt = _getNextFileMemoryBlockIntervalToMove(memoryLayout, memoryLayout.compactMemoryIntervals.size() - 1);
    while (currentFileMemoryIntervalIdxOpt) {
        size_t currentFileMemoryIntervalIdx = *currentFileMemoryIntervalIdxOpt;
        auto &fileMemoryInterval = compactMemoryIntervals[currentFileMemoryIntervalIdx];

        optional<size_t> currentFreeMemoryIntervalIdxOpt = _getNextFreeMemoryBlockInterval(memoryLayout, 0, fileMemoryInterval.size());
        if (!currentFreeMemoryIntervalIdxOpt || *currentFreeMemoryIntervalIdxOpt > currentFileMemoryIntervalIdx) {
            // current file blocks can not be moved because there is not enough contiguous free memory blocks
            // move on to the next file
            if (currentFileMemoryIntervalIdx == 0) {
                break;
            }
            currentFileMemoryIntervalIdxOpt = _getNextFileMemoryBlockIntervalToMove(memoryLayout, currentFileMemoryIntervalIdx - 1);
            continue;
        }

        // we found enough contiguous free memory blocks to move the file
        size_t currentFreeMemoryIntervalIdx = *currentFreeMemoryIntervalIdxOpt;
        auto &freeMemoryInterval = compactMemoryIntervals[currentFreeMemoryIntervalIdx];

        // partially fill the free memory interval by splitting it into 2 (one part fully filled with the file blocks,
        // the other part with the remaining free blocks)
        const auto movedFileInterval = MemoryBlockInterval {
            .startIdx = freeMemoryInterval.startIdx,
            .endIdx = freeMemoryInterval.startIdx + fileMemoryInterval.size() - 1,
            .id = fileMemoryInterval.id,
        };

        // free up the file blocks that we moved out
        fileMemoryInterval.id = nullopt;

        // shrink the free interval by the number of elements we moved into it
        freeMemoryInterval.startIdx += fileMemoryInterval.size();
        // insert the new file interval in the memory layout
        compactMemoryIntervals.insert(compactMemoryIntervals.begin() + currentFreeMemoryIntervalIdx, movedFileInterval);
        // move the free memory interval index by 1, since we added the file interval before it
        currentFreeMemoryIntervalIdx += 1;

        // find the next file interval to move and continue
        if (currentFileMemoryIntervalIdx == 0) {
            break;
        }
        currentFileMemoryIntervalIdxOpt = _getNextFileMemoryBlockIntervalToMove(memoryLayout, currentFileMemoryIntervalIdx - 1);

        // _printMemoryBlocks(compactMemoryIntervals);

        // DEBUG_IDX += 1;
        // if (DEBUG_IDX == 100) {
        //     break;
        // }
    }

    // cout << "--- END DEBUG" << endl << endl;
}

void part1(istream& inputFile) {
    string diskMap;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        diskMap = line;
    }

    MemoryLayout memoryLayout = _createMemory(diskMap);
    // _printMemoryBlocks(memoryLayout.memoryIntervals);
    _compactMemory(memoryLayout);
    // _printMemoryBlocks(memoryLayout.compactMemoryIntervals);

    const auto checksum = _computeMemoryChecksum(memoryLayout.compactMemoryIntervals);
    cout << checksum << endl;
}

void part2(istream& inputFile) {
    string diskMap;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        diskMap = line;
    }

    MemoryLayout memoryLayout = _createMemory(diskMap);
    // _printMemoryBlocks(memoryLayout.memoryIntervals);
    _compactMemoryWholeFiles(memoryLayout);
    // _printMemoryBlocks(memoryLayout.compactMemoryIntervals);

    const auto checksum = _computeMemoryChecksum(memoryLayout.compactMemoryIntervals);
    cout << checksum << endl;
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