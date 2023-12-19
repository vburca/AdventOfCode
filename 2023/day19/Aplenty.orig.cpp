#include "../../util/StringUtil.h"
#include "../../util/MathUtil.h"
#include "../../util/Types.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint>
#include <vector>
#include <regex>
#include <optional>
#include <map>

using namespace std;
using namespace aoc::util::types;

regex WORKFLOW_RX("([a-z]+)\\{(.*)\\}");
regex DECISION_RULE_RX("([xmas])([<>])([0-9]+)\\:([a-zAR]+)");
regex TERMINAL_RULE_RX("([a-zAR]+)");
regex PART_RX("\\{x=([0-9]+),m=([0-9]+),a=([0-9]+),s=([0-9]+)\\}");

struct Part {
    size_t x;
    size_t m;
    size_t a;
    size_t s;
};

struct Rule {
    char category;
    char op;
    size_t threshold;
    string result;
    bool isTerminal;

    optional<string> evaluate(const Part &part) const {
        if (isTerminal) {
            return result;
        }

        const auto categoryValue = _getCategoryValue(part);

        switch (op) {
            case '>': {
                if (categoryValue > threshold) {
                    return result;
                }

                return nullopt;
            }
            case '<': {
                if (categoryValue < threshold) {
                    return result;
                }

                return nullopt;
            }
            default: {
                cout << "Unknown operator: " << op << endl;
                exit(1);
            }
        }

        return nullopt;
    }

private:
    size_t _getCategoryValue(const Part &part) const {
        switch (category) {
            case 'x': return part.x;
            case 'm': return part.m;
            case 'a': return part.a;
            case 's': return part.s;
            default: {
                cout << "Unknown category: " << category << endl;
                exit(1);
            }
        }

        return 0;
    }
};

struct Input {
    map<string, vector<Rule>> workflowToRules;
    vector<Part> parts;
};

void _printRule(const Rule &rule) {
    if (!rule.isTerminal) {
        cout << rule.category << rule.op << rule.threshold << ":" << rule.result;
    } else {
        cout << rule.result;
    }
}

void _printPart(const Part &part) {
    cout << "{x=" << part.x << ",m=" << part.m << ",a=" << part.a << ",s=" << part.s << "}";
}

const Input _parseInput(istream& inputFile) {
    Input input;

    bool partsInput = false;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        if (line.size() == 0) {
            partsInput = true;
            continue;
        }

        if (!partsInput) {
            // parse workflows
            smatch workflowMatch;
            if (!regex_match(line, workflowMatch, WORKFLOW_RX)) {
                cout << "WORKFLOW_RX failed to match line: { " << line << " }" << endl;
                exit(1);
            }

            string workflowName = workflowMatch[1].str();
            string workflowRulesStr = workflowMatch[2].str();
            vector<string> rulesStr = aoc::util::string::split(workflowRulesStr, ',');

            input.workflowToRules[workflowName] = vector<Rule>();
            auto &rules = input.workflowToRules[workflowName];
            for (const auto &ruleStr : rulesStr) {
                smatch ruleMatch;
                if (regex_match(ruleStr, ruleMatch, DECISION_RULE_RX)) {
                    rules.emplace_back(Rule {
                        .category = ruleMatch[1].str()[0],
                        .op = ruleMatch[2].str()[0],
                        .threshold = stoull(ruleMatch[3].str()),
                        .result = ruleMatch[4].str(),
                        .isTerminal = false,
                    });
                    continue;
                }

                if (regex_match(ruleStr, ruleMatch, TERMINAL_RULE_RX)) {
                    rules.emplace_back(Rule {
                        .result = ruleMatch[1].str(),
                        .isTerminal = true,
                    });
                    continue;
                }

                cout << "DECISION_RULE_RX and TERMINAL_RULE_RX failed to match rule: " << ruleStr << endl;
                exit(1);
            }
        } else {
            // parse parts
            smatch partMatch;
            if (!regex_match(line, partMatch, PART_RX)) {
                cout << "PART_RX failed to match line: { " << line << " }" << endl;
                exit(1);
            }

            input.parts.emplace_back(Part {
                .x = stoull(partMatch[1].str()),
                .m = stoull(partMatch[2].str()),
                .a = stoull(partMatch[3].str()),
                .s = stoull(partMatch[4].str()),
            });
        }
    }

    return input;
}

void _printInput(const Input &input, bool printMap = true) {
    cout << "Workflows: (" << input.workflowToRules.size() << ")" << endl;
    for (const auto &p : input.workflowToRules) {
        const auto workflowName = p.first;
        const auto workflowRules = p.second;

        cout << workflowName << "{";
        for (size_t i = 0; i < workflowRules.size(); i++) {
            const auto rule = workflowRules[i];
            _printRule(rule);

            if (i < workflowRules.size() - 1) {
                cout << ",";
            } else {
                cout << "}" << endl;
            }
        }
    }
    cout << endl;

    cout << "Parts: (" << input.parts.size() << ")" << endl;
    for (const auto &part : input.parts) {
        _printPart(part);
        cout << endl;
    }
    cout << endl;
}

bool _processPartWithWorkflow(const Part &part, const string &workflowName, const Input &input) {
    if (!input.workflowToRules.contains(workflowName)) {
        cout << "Unknown workflow name: " << workflowName << endl;
        exit(1);
    }

    const auto rules = input.workflowToRules.at(workflowName);
    for (const auto &rule : rules) {
        const auto evaluateOpt = rule.evaluate(part);
        if (evaluateOpt) {
            const auto result = *evaluateOpt;
            if (result == "A") {
                return true;
            }

            if (result == "R") {
                return false;
            }

            return _processPartWithWorkflow(part, result, input);
        }
    }

    cout << "Processed all rules of workflow " << workflowName << " and did not find terminal result for part: " << endl;
    _printPart(part);
    cout << endl;
    exit(1);

    return false;
}

void part1(const Input &input) {
    size_t totalRating = 0;
    for (const auto &part : input.parts) {
        if (!_processPartWithWorkflow(part, "in", input)) {
            continue;
        }

        totalRating += part.x + part.m + part.a + part.s;
    }

    cout << totalRating << endl;
}

void part2(const Input &input) {
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        std::cout << "Must specify input file!" << endl;
        return 0;
    }

    std::cout << "Input file name: " << argv[1] << endl;
    ifstream inputFile(argv[1]);

    const auto input = _parseInput(inputFile);
    _printInput(input);

    std::cout << "Part 1: " << endl;
    part1(input);

    std::cout << "Part 2: " << endl;
    part2(input);

    inputFile.close();

    return 0;
}