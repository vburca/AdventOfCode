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

constexpr size_t kCategoryMinValue = 1;
constexpr size_t kCategoryMaxValue = 4000;

struct Category {
    size_t minValue;
    size_t maxValue;

    size_t getCount() const {
        return maxValue - minValue + 1;
    }
};

struct Part {
    Category x;
    Category m;
    Category a;
    Category s;

    size_t getCount() const {
        return x.getCount() * m.getCount() * a.getCount() * s.getCount();
    }

    void updateCategory(const char categoryName, const Category &category) {
        switch (categoryName) {
            case 'x': x = category; break;
            case 'm': m = category; break;
            case 'a': a = category; break;
            case 's': s = category; break;
            default: {
                cout << "Unknown category to update: " << categoryName << endl;
                exit(1);
            }
        }
    }
};

struct Rule {
    char category;
    char op;
    size_t threshold;
    string result;
    bool isTerminal;

    vector<Part> split(const Part &part) const {
        if (isTerminal) {
            return { part };
        }

        const auto categoryValue = _getCategoryValue(part);

        if (categoryValue.minValue == categoryValue.maxValue) {
            // we can not split single interval
            return { part };
        }

        switch (op) {
            case '<': {
                if (categoryValue.maxValue < threshold || categoryValue.minValue >= threshold) {
                    return { part };
                }

                Part lowerPart = part;
                lowerPart.updateCategory(category, {
                    .minValue = categoryValue.minValue,
                    .maxValue = threshold - 1,
                });

                Part upperPart = part;
                upperPart.updateCategory(category, {
                    .minValue = threshold,
                    .maxValue = categoryValue.maxValue,
                });

                return { lowerPart, upperPart };
            }
            case '>': {
                if (categoryValue.minValue > threshold || categoryValue.maxValue <= threshold) {
                    return { part };
                }

                Part lowerPart = part;
                lowerPart.updateCategory(category, {
                    .minValue = categoryValue.minValue,
                    .maxValue = threshold,
                });

                Part upperPart = part;
                upperPart.updateCategory(category, {
                    .minValue = threshold + 1,
                    .maxValue = categoryValue.maxValue,
                });

                return { lowerPart, upperPart };
            }
            default: {
                cout << "Unknown operator used for splitting: " << op << endl;
                exit(1);
            }
        }

        return {};
    }

    optional<string> evaluate(const Part &part) const {
        if (isTerminal) {
            return result;
        }

        const auto categoryValue = _getCategoryValue(part);

        switch (op) {
            case '>': {
                if (categoryValue.minValue > threshold) {
                    return result;
                }

                return nullopt;
            }
            case '<': {
                if (categoryValue.maxValue < threshold) {
                    return result;
                }

                return nullopt;
            }
            default: {
                cout << "Unknown operator for evaluation: " << op << endl;
                exit(1);
            }
        }

        return nullopt;
    }

private:
    Category _getCategoryValue(const Part &part) const {
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

        return {0, 0};
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
    if (part.x.minValue == part.x.maxValue &&
        part.m.minValue == part.m.maxValue &&
        part.a.minValue == part.a.maxValue &&
        part.s.minValue == part.s.maxValue) {
        cout << "{x=" << part.x.minValue << ",m=" << part.m.minValue << ",a=" << part.a.minValue << ",s=" << part.s.minValue << "}";
    } else {
        cout << "{x=(" << part.x.minValue << "," << part.x.maxValue << "),";
        cout << "m=(" << part.m.minValue << "," << part.m.maxValue << "),";
        cout << "a=(" << part.a.minValue << "," << part.a.maxValue << "),";
        cout << "s=(" << part.s.minValue << "," << part.s.maxValue << ")" << "}";
    }
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
                .x.minValue = stoull(partMatch[1].str()),
                .x.maxValue = stoull(partMatch[1].str()),

                .m.minValue = stoull(partMatch[2].str()),
                .m.maxValue = stoull(partMatch[2].str()),

                .a.minValue = stoull(partMatch[3].str()),
                .a.maxValue = stoull(partMatch[3].str()),

                .s.minValue = stoull(partMatch[4].str()),
                .s.maxValue = stoull(partMatch[4].str()),
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

size_t _processPartWithWorkflow(const Part &part, const string &previousEvaluationResult, const vector<Rule> &remainingRules, const Input &input) {
    if (previousEvaluationResult == "A") {
        return part.getCount();
    }

    if (previousEvaluationResult == "R") {
        return 0;
    }

    if (remainingRules.size() == 0) {
        cout << "Found empty remaining rules for workflow evaluation: " << previousEvaluationResult << endl;
        exit(1);
        return 0;
    }

    const auto &rule = remainingRules.front();

    size_t countAccepted = 0;

    const auto splitSegmentParts = rule.split(part);
    for (const auto &segmentPart : splitSegmentParts) {
        const auto evaluateOpt = rule.evaluate(segmentPart);
        if (evaluateOpt) {
            // we were able to successfully evaluate this rule (or it is a terminal rule)
            // move to the result of it (either a new workflow or Approve / Reject result)
            const auto result = *evaluateOpt;
            vector<Rule> newRules;
            if (result != "A" && result != "R") {
                const auto workflowName = result;
                if (!input.workflowToRules.contains(workflowName)) {
                    cout << "Unknown workflow name: " << workflowName << endl;
                    exit(1);
                }

                newRules = input.workflowToRules.at(workflowName);
            }

            countAccepted += _processPartWithWorkflow(segmentPart, result, newRules, input);
        } else {
            // move to the next rule in this workflow
            vector<Rule> nextRemainingRules(remainingRules.begin() + 1, remainingRules.end());
            countAccepted += _processPartWithWorkflow(segmentPart, previousEvaluationResult, nextRemainingRules, input);
        }
    }

    return countAccepted;
}

void part1(const Input &input) {
    size_t totalRating = 0;
    const auto startWorkflowName = "in";
    for (const auto &part : input.parts) {
        if (_processPartWithWorkflow(part, startWorkflowName, input.workflowToRules.at(startWorkflowName), input) == 0) {
            continue;
        }

        totalRating += part.x.minValue + part.m.minValue + part.a.minValue + part.s.minValue;
    }

    cout << totalRating << endl;
}

void part2(const Input &input) {
    const auto combinationsPart = Part {
        .x.minValue = kCategoryMinValue,
        .x.maxValue = kCategoryMaxValue,

        .m.minValue = kCategoryMinValue,
        .m.maxValue = kCategoryMaxValue,

        .a.minValue = kCategoryMinValue,
        .a.maxValue = kCategoryMaxValue,

        .s.minValue = kCategoryMinValue,
        .s.maxValue = kCategoryMaxValue,
    };

    const auto startWorkflowName = "in";
    const auto totalAcceptedCombinationsCount = _processPartWithWorkflow(combinationsPart, startWorkflowName, input.workflowToRules.at(startWorkflowName), input);

    cout << totalAcceptedCombinationsCount << endl;
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