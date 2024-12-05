#include "../../util/BasicIncludes.h"
#include "../../util/StringUtil.h"

#include <set>
#include <map>
#include <regex>

using namespace std;

const regex ORDERING_RX("([0-9]+)\\|([0-9]+)");

void _printRules(const map<size_t, set<size_t>> &rules) {
    for (const auto &rule : rules) {
        cout << rule.first << ": { ";
        for (const auto &pageNumber : rule.second) {
            cout << pageNumber << " ";
        }
        cout << "}" <<endl;
    }
}

void _printUpdates(const vector<vector<size_t>> &updates) {
    for (const auto &update : updates) {
        cout << "[ ";
        for (const auto &pageNumber : update) {
            cout << pageNumber << " ";
        }
        cout << "]" << endl;
    }
}

void part1(istream& inputFile) {
    map<size_t, set<size_t>> orderingRules;
    vector<vector<size_t>> updates;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        if (line.empty()) {
            continue;
        }

        smatch match;
        if (regex_match(line, match, ORDERING_RX)) {
            const size_t numA = stoul(match[1].str());
            const size_t numB = stoul(match[2].str());

            orderingRules[numA].insert(numB);
        } else {
            vector<size_t> update;

            istringstream iss(line);
            size_t pageNumber;
            while (iss >> pageNumber) {
                if (iss.peek() == ',') {
                    iss.ignore();
                }
                update.push_back(pageNumber);
            }

            updates.push_back(update);
        }
    }

    // _printRules(orderingRules);
    // _printUpdates(updates);

    size_t midPageNumberSum = 0;
    for (const auto &update : updates) {
        bool isValid = true;
        for (size_t i = 0; i < update.size() - 1 && isValid; ++i) {
            for (size_t j = i + 1; j < update.size() && isValid; ++j) {
                const auto numA = update[i];
                const auto numB = update[j];

                if (orderingRules[numB].contains(numA)) {
                    // ordering is reversed, we are not good, stop
                    isValid = false;
                    break;
                }
            }
        }

        if (isValid) {
            midPageNumberSum += update[update.size() / 2];
        }
    }

    cout << midPageNumberSum << endl;
}

const pair<bool, vector<size_t>> _fixOrdering(const vector<size_t> &update, map<size_t, set<size_t>> &orderingRules) {
    vector<size_t> orderedUpdate = update;

    bool fixedOrder = false;
    for (size_t i = 0; i < orderedUpdate.size() - 1; ++i) {
        bool updateDone = true;
        while (updateDone) {
            updateDone = false;
            for (size_t j = i + 1; j < orderedUpdate.size(); ++j) {
                const size_t numA = orderedUpdate[i];
                const size_t numB = orderedUpdate[j];

                if (orderingRules[numB].contains(numA)) {
                    // ordering is reversed, we are not good, reorder
                    orderedUpdate[i] = numB;
                    orderedUpdate[j] = numA;

                    updateDone = true;
                    fixedOrder = true;
                }
            }
        }
    }

    return make_pair(fixedOrder, orderedUpdate);
}

void part2(istream& inputFile) {
    map<size_t, set<size_t>> orderingRules;
    vector<vector<size_t>> updates;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        if (line.empty()) {
            continue;
        }

        smatch match;
        if (regex_match(line, match, ORDERING_RX)) {
            const size_t numA = stoul(match[1].str());
            const size_t numB = stoul(match[2].str());

            orderingRules[numA].insert(numB);
        } else {
            vector<size_t> update;

            istringstream iss(line);
            size_t pageNumber;
            while (iss >> pageNumber) {
                if (iss.peek() == ',') {
                    iss.ignore();
                }
                update.push_back(pageNumber);
            }

            updates.push_back(update);
        }
    }

    // _printRules(orderingRules);
    // _printUpdates(updates);

    size_t midPageNumberSum = 0;
    for (const auto &update : updates) {
        const auto res = _fixOrdering(update, orderingRules);

        if (res.first) {
            midPageNumberSum += res.second[res.second.size() / 2];
        }
    }

    cout << midPageNumberSum << endl;
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