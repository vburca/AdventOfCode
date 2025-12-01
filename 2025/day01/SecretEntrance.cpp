#include "../../util/BasicIncludes.h"
#include "../../util/MathUtil.h"

#include <regex>

using namespace std;

const regex INSTRUCTION_RX("([L|R])([0-9]+)");

void part1(istream& inputFile) {
    short int dial = 50;
    size_t numZeros = 0;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        smatch match;
        if (regex_match(line, match, INSTRUCTION_RX)) {
            const char direction = match[1].str()[0];
            const short int distance = stoull(match[2].str());

            const short int sign = direction == 'L' ? -1 : 1;

            dial = aoc::util::math::positiveModulo(dial + sign * distance, 100);
            numZeros += dial == 0 ? 1 : 0;
        }
    }

    cout << numZeros << endl;
}

void part2(istream& inputFile) {
    short int dial = 50;
    size_t numZeros = 0;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        smatch match;
        if (regex_match(line, match, INSTRUCTION_RX)) {
            const char direction = match[1].str()[0];
            const short int distance = stoull(match[2].str());

            const short int sign = direction == 'L' ? -1 : 1;

            const short int rotations = distance / 100;
            const short int distanceMod = aoc::util::math::positiveModulo(distance, 100);
            const short int res = dial + sign * distanceMod;
            numZeros += ((dial > 0 && res <= 0) || res >= 100) ? rotations + 1 : rotations;
            dial = aoc::util::math::positiveModulo(res, 100);
        } else {
            cout << "Line { " << line << " } did not match regex" << endl;
        }
    }

    cout << numZeros << endl;
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