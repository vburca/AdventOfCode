#include "BingoBoard.h"


BingoBoard::BingoBoard()
{
    _rowSums = std::vector<size_t>(MAX_ROWS, 0);
    _colSums = std::vector<size_t>(MAX_COLS, 0);
}

BingoBoard::~BingoBoard() {}

bool BingoBoard::isWinner()
{
    return (std::find(_rowSums.begin(), _rowSums.end(), MAX_COLS) != _rowSums.end()) ||
            (std::find(_colSums.begin(), _colSums.end(), MAX_ROWS) != _colSums.end());
}

void BingoBoard::add(const size_t row, const size_t col, const size_t number)
{
    _lookup.insert({ number, std::make_tuple(row, col, false) });
}

size_t BingoBoard::mark(const size_t number)
{
    const auto it = _lookup.find(number);
    if (it == _lookup.end())
    {
        // current number is not on this board
        return 0;
    }

    // mark it on the board, update the sums for its row and column
    auto& element = it->second;
    auto& marked = std::get<2>(element);
    marked = true;

    const auto row = std::get<0>(element);
    const auto col = std::get<1>(element);

    _rowSums[row] += 1;
    _colSums[col] += 1;

    if (isWinner())
    {
        return getUnmarkedSum() * number;
    }

    return 0;
}

size_t BingoBoard::getUnmarkedSum()
{
    size_t sum = 0;
    for (auto it = _lookup.begin(); it != _lookup.end(); ++it)
    {
        const auto& element = it->second;
        const auto marked = std::get<2>(element);

        if (!marked)
        {
            sum += it->first;
        }
    }

    return sum;
}