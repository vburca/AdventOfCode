#include <algorithm>
#include <cctype>
#include <locale>
#include <vector>
#include <sstream>

namespace aoc {
namespace util {
namespace string {

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    rtrim(s);
    ltrim(s);
}

// trim from start (copying)
static inline std::string ltrimCopy(std::string s) {
    ltrim(s);
    return s;
}

// trim from end (copying)
static inline std::string rtrimCopy(std::string s) {
    rtrim(s);
    return s;
}

// trim from both ends (copying)
static inline std::string trimCopy(std::string s) {
    trim(s);
    return s;
}

static inline std::vector<std::string> split(const std::string &s, const char delim, const bool trim = false) {
    std::vector<std::string> separatedWords;
    std::stringstream ss(s);
    std::string word;
    while (!ss.eof()) {
        getline(ss, word, delim);
        word = trim ? trimCopy(word) : word;
        separatedWords.push_back(trimCopy(word));
    }

    return separatedWords;
}

template <typename T>
static inline std::vector<T> parseDelimSeparatedNumbers(const std::string &s, const char delim) {
    std::vector<T> numbers;

    std::istringstream ss(s);
    std::string token;
    while (std::getline(ss, token, delim)) {
        const auto trimmedNumber = aoc::util::string::trimCopy(token);
        if (trimmedNumber.size() == 0) {
            continue;
        }
        numbers.push_back(static_cast<T>(stoll(trimmedNumber)));
    }

    return numbers;
}

static inline std::vector<size_t> findAllPositions(const std::string &s, const char ch) {
    std::vector<size_t> positions;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == ch) {
            positions.emplace_back(i);
        }
    }

    return positions;
}

}  // namespace string
}  // namespace util
}  // namespace aoc