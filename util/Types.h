#include <utility>

namespace aoc {
namespace util {
namespace types {

typedef std::pair<size_t, size_t> coord_t;
typedef std::pair<int64_t, int64_t> coord_signed_t;
typedef std::pair<float, float> coord_float_t;
typedef std::pair<double, double> coord_double_t;

struct coord3d_t {
    size_t x;
    size_t y;
    size_t z;

    bool operator==(const coord3d_t &other) const {
        return std::tie(x, y, z) == std::tie(other.x, other.y, other.z);
    }

    bool operator<(const coord3d_t &other) const {
        return std::tie(x, y, z) < std::tie(other.x, other.y, other.z);
    }
};

struct coord3d_signed_t {
    int64_t x;
    int64_t y;
    int64_t z;

    bool operator==(const coord3d_signed_t &other) const {
        return std::tie(x, y, z) == std::tie(other.x, other.y, other.z);
    }

    bool operator<(const coord3d_signed_t &other) const {
        return std::tie(x, y, z) < std::tie(other.x, other.y, other.z);
    }
};

}  // namespace types
}  // namespace util
}  // namespace aoc