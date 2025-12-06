#include "../../util/BasicIncludes.h"
#include "../../util/MathUtil.h"
#include "../../util/StringUtil.h"
#include "../../util/Types.h"

using namespace std;

void part1(istream& inputFile) {
    vector<vector<uint64_t>> problems;
    vector<char> operators;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);
        istringstream iss(line);

        if (line[0] == '+' || line[0] == '*') {
            char op;
            while (iss >> op) {
                operators.push_back(op);
            }
        } else {
            vector<uint64_t> problemNums;
            size_t number;
            while (iss >> number) {
                problemNums.push_back(number);
            }

            problems.push_back(problemNums);
        }
    }

    uint64_t grandTotal = 0;
    for (size_t problemId = 0; problemId < problems.front().size(); problemId++) {
        const auto op = operators[problemId];
        uint64_t problemTotal = 0;
        for (size_t numberId = 0; numberId < problems.size(); numberId++) {
            if (numberId == 0) {
                problemTotal = problems[numberId][problemId];
            } else {
                switch (op) {
                    case '+': problemTotal += problems[numberId][problemId]; break;
                    case '*': problemTotal *= problems[numberId][problemId]; break;
                    default: cout << "Unknown operator= " << op << endl; problemTotal = 0; break;
                }
            }
        }
        grandTotal += problemTotal;
    }

    cout << grandTotal << endl;
}

vector<uint64_t> getVerticalNumbers(const vector<uint64_t> &numbers) {
    vector<uint64_t> verticalNumbers;


    return verticalNumbers;
}

void part2(istream& inputFile) {
    vector<string> numbers;
    vector<char> operators;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);
        istringstream iss(line);

        if (line[0] == '+' || line[0] == '*') {
            char op;
            while (iss >> op) {
                operators.push_back(op);
            }
        } else {
            numbers.push_back(line);
        }
    }

    size_t maxColumnSize = 0;
    for (const auto &numbersLine : numbers) {
        maxColumnSize = max(maxColumnSize, numbersLine.size());
    }

    uint64_t grandTotal = 0;
    size_t problemIdx = 0;
    vector<uint64_t> problemNumbers;
    for (int columnIdx = maxColumnSize - 1; columnIdx >= 0; columnIdx--) {
        string numberStr;
        for (size_t digitIdx = 0; digitIdx < numbers.size(); digitIdx++) {
            if (columnIdx >= numbers[digitIdx].size()) {
                continue;
            }

            const auto ch = numbers[digitIdx][columnIdx];
            numberStr.push_back(ch);
        }

        aoc::util::string::trim(numberStr);

        if (!numberStr.empty()) {
            uint64_t number = stoull(numberStr);
            problemNumbers.push_back(number);
        }

        if (numberStr.empty() || columnIdx == 0) {
            const auto op = operators[operators.size() - problemIdx - 1];

            uint64_t problemTotal = problemNumbers.front();
            for (size_t i = 1; i < problemNumbers.size(); i++) {
                switch (op) {
                    case '+': problemTotal += problemNumbers[i]; break;
                    case '*': problemTotal *= problemNumbers[i]; break;
                    default: cout << "Unknown operator= " << op << endl; problemTotal = 0; exit(1);
                }
            }

            grandTotal += problemTotal;

            // reset everything
            problemNumbers.clear();
            problemIdx += 1;
        }
    }

    cout << grandTotal << endl;
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