#include "../../util/BasicIncludes.h"
#include "../../util/MathUtil.h"
#include "../../util/StringUtil.h"
#include "../../util/Types.h"

#include <queue>
#include <unordered_set>

using namespace std;
using namespace aoc::util::types;

using ll = long long;

const string pressButton(const string &lights, const vector<size_t> &button) {
    string newLights = lights;
    for (const auto &b : button) {
        if (lights[b] == '.') {
            newLights[b] = '#';
        } else {
            newLights[b] = '.';
        }
    }

    return newLights;
}

const size_t getButtonPressesP1(const string &goalLights, const vector<vector<size_t>> &buttonSchematics) {
    const string initialLights(goalLights.size(), '.');

    // state: {current lights configuration, number of button presses}
    queue<pair<string, size_t>> states;
    unordered_set<string> visited;

    states.emplace(initialLights, 0);
    visited.insert(initialLights);

    while (!states.empty()) {
        auto [currentLights, buttonPresses] = states.front();
        states.pop();

        if (currentLights == goalLights) {
            return buttonPresses;
        }

        // press each button
        for (size_t i = 0; i < buttonSchematics.size(); i++) {
            const string nextLights = pressButton(currentLights, buttonSchematics[i]);

            if (visited.contains(nextLights)) {
                continue;
            }

            visited.insert(nextLights);
            states.emplace(nextLights, buttonPresses + 1);
        }
    }

    return 0;
}

const string serializeJoltages(const vector<size_t> &joltages) {
    string serialization;
    for (const auto &j : joltages) {
        serialization += to_string(j) + ",";
    }

    return serialization;
}

const vector<vector<size_t>> convertButtons(const size_t numJoltages, const vector<vector<size_t>> &buttonSchematics) {
    vector<vector<size_t>> joltageIncrements;

    for (const auto &button : buttonSchematics) {
        vector<size_t> increments(numJoltages, 0);
        for (const auto &b : button) {
            increments[b] = 1;
        }
        joltageIncrements.push_back(increments);
    }

    return joltageIncrements;
}

// This is TOO SLOW for the search space...
const size_t getButtonPressesP2BFS(const vector<size_t> &goalJoltages, const vector<vector<size_t>> &buttonSchematics) {
    // state: {current joltage values, number of button presses}
    queue<pair<vector<size_t>, size_t>> states;
    unordered_set<string> visited;

    vector<size_t> startJoltages(goalJoltages.size(), 0);
    states.emplace(startJoltages, 0);
    visited.insert(serializeJoltages(startJoltages));

    const auto joltageIncrements = convertButtons(goalJoltages.size(), buttonSchematics);

    while (!states.empty()) {
        auto [currentJoltages, buttonPresses] = states.front();
        states.pop();

        if (currentJoltages == goalJoltages) {
            return buttonPresses;
        }

        // press each button
        for (size_t i = 0; i < buttonSchematics.size(); i++) {
            vector<size_t> nextJoltages = currentJoltages;
            bool isValid = true;

            for (size_t j = 0; j < nextJoltages.size(); j++) {
                nextJoltages[j] += joltageIncrements[i][j];
                if (nextJoltages[i] > goalJoltages[i]) {
                    isValid = false;
                    break;
                }
            }

            if (isValid) {
                const auto key = serializeJoltages(nextJoltages);
                if (visited.contains(key)) {
                    continue;
                }

                visited.insert(key);
                states.emplace(nextJoltages, buttonPresses + 1);
            }
        }
    }

    return 0;
}

