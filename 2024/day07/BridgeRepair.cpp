#include "../../util/BasicIncludes.h"
#include "../../util/StringUtil.h"

#include <vector>
#include <cmath>
#include <regex>

using namespace std;
typedef uint64_t value_t;

const regex EQUATION_RX("([0-9]+): ([0-9 ]+)");

struct Equation {
    value_t testValue;
    vector<value_t> values;
};

enum Operator {
    Add = 0,
    Multiply,
    Concatenation,
    Count,
};

void _printEquation(const Equation &eq) {
    cout << eq.testValue << ": ";
    for (const auto &v : eq.values) {
        cout << v << " ";
    }
    cout << endl;
}

size_t _getNumDigits(const value_t n) {
    size_t numDigits = 0;
    value_t aux = n;
    while (aux > 0) {
        aux = aux / 10;
        numDigits += 1;
    }

    return numDigits;
}

value_t applyOperator(const value_t a, const value_t b, const Operator &op) {
    switch (op) {
        case Operator::Add: return a + b;
        case Operator::Multiply: return a * b;
        case Operator::Concatenation: {
            return a * pow(10, _getNumDigits(b)) + b;
        }
        default: {
            cout << "Unknown operator: " << op << endl;
            return 0;
        }
    }
}

bool evaluateEquation(const value_t resultSoFar, const Operator op, const value_t elementIdToEvaluate, const Equation &eq, bool part2 = false) {
    if (elementIdToEvaluate >= eq.values.size()) {
        return resultSoFar == eq.testValue;
    }

    const value_t newResult = applyOperator(resultSoFar, eq.values[elementIdToEvaluate], op);
    if (newResult > eq.testValue) {
        return false;
    }

    bool isEvaluatedCorrect = false;
    for (size_t opId = 0; opId < Operator::Count; ++opId) {
        const auto newOp = (Operator)opId;
        if (newOp == Operator::Concatenation && !part2) {
            continue;
        }
        isEvaluatedCorrect = isEvaluatedCorrect || evaluateEquation(newResult, newOp, elementIdToEvaluate + 1, eq, part2);
    }

    return isEvaluatedCorrect;
}

bool evaluateEquation(const Equation &eq, bool part2 = false) {
    bool isEvaluatedCorrect = false;
    for (size_t opId = 0; opId < Operator::Count; ++opId) {
        const auto op = (Operator)opId;
        if (op == Operator::Concatenation && !part2) {
            continue;
        }
        isEvaluatedCorrect = isEvaluatedCorrect || evaluateEquation(eq.values.front(), op, 1, eq, part2);
    }

    return isEvaluatedCorrect;
}

void part1(istream& inputFile) {
    vector<Equation> equations;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        smatch match;
        if (regex_match(line, match, EQUATION_RX)) {
            Equation eq = {};
            eq.testValue = stoull(match[1].str());
            const string valuesStr = match[2].str();

            istringstream iss(valuesStr);
            value_t value;
            while (iss >> value) {
                eq.values.push_back(value);
            }

            equations.push_back(eq);
        }
    }

    value_t totalCalibrationResult = 0;
    for (const auto &eq : equations) {
        if (!evaluateEquation(eq)) {
            continue;
        }

        totalCalibrationResult += eq.testValue;
    }

    cout << totalCalibrationResult << endl;
}

void part2(istream& inputFile) {
    vector<Equation> equations;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        smatch match;
        if (regex_match(line, match, EQUATION_RX)) {
            Equation eq = {};
            eq.testValue = stoull(match[1].str());
            const string valuesStr = match[2].str();

            istringstream iss(valuesStr);
            value_t value;
            while (iss >> value) {
                eq.values.push_back(value);
            }

            equations.push_back(eq);
        }
    }

    value_t totalCalibrationResult = 0;
    for (const auto &eq : equations) {
        if (!evaluateEquation(eq, true)) {
            continue;
        }

        // _printEquation(eq);

        totalCalibrationResult += eq.testValue;
    }

    cout << totalCalibrationResult << endl;
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