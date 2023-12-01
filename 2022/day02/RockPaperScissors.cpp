#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <cstdint>

using namespace std;

enum Shape {
    None = 0,
    Rock,
    Paper,
    Scissors,
};

enum Outcome {
    Defeat = 0,
    Draw = 3,
    Win = 6,
};

unordered_map<Shape, Shape> _defeats = {
    { Shape::Rock, Shape::Scissors },
    { Shape::Scissors, Shape::Paper },
    { Shape::Paper, Shape::Rock },
};

unordered_map<Shape, Shape> _loses = {
    { Shape::Scissors, Shape::Rock },
    { Shape::Paper, Shape::Scissors },
    { Shape::Rock, Shape::Paper },
};

unordered_map<char, Shape> _opponentShapes = {
    { 'A', Shape::Rock },
    { 'B', Shape::Paper },
    { 'C', Shape::Scissors },
};

unordered_map<char, Shape> _inputToShape = {
    { 'X', Shape::Rock },
    { 'Y', Shape::Paper },
    { 'Z', Shape::Scissors },
};

unordered_map<char, Outcome> _inputToOutcome = {
    { 'X', Outcome::Defeat },
    { 'Y', Outcome::Draw },
    { 'Z', Outcome::Win },
};

const Outcome _getOutcome(const Shape& myShape, const Shape& opponentShape) {
    if (myShape == opponentShape) {
        return Outcome::Draw;
    }

    const bool win = (_defeats[myShape] == opponentShape);
    if (win) {
        return Outcome::Win;
    }

    return Outcome::Defeat;
}

const Shape _getShapeFromOutcome(const Shape& opponentShape, const Outcome& outcome) {
    if (outcome == Outcome::Draw) {
        return opponentShape;
    }

    if (outcome == Outcome::Defeat) {
        return _defeats[opponentShape];
    }

    return _loses[opponentShape];
}

void part1(istream& inputFile)
{
    uint64_t totalScore = 0;
    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        istringstream iss(line);
        char ch;

        iss >> ch;
        const auto opponentShape = _opponentShapes[ch];
        iss >> ch;
        const auto myShape = _inputToShape[ch];

        totalScore += static_cast<uint64_t>(myShape) + _getOutcome(myShape, opponentShape);
    }

    cout << totalScore << endl;
}

void part2(istream& inputFile)
{
    uint64_t totalScore = 0;
    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        istringstream iss(line);
        char ch;

        iss >> ch;
        const auto opponentShape = _opponentShapes[ch];
        iss >> ch;
        const auto outcome = _inputToOutcome[ch];

        totalScore += static_cast<uint64_t>(_getShapeFromOutcome(opponentShape, outcome)) + static_cast<uint64_t>(outcome);
    }

    cout << totalScore << endl;
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

    cout << "Part 1: " << endl;
    part1(inputFile);

    inputFile.clear();
    inputFile.seekg(0, ios::beg);

    cout << "Part 2: " << endl;
    part2(inputFile);

    inputFile.close();

    return 0;
}