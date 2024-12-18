#include "../../util/BasicIncludes.h"
#include "../../util/StringUtil.h"
#include "../../util/Types.h"
#include "../../util/MathUtil.h"
#include "../../util/Graph.h"

#include <vector>
#include <regex>
#include <iterator>
#include <set>

using namespace std;
using namespace aoc::util::types;
using namespace aoc::util::graph;
using namespace aoc::util::math;

const regex REGISTER_RX("Register [ABC]: ([0-9]+)");
const regex PROGRAM_RX("Program: (.*)");

enum RegisterIdx {
    kA = 0,
    kB,
    kC,
    kCount,
};

struct ComputerState {
    vector<uint64_t> registers = vector<uint64_t>(RegisterIdx::kCount, 0ull);
    vector<size_t> program;
    size_t instructionPtr = 0;
    vector<uint64_t> output;
    bool isHalted = false;

    void reset() {
        isHalted = false;
        instructionPtr = 0;
        for (size_t i = 0; i < registers.size(); ++i) {
            registers[i] = 0;
        }
        output.clear();
        output.resize(0);
    }
};

void _printComputerState(const ComputerState &s) {
    for (size_t i = 0; i < RegisterIdx::kCount; ++i) {
        char registerName = 'A' + i;
        cout << "Register " << registerName << ": " << s.registers[i] << endl;
    }
    cout << endl;
    cout << "Program: ";
    for (size_t i = 0; i < s.program.size(); ++i) {
        const auto v = s.program[i];
        cout << v;
        if (i < s.program.size() - 1) {
            cout << ",";
        }
    }
    cout << endl;
}

const string _constructOutputString(const vector<uint64_t> &output) {
    stringstream ss;
    copy(output.begin(), output.end(), ostream_iterator<uint64_t>(ss, ","));

    string result = ss.str();
    result = result.substr(0, result.length() - 1);

    return result;
}

const uint64_t _comboOperand(const uint64_t operand, const ComputerState &s) {
    if (operand >= 0 && operand <= 3) {
        return operand;
    }

    if (operand == 4) {
        return s.registers[RegisterIdx::kA];
    }

    if (operand == 5) {
        return s.registers[RegisterIdx::kB];
    }

    if (operand == 6) {
        return s.registers[RegisterIdx::kC];
    }

    if (operand == 7) {
        cout << "Operand 7 encountered! Program is not valid!" << endl;
        exit(1);
    }

    cout << "Unknown Operand: " << operand << endl;
    exit(1);
}

// opcode: 0
void _adv(const uint64_t operand, ComputerState &s) {
    const uint64_t numerator = s.registers[RegisterIdx::kA];
    const uint64_t denominator = 1 << _comboOperand(operand, s);
    s.registers[RegisterIdx::kA] = numerator / denominator;

    s.instructionPtr += 2;
}

// opcode: 1
void _bxl(const uint64_t operand, ComputerState &s) {
    s.registers[RegisterIdx::kB] = s.registers[RegisterIdx::kB] ^ operand;

    s.instructionPtr += 2;
}

// opcode: 2
void _bst(const uint64_t operand, ComputerState &s) {
    // combo MODULO 8 == combo & 7;
    s.registers[RegisterIdx::kB] = _comboOperand(operand, s) & 7;

    s.instructionPtr += 2;
}

// opcode: 3
void _jnz(const uint64_t operand, ComputerState &s) {
    if (s.registers[RegisterIdx::kA] == 0) {
        s.instructionPtr += 2;
        return;
    }

    s.instructionPtr = operand;
}

// opcode: 4
void _bxc(const uint64_t operand, ComputerState &s) {
    (void)operand; // legacy, not used
    s.registers[RegisterIdx::kB] = s.registers[RegisterIdx::kB] ^ s.registers[RegisterIdx::kC];

    s.instructionPtr += 2;
}

// opcode: 5
void _out(const uint64_t operand, ComputerState &s) {
    const uint64_t res = _comboOperand(operand, s) & 7;
    s.output.push_back(res);

    s.instructionPtr += 2;
}

// opcode: 6
void _bdv(const uint64_t operand, ComputerState &s) {
    const uint64_t numerator = s.registers[RegisterIdx::kA];
    const uint64_t denominator = 1 << _comboOperand(operand, s);
    s.registers[RegisterIdx::kB] = numerator / denominator;

    s.instructionPtr += 2;
}

// opcode: 7
void _cdv(const uint64_t operand, ComputerState &s) {
    const uint64_t numerator = s.registers[RegisterIdx::kA];
    const uint64_t denominator = 1 << _comboOperand(operand, s);
    s.registers[RegisterIdx::kC] = numerator / denominator;

    s.instructionPtr += 2;
}

