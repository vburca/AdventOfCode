#include "../../util/BasicIncludes.h"
#include "../../util/StringUtil.h"
#include "../../util/Types.h"
#include "../../util/Graph.h"

#include <regex>
#include <vector>
#include <optional>

using namespace std;
using namespace aoc::util::types;

const regex BUTTON_RX("Button [AB]: X\\+([0-9]+), Y\\+([0-9]+)");
const regex PRIZE_RX("Prize: X=([0-9]+), Y=([0-9]+)");

struct Machine {
    coord_t buttonA;
    coord_t buttonB;
    coord_t prize;
    size_t tokens;
};

void _printMachines(const vector<Machine> &machines) {
    for (const auto &m : machines) {
        cout << "Button A: X+" << m.buttonA.first << ", Y+" << m.buttonA.second << endl;
        cout << "Button B: X+" << m.buttonB.first << ", Y+" << m.buttonB.second << endl;
        cout << "Prize: X=" << m.prize.first << ", Y=" << m.prize.second << endl;
        cout << endl;
    }

    cout << "Number machines: " << machines.size() << endl << endl;
}

const coord_double_t _solveSystem(const double a, const double b, const double c, const double d, const double p, const double q) {
    const double scalar = a * d - b * c;
    const coord_double_t matrixVectorResult = {
        d * p - b * q,
        -1 * c * p + a * q,
    };

    const coord_double_t solution = {
        matrixVectorResult.first / scalar,
        matrixVectorResult.second / scalar,
    };

    return solution;
}

const optional<coord_big_t> _findMinButtonPresses(const Machine &machine, const bool isPart2 = false) {
    coord_double_t buttonPressesFloat;

    if (!isPart2) {
        buttonPressesFloat = _solveSystem(machine.buttonA.first, machine.buttonB.first,
                                            machine.buttonA.second, machine.buttonB.second,
                                            machine.prize.first, machine.prize.second);
    } else {
        constexpr uint64_t kPrizeFactor = 10000000000000;
        buttonPressesFloat = _solveSystem(machine.buttonA.first, machine.buttonB.first,
                                            machine.buttonA.second, machine.buttonB.second,
                                            machine.prize.first + kPrizeFactor, machine.prize.second + kPrizeFactor);
    }

    const coord_big_t buttonPresses = {buttonPressesFloat.first, buttonPressesFloat.second};
    if (buttonPresses.first != buttonPressesFloat.first ||
        buttonPresses.second != buttonPressesFloat.second) {
        return nullopt;
    }

    if (!isPart2) {
        constexpr size_t kMaximumPresses = 100;
        if (buttonPresses.first > kMaximumPresses || buttonPresses.second > kMaximumPresses) {
            return nullopt;
        }
    }

    return buttonPresses;
}

void part1(istream& inputFile) {
    vector<Machine> machines;

    while (!inputFile.eof()) {
        string line;
        smatch match;

        Machine machine;

        getline(inputFile, line);
        if (line.empty()) {
            continue;
        }
        if (!regex_match(line, match, BUTTON_RX)) {
            cout << "Unable to match Button A regex for line: " << line << endl;
            exit(1);
        }
        machine.buttonA = {stoul(match[1].str()), stoul(match[2].str())};

        getline(inputFile, line);
        if (!regex_match(line, match, BUTTON_RX)) {
            cout << "Unable to match Button B regex for line: " << line << endl;
            exit(1);
        }
        machine.buttonB = {stoul(match[1].str()), stoul(match[2].str())};

        getline(inputFile, line);
        if (!regex_match(line, match, PRIZE_RX)) {
            cout << "Unable to match Prize regex for line: " << line << endl;
            exit(1);
        }
        machine.prize = {stoul(match[1].str()), stoul(match[2].str())};
        machines.push_back(machine);
    }

    // _printMachines(machines);

    uint64_t numTokens = 0;
    for (const auto &machine : machines) {
        const auto buttonPresses = _findMinButtonPresses(machine);

        if (!buttonPresses) {
            continue;
        }

        numTokens += 3 * (uint64_t)(*buttonPresses).first + 1 * (uint64_t)(*buttonPresses).second;
    }

    cout << numTokens << endl;
}

void part2(istream& inputFile) {
    vector<Machine> machines;

    while (!inputFile.eof()) {
        string line;
        smatch match;

        Machine machine;

        getline(inputFile, line);
        if (line.empty()) {
            continue;
        }
        if (!regex_match(line, match, BUTTON_RX)) {
            cout << "Unable to match Button A regex for line: " << line << endl;
            exit(1);
        }
        machine.buttonA = {stoul(match[1].str()), stoul(match[2].str())};

        getline(inputFile, line);
        if (!regex_match(line, match, BUTTON_RX)) {
            cout << "Unable to match Button B regex for line: " << line << endl;
            exit(1);
        }
        machine.buttonB = {stoul(match[1].str()), stoul(match[2].str())};

        getline(inputFile, line);
        if (!regex_match(line, match, PRIZE_RX)) {
            cout << "Unable to match Prize regex for line: " << line << endl;
            exit(1);
        }
        machine.prize = {stoul(match[1].str()), stoul(match[2].str())};
        machines.push_back(machine);
    }

    // _printMachines(machines);

    uint64_t numTokens = 0;
    for (const auto &machine : machines) {
        const auto buttonPresses = _findMinButtonPresses(machine, true);

        if (!buttonPresses) {
            continue;
        }

        numTokens += 3 * (uint64_t)(*buttonPresses).first + 1 * (uint64_t)(*buttonPresses).second;
    }

    cout << numTokens << endl;
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