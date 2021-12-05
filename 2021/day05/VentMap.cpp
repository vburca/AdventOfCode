#include <iostream>

#include "VentMap.h"

using namespace std;

VentMap::VentMap(const size_t overlapThreshold)
{
    _overlapThreshold = overlapThreshold;
    _overlapCount = 0;
}

VentMap::~VentMap() {}

const size_t VentMap::getOverlapCount() const
{
    return _overlapCount;
}

void VentMap::fillPosition(const size_t x1, const size_t y1, const size_t x2, const size_t y2, bool diagonals)
{
    if (!diagonals)
    {
        if (x1 != x2 && y1 != y2)
        {
            return;
        }
    }

    size_t x = x1;
    size_t y = y1;
    const size_t xSign = x2 >= x1 ? 1 : -1;
    const size_t ySign = y2 >= y1 ? 1 : -1;

    bool done = false;
    while (!done)
    {
        position_t pos = make_tuple(y, x); // x is along columns, y is along rows

        const auto it = _map.find(pos);
        if (it == _map.end())
        {
            _map.insert({pos, 1});
        }
        else
        {
            auto& count = it->second;
            count += 1;

            if (count == _overlapThreshold)
            {
                _overlapCount += 1;
            }
        }

        if (x == x2 && y == y2)
        {
            done = true;
        }
        else
        {
            if (x != x2)
            {
                x += xSign;
            }

            if (y != y2)
            {
                y += ySign;
            }
        }
    }
}

const size_t VentMap::scanPositions(const size_t overlapThreshold) const
{
    size_t badOverlaps = 0;
    for (auto it = _map.begin(); it != _map.end(); ++it)
    {
        badOverlaps += it->second >= overlapThreshold ? 1 : 0;
    }

    return badOverlaps;
}

void VentMap::printMap() const
{
    if (_map.empty())
    {
        return;
    }

    size_t maxX = get<0>(_map.begin()->first);
    size_t maxY = get<1>(_map.begin()->first);
    for (auto it = _map.begin(); it != _map.end(); ++it)
    {
        const size_t x = get<0>(it->first);
        const size_t y = get<1>(it->first);

        maxX = x > maxX ? x : maxX;
        maxY = y > maxY ? y : maxY;
    }

    cout << endl;
    for (size_t row = 0; row <= maxX; ++row)
    {
        for (size_t col = 0; col <= maxY; ++col)
        {
            const position_t pos = make_tuple(row, col);
            const auto it = _map.find(pos);
            if (it == _map.end())
            {
                cout << ".";
            }
            else
            {
                cout << it->second;
            }
        }
        cout << endl;
    }
    cout << endl;
}