void _interpretProgram(ComputerState &s) {
    while (!s.isHalted) {
        if (s.instructionPtr >= s.program.size()) {
            s.isHalted = true;
            break;
        }

        const size_t opcode = s.program[s.instructionPtr];
        const size_t operand = s.program[s.instructionPtr + 1];
        switch (opcode) {
            case 0: _adv(operand, s); break;
            case 1: _bxl(operand, s); break;
            case 2: _bst(operand, s); break;
            case 3: _jnz(operand, s); break;
            case 4: _bxc(operand, s); break;
            case 5: _out(operand, s); break;
            case 6: _bdv(operand, s); break;
            case 7: _cdv(operand, s); break;
            default: cout << "Unknown opcode: " << opcode << endl; exit(1);
        }
    }
}

void part1(istream& inputFile) {
    ComputerState s;

    string line;

    // read registers
    for (size_t i = 0; i < RegisterIdx::kCount; ++i) {
        getline(inputFile, line);

        smatch match;
        if (!regex_match(line, match, REGISTER_RX)) {
            cout << "Can not read Register line: " << line << endl;
            exit(1);
        }

        s.registers[i] = stoull(match[1]);
    }
    // read new line
    getline(inputFile, line);

    // read program
    getline(inputFile, line);
    smatch match;
    if (!regex_match(line, match, PROGRAM_RX)) {
        cout << "Can not read Program line: " << line << endl;
        exit(1);
    }

    istringstream iss(match[1]);
    uint64_t v;
    while (iss >> v) {
        if (iss.peek() == ',') {
            iss.ignore();
        }
        s.program.push_back(v);
    }

    // _printComputerState(s);
    // cout << endl;

    _interpretProgram(s);
    const auto outputStr = _constructOutputString(s.output);

    _printComputerState(s);
    cout << endl;

    cout << outputStr << endl;
}

void part2(istream& inputFile) {
    ComputerState s;

    string line;

    // read registers
    for (size_t i = 0; i < RegisterIdx::kCount; ++i) {
        getline(inputFile, line);

        smatch match;
        if (!regex_match(line, match, REGISTER_RX)) {
            cout << "Can not read Register line: " << line << endl;
            exit(1);
        }

        s.registers[i] = stoull(match[1]);
    }
    // read new line
    getline(inputFile, line);

    // read program
    getline(inputFile, line);
    smatch match;
    if (!regex_match(line, match, PROGRAM_RX)) {
        cout << "Can not read Program line: " << line << endl;
        exit(1);
    }

    istringstream iss(match[1]);
    uint64_t v;
    while (iss >> v) {
        if (iss.peek() == ',') {
            iss.ignore();
        }
        s.program.push_back(v);
    }

    struct RegisterValuesState {
        uint64_t registerAValue;
        size_t matchedLastNDigits;
    };

    set<uint64_t> validRegisterAValues;

    stack<RegisterValuesState> registerAValuesToCheck;
    registerAValuesToCheck.push(RegisterValuesState {
        .registerAValue = 0,
        .matchedLastNDigits = 0,
    });

    while (!registerAValuesToCheck.empty()) {
        const auto top = registerAValuesToCheck.top();
        registerAValuesToCheck.pop();

        const auto registerAValue = top.registerAValue;
        auto matchedLastNDigits = top.matchedLastNDigits;

        if (matchedLastNDigits == s.program.size()) {
            validRegisterAValues.insert(registerAValue);
            continue;
        }

        matchedLastNDigits += 1;
        for (size_t remainder = 0; remainder < 8; ++remainder) {
            const uint64_t newRegisterAValue = (registerAValue << 3) + (uint64_t)remainder;
            // cout << endl << "checking registerA=" << newRegisterAValue << endl;

            s.reset();
            s.registers[RegisterIdx::kA] = newRegisterAValue;
            _interpretProgram(s);

            // const auto outputStr = _constructOutputString(s.output);
            // cout << outputStr << endl;

            // check the output for the last `matchedLastNDigits` to see if it matches
            bool matchedOutputDigits = true;
            for (size_t digitId = 0; digitId < matchedLastNDigits; ++digitId) {
                const auto programLastNthDigit = s.program.size() - matchedLastNDigits + digitId;
                const auto programDigit = s.program[programLastNthDigit];
                if (s.output[digitId] != programDigit) {
                    // cout << "s.output[digitId]=" << s.output[digitId] << " != programDigit=" << programDigit << endl;
                    matchedOutputDigits = false;
                    break;
                }
            }

            if (!matchedOutputDigits) {
                continue;
            }

            // cout << "adding registerA=" << newRegisterAValue << endl;

            registerAValuesToCheck.push(RegisterValuesState {
                .registerAValue = newRegisterAValue,
                .matchedLastNDigits = matchedLastNDigits,
            });
        }
    }

    cout << "Found " << validRegisterAValues.size() << " values for registerA:" << endl;
    for (const auto &v : validRegisterAValues) {
        cout << v << endl;
    }

    const auto minRegisterA = *validRegisterAValues.begin();\
    cout << "Minimum registerA value: " << endl;
    cout << minRegisterA << endl;
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