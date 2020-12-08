#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <regex>
#include <utility>

using namespace std;

struct Instruction
{
    Instruction(string o, int32_t a)
    {
        op = o;
        arg = a;
    }

    string op;
    int32_t arg;
};

void processInput(
    istream& inputFile,
    vector<pair<Instruction, bool>>& instructions)
{
    const regex rx("(nop|acc|jmp) (.*)");

    // read input line by line
    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        smatch match;
        if (regex_match(line, match, rx))
        {
            string op = match[1].str();
            int32_t arg;
            istringstream ss(match[2].str());
            ss >> arg;

            instructions.emplace_back(Instruction(op, arg), false);
        }
        else
        {
            cout << "Line did not match instruction regex: " << line << endl;
            exit(1);
        }
    }
}

bool runProgram(vector<pair<Instruction, bool>>& instructions, int32_t& accumRet)
{
    int32_t accum = 0;
    size_t pos = 0;

    vector<pair<Instruction, bool>> instr = instructions;

    while (pos < instr.size() && !instr[pos].second)
    {
        // get instruction
        auto& instruction = instr[pos].first;

        // set instruction as executed
        instr[pos].second = true;

        // execute it based on which operation it represents
        if (instruction.op == "nop")
        {
            pos += 1;
        }
        else if (instruction.op == "jmp")
        {
            pos += instruction.arg;
        }
        else if (instruction.op == "acc")
        {
            accum += instruction.arg;
            pos += 1;
        }
        else
        {
            cout << "Unknown operator: ( " << instruction.op << ", " << instruction.arg << " )" << endl;
            exit(1);
        }
    }

    bool hasLoop = false;
    if (pos < instr.size())
    {
        hasLoop = instr[pos].second;
    }

    accumRet = accum;
    return hasLoop;
}

void part1(vector<pair<Instruction, bool>>& instructions)
{
    int32_t accum = 0;

    runProgram(instructions, accum);

    cout << accum << endl;
}

void part2(vector<pair<Instruction, bool>>& instructions)
{
    bool hasLoop = true;
    int32_t accum = 0;

    for (size_t i = 0; i < instructions.size() && hasLoop; ++i)
    {
        accum = 0;
        if (instructions[i].first.op == "nop")
        {
            vector<pair<Instruction, bool>> modifiedInstructions = instructions;
            modifiedInstructions[i].first.op = "jmp";

            hasLoop = runProgram(modifiedInstructions, accum);
        }
        else if (instructions[i].first.op == "jmp")
        {
            vector<pair<Instruction, bool>> modifiedInstructions = instructions;
            modifiedInstructions[i].first.op = "nop";

            hasLoop = runProgram(modifiedInstructions, accum);
        }
    }

    cout << accum << endl;
}


int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        cout << "Must specify input file!" << endl;
        return 0;
    }

    cout << "Input file name: " << argv[1] << endl;
    ifstream inputFile(argv[1]);

    vector<pair<Instruction, bool>> instructions;

    processInput(inputFile, instructions);

    cout << "Part 1: " << endl;
    part1(instructions);

    inputFile.clear();
    inputFile.seekg(0, ios::beg);

    cout << "Part 2: " << endl;
    part2(instructions);

    inputFile.close();

    return 0;
}