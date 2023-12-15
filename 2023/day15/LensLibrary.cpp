#include "../../util/StringUtil.h"
#include "../../util/MathUtil.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint>
#include <vector>
#include <map>
#include <regex>

using namespace std;

regex REMOVE_OP_RX("([a-z]+)-");
regex ADD_OP_RX("([a-z]+)=([0-9]+)");

const size_t kNumBoxes = 256;

struct Lens {
    string label;
    size_t focalLength;
};

struct Box {
    vector<string> lensLabels;
    map<string, Lens> labelToLens;
};

enum Operation {
    Removal,
    Update,
};

struct Operator {
    string label;
    size_t hash;
    size_t focalLength;
    Operation operation;
};

struct Input {
    vector<string> steps;
};

const Input _parseInput(istream& inputFile) {
    Input input;

    string step;
    while (getline(inputFile, step, ',')) {
        input.steps.push_back(step);
    }

    return input;
}

void _printInput(const Input &input) {
    cout << "Steps: (" << input.steps.size() << ")" << endl;
    for (const auto &s : input.steps) {
        cout << s << endl;
    }
    cout << endl;
}

size_t _hash(const string s) {
    size_t currentValue = 0;
    for (const auto ch : s) {
        currentValue += int(ch);
        currentValue *= 17;
        currentValue %= kNumBoxes;
    }

    return currentValue;
}

void part1(const Input &input) {
    size_t sum = 0;
    for (const auto &s : input.steps) {
        sum += _hash(s);
    }

    cout << sum << endl;
}

Operator _extractOperator(const string s) {
    Operator op;

    smatch match;
    if (regex_match(s, match, REMOVE_OP_RX)) {
        op.label = match[1].str();
        op.hash = _hash(op.label);
        op.operation = Removal;
    } else if (regex_match(s, match, ADD_OP_RX)) {
        op.label = match[1].str();
        op.hash = _hash(op.label);
        op.focalLength = stoul(match[2].str());
        op.operation = Update;
    } else {
        cout << "Unable to match instruction: " << s << endl;
        exit(1);
    }

    return op;
}

void _updateBox(Box &box, const Operator &op) {
    if (op.operation == Update) {
        // add / update
        if (box.labelToLens.contains(op.label)) {
            // update focal length
            auto &lens = box.labelToLens.at(op.label);
            lens.focalLength = op.focalLength;
            return;
        }

        // add new lens
        Lens lens = {
            .label = op.label,
            .focalLength = op.focalLength,
        };

        box.lensLabels.push_back(lens.label);
        box.labelToLens[lens.label] = lens;
    } else {
        // removal
        if (!box.labelToLens.contains(op.label)) {
            return;
        }

        const auto lensToBeRemoved = box.labelToLens[op.label];

        // find index
        size_t idxToRemove = 0;
        for (size_t i = 0; i < box.lensLabels.size(); i++) {
            if (box.lensLabels[i] == op.label) {
                idxToRemove = i;
                break;
            }
        }

        box.lensLabels.erase(box.lensLabels.begin() + idxToRemove);
        box.labelToLens.erase(op.label);
    }

    return;
}

void part2(const Input &input) {
    array<Box, kNumBoxes> boxHASHMAP;

    for (const auto &step : input.steps) {
        const auto op = _extractOperator(step);
        _updateBox(boxHASHMAP[op.hash], op);
    }

    size_t totalFocusPower = 0;
    for (size_t boxIdx = 0; boxIdx < boxHASHMAP.size(); boxIdx++) {
        const auto &box = boxHASHMAP[boxIdx];
        for (size_t lensIdx = 0; lensIdx < box.lensLabels.size(); lensIdx++) {
            const auto &lensLabel = box.lensLabels[lensIdx];
            const auto &lens = box.labelToLens.at(lensLabel);

            const auto focusPower = (boxIdx + 1) * (lensIdx + 1) * lens.focalLength;
            totalFocusPower += focusPower;
        }
    }

    cout << totalFocusPower << endl;
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        std::cout << "Must specify input file!" << endl;
        return 0;
    }

    std::cout << "Input file name: " << argv[1] << endl;
    ifstream inputFile(argv[1]);

    const auto input = _parseInput(inputFile);
    // _printInput(input);

    std::cout << "Part 1: " << endl;
    part1(input);

    std::cout << "Part 2: " << endl;
    part2(input);

    inputFile.close();

    return 0;
}