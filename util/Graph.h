#pragma once

#include "Types.h"

#include <vector>
#include <utility>

namespace aoc {
namespace util {
namespace graph {

const std::vector<types::coord_signed_t> kCompassDirections8 = {
    {-1, 0},  // up
    {-1, 1},  // up-right
    {0, 1},   // right
    {1, 1},   // down-right
    {1, 0},   // down
    {1, -1},  // down-left
    {0, -1},  // left
    {-1, -1}, // up-left
};

enum kCompass4Orientation {
    kNorth = 0,
    kEast,
    kSouth,
    kWest,
    kCount,
};

const std::vector<types::coord_signed_t> kCompassDirections4 = {
    {-1, 0},  // up
    {0, 1},   // right
    {1, 0},   // down
    {0, -1},  // left
};

// Adjacency matrix implementation of Stoer–Wagner min cut algorithm.
// Running time:
//     O(|V|^3)
// Reference: https://en.wikipedia.org/wiki/Stoer%E2%80%93Wagner_algorithm
std::pair<int, std::vector<int>> globalMinCut_StoerWagner(std::vector<std::vector<int>> mat) {
    std::pair<int, std::vector<int>> best = {INT_MAX, {}};
    int n = mat.size();
    std::vector<std::vector<int>> co(n);

    for (int i = 0; i < n; i++)
        co[i] = {i};

    for (int ph = 1; ph < n; ph++) {
        std::vector<int> w = mat[0];
        size_t s = 0, t = 0;
        for (int it = 0; it < n - ph; it++) { // O(V^2) -> O(E log V) with prio. queue
            w[t] = INT_MIN;
            s = t, t = max_element(w.begin(), w.end()) - w.begin();
            for (int i = 0; i < n; i++) w[i] += mat[t][i];
        }
        best = min(best, {w[t] - mat[t][t], co[t]});
        co[s].insert(co[s].end(), co[t].begin(), co[t].end());
        for (int i = 0; i < n; i++) mat[s][i] += mat[t][i];
        for (int i = 0; i < n; i++) mat[i][s] = mat[s][i];
        mat[0][t] = INT_MIN;
    }

    return best;
}

}  // namespace graph
}  // namespace util
}  // namespace aoc
