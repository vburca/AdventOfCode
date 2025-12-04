#include "../../util/BasicIncludes.h"
#include "../../util/MathUtil.h"
#include "../../util/StringUtil.h"
#include "../../util/Types.h"

#include <set>

using namespace std;

vector<aoc::util::types::coord_t> getAdjacentRolls(const vector<string>& diagram,
                                                    const size_t n,
                                                    const size_t m,
                                                    aoc::util::types::coord_t position) {
    vector<aoc::util::types::coord_t> adjacentRolls;

    for (short int di = -1; di <= 1; di++) {
        const short int i = position.first + di;
        if (i < 0 || i >= n) {
            continue;
        }
        for (short int dj = -1; dj <= 1; dj++) {
            const short j = position.second + dj;
            if (j < 0 || j >= m) {
                continue;
            }

            if (di == 0 && dj == 0) {
                continue;
            }

            if (diagram[i][j] == '@') {
                adjacentRolls.push_back({i, j});
            }
        }
    }

    return adjacentRolls;
}

void part1(istream& inputFile) {
    vector<string> diagram;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        diagram.push_back(line);
    }

    const size_t n = diagram.size();
    const size_t m = diagram.front().size();

    size_t numAccessibleRolls = 0;
    set<aoc::util::types::coord_t> accessibleRolls;
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < m; j++) {
            if (diagram[i][j] != '@') {
                continue;
            }

            const auto adjacentRolls = getAdjacentRolls(diagram, n, m, {i, j});

            if (adjacentRolls.size() < 4) {
                accessibleRolls.insert({i, j});
            }
        }
    }

    cout << accessibleRolls.size() << endl;
}

void part2(istream& inputFile) {
    vector<string> diagram;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        diagram.push_back(line);
    }

    const size_t n = diagram.size();
    const size_t m = diagram.front().size();

    size_t numAccessibleRolls = 0;
    set<aoc::util::types::coord_t> totalAccessibleRolls;

    bool hadAccessibleRolls = false;
    vector<string> updatedDiagram = diagram;
    do {
        hadAccessibleRolls = false;
        set<aoc::util::types::coord_t> accessibleRolls;
        for (size_t i = 0; i < n; i++) {
            for (size_t j = 0; j < m; j++) {
                if (updatedDiagram[i][j] != '@') {
                    continue;
                }

                const auto adjacentRolls = getAdjacentRolls(updatedDiagram, n, m, {i, j});

                if (adjacentRolls.size() < 4) {
                    accessibleRolls.insert({i, j});
                }
            }
        }

        if (accessibleRolls.size()) {
            hadAccessibleRolls = true;
            for (const auto &r : accessibleRolls) {
                updatedDiagram[r.first][r.second] = '.';
            }

            totalAccessibleRolls.insert(accessibleRolls.begin(), accessibleRolls.end());
        }
    } while (hadAccessibleRolls);

    cout << totalAccessibleRolls.size() << endl;
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