// Branch-and-bound solver: find non-negative integer x such that A x = b and sum(x) is minimized
uint64_t getButtonPressesP2(const vector<vector<ll>>& A, const vector<ll>& b) {
    int m = A.size();   // number of counters
    int n = A[0].size(); // number of buttons

    ll best = LLONG_MAX / 2; // a very large number

    // Upper bound for each button (safe but tight enough)
    vector<ll> upper(n, 0);
    for (int j = 0; j < n; ++j) {
        for (int i = 0; i < m; ++i) {
            if (A[i][j] > 0) upper[j] = max(upper[j], b[i]);
        }
        upper[j] += 100; // safety margin
    }

    vector<ll> x(n, 0); // current solution

    // Recursive function that tries all possible values for the remaining buttons
    function<void(int, ll)> search = [&](int button, ll current_presses) {
        if (current_presses >= best) return; // prune: already worse than best found

        if (button == n) {
            // Check if this solution satisfies A x == b
            vector<ll> result(m, 0);
            for (int j = 0; j < n; ++j) {
                for (int i = 0; i < m; ++i) {
                    result[i] += x[j] * A[i][j];
                }
            }
            if (result == b) {
                best = current_presses;
            }
            return;
        }

        // Try every possible number of presses for this button
        for (ll presses = 0; presses <= upper[button]; ++presses) {
            x[button] = presses;
            search(button + 1, current_presses + presses);
            // Early stop if adding more presses can't improve the best
            if (current_presses + presses >= best) break;
        }
        x[button] = 0; // reset
    };

    search(0, 0); // start from button 0 with 0 presses

    return (best == LLONG_MAX / 2) ? 0 : best;
}

// Build matrix A (rows = counters, columns = buttons)
vector<vector<ll>> buildMatrixA(size_t numCounters, const vector<vector<size_t>>& buttonSchematic) {
    vector<vector<ll>> A(numCounters, vector<ll>(buttonSchematic.size(), 0));
    for (size_t j = 0; j < buttonSchematic.size(); ++j) {
        for (size_t c : buttonSchematic[j]) {
            if (c < numCounters) A[c][j] = 1;
        }
    }
    return A;
}

void part1(istream& inputFile) {
    uint64_t totalButtonPresses = 0;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        string goalLights = "";
        vector<vector<size_t>> buttonSchematics;
        vector<size_t> joltageRequirements;

        const auto manualStr = aoc::util::string::split(line, ' ');
        goalLights = manualStr[0].substr(1, manualStr[0].size() - 2);

        for (size_t i = 1; i < manualStr.size() - 1; i++) {
            const auto buttonSchematicsRaw = manualStr[i];
            const auto buttonSchematicsStr = buttonSchematicsRaw.substr(1, buttonSchematicsRaw.size() - 2);
            const auto buttonsValues = aoc::util::string::split(buttonSchematicsStr, ',');

            vector<size_t> buttons;
            for (const auto &b : buttonsValues) {
                buttons.push_back(stoull(b));
            }
            buttonSchematics.push_back(buttons);
        }

        const auto joltagesRaw = manualStr[manualStr.size() - 1];
        const auto joltagesStr = joltagesRaw.substr(1, joltagesRaw.size() - 2);
        const auto joltageValues = aoc::util::string::split(joltagesStr, ',');
        for (const auto &j : joltageValues) {
            joltageRequirements.push_back(stoull(j));
        }

        const auto numButtonPresses = getButtonPressesP1(goalLights, buttonSchematics);
        totalButtonPresses += numButtonPresses;
    }

    cout << totalButtonPresses << endl;
}

void part2(istream& inputFile) {
    uint64_t totalButtonPresses = 0;
    size_t i = 0;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        string goalLights = "";
        vector<vector<size_t>> buttonSchematics;
        vector<ll> joltageRequirements;

        const auto manualStr = aoc::util::string::split(line, ' ');
        goalLights = manualStr[0].substr(1, manualStr[0].size() - 2);

        for (size_t i = 1; i < manualStr.size() - 1; i++) {
            const auto buttonSchematicsRaw = manualStr[i];
            const auto buttonSchematicsStr = buttonSchematicsRaw.substr(1, buttonSchematicsRaw.size() - 2);
            const auto buttonsValues = aoc::util::string::split(buttonSchematicsStr, ',');

            vector<size_t> buttons;
            for (const auto &b : buttonsValues) {
                buttons.push_back(stoull(b));
            }
            buttonSchematics.push_back(buttons);
        }

        const auto joltagesRaw = manualStr[manualStr.size() - 1];
        const auto joltagesStr = joltagesRaw.substr(1, joltagesRaw.size() - 2);
        const auto joltageValues = aoc::util::string::split(joltagesStr, ',');
        for (const auto &j : joltageValues) {
            joltageRequirements.push_back(stoull(j));
        }

        cout << "i= " << i << endl;

        const auto numButtonPresses = getButtonPressesP2(buildMatrixA(joltageRequirements.size(), buttonSchematics), joltageRequirements);
        totalButtonPresses += numButtonPresses;

        i += 1;
    }

    cout << totalButtonPresses << endl;
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