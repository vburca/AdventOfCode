#pragma once

#include <algorithm>
#include <vector>
#include <unordered_map>

#define MAX_ROWS 5
#define MAX_COLS 5

class BingoBoard
{
public:
    BingoBoard();
    ~BingoBoard();

    bool isWinner();
    void add(const size_t row, const size_t col, const size_t number);
    size_t mark(const size_t number);
private:
    size_t getUnmarkedSum();

    std::vector<size_t> _rowSums;
    std::vector<size_t> _colSums;
    std::unordered_map<size_t, std::tuple<size_t, size_t, bool>> _lookup;   // bingo number -> <row, col, marked>
};