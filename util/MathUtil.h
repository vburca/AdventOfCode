#include <algorithm>
#include <cctype>
#include <locale>
#include <vector>
#include <sstream>
#include <numeric>
#include <cstdint>
#include <functional>

namespace aoc {
namespace util {
namespace math {

static inline size_t getNumberOfDigits(const size_t n) {
    size_t nn = n;
    size_t numDigits = 0;
    while (nn) {
        nn = nn / 10;
        numDigits += 1;
    }

    return numDigits;
}

template <typename M>
constexpr auto lcm(std::vector<M> &v) {
    return std::accumulate(v.begin(), v.end(), 1ull, [](const auto &a, const auto &b) {
        return std::lcm(a, b);
    });
}

inline int64_t positiveModulo(int64_t i, int64_t n) {
    return (i % n + n) % n;
}

template <typename T>
short int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

}  // namespace math
}  // namespace util
}  // namespace aoc