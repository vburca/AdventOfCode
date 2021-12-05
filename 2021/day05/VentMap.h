#pragma once

#include <unordered_map>
#include <tuple>

typedef std::tuple<size_t, size_t> position_t;

struct position_t_equal : public std::binary_function<position_t, position_t, size_t>
{
    bool operator()(const position_t& p1, const position_t& p2) const
    {
        return (std::get<0>(p1) == std::get<0>(p2) &&
                std::get<1>(p1) == std::get<1>(p1));
    }
};

struct position_t_hash : public std::unary_function<position_t, size_t>
{
    size_t operator()(const position_t& p) const
    {
        return std::get<0>(p) ^ std::get<1>(p);
    }
};

typedef std::unordered_map<position_t, size_t, position_t_hash, position_t_equal> positions_map_t;

class VentMap
{
public:
    VentMap(const size_t overlapThreshold);
    ~VentMap();

    void fillPosition(const size_t x1, const size_t y1, const size_t x2, const size_t y2, bool diagonals = false);
    const size_t getOverlapCount() const;
    const size_t scanPositions(const size_t overlapThreshold) const;
    void printMap() const;
private:
    size_t _overlapThreshold;
    size_t _overlapCount;
    positions_map_t _map;
};