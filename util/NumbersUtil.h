#include <algorithm>
#include <cctype>
#include <locale>
#include <vector>
#include <sstream>

namespace aoc {
namespace util {
namespace numbers {

static inline size_t getNumberOfDigits(const size_t n) {
    size_t nn = n;
    size_t numDigits = 0;
    while (nn) {
        nn = nn / 10;
        numDigits += 1;
    }

    return numDigits;
}


}  // namespace string
}  // namespace util
}  // namespace aoc