#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include <map>
#include <utility>

using namespace std;

#define DEBUG 0

const regex INPUT_LINE("([0-9]+): (.*)");
const regex TERMINAL_RULE("\"([ab])\"");
const regex LIST_OF_LISTS_OF_RULES("([0-9 ]+) \\| ([0-9 ]+)");

void readInput(istream& inputFile, map<uint64_t, string>& rules, vector<string>& messages)
{
    // read rules
    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        if (line.size() == 0)
        {
            break;
        }

        smatch match;
        if (regex_match(line, match, INPUT_LINE))
        {
            string keyStr = match[1].str();
            string value = match[2].str();

            smatch terminalRuleMatch;
            if (regex_match(value, terminalRuleMatch, TERMINAL_RULE))
            {
                value = terminalRuleMatch[1].str();
            }

            uint64_t key;
            istringstream ss(keyStr);
            ss >> key;

            rules.emplace(key, value);
        }
    }

    // read messages
    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        messages.emplace_back(line);
    }
}

const string cleanRuleList(const string ruleListStr)
{
    string ruleStr;
    vector<string> ruleList;

    istringstream ss(ruleListStr);
    while (getline(ss, ruleStr, ' '))
    {
        ruleList.push_back(ruleStr);
    }

    ostringstream rulesList1;
    for (size_t i = 0; i < ruleList.size(); ++i)
    {
        rulesList1 << ruleList[i];
        if (i != ruleList.size() - 1)
        {
            rulesList1 << " ";
        }
    }

    return rulesList1.str();
}

bool matchesRule(string rule, string message, map<uint64_t, string>& rules, map<pair<string, string>, bool>& cachedResults)
{
    if (DEBUG)
    {
        cout << endl << "checking (" << rule << "," << message << ")" << endl;
    }

    const auto cacheKey = pair(rule, message);
    if (cachedResults.find(cacheKey) != cachedResults.end())
    {
        return cachedResults[cacheKey];
    }

    // base cases
    if (message.size() == 0)
    {
        return false;
    }

    // it is a single rule
    if (rule.find(' ') == string::npos)
    {
        const auto ch = rule[0];

        if (ch == 'a' || ch == 'b')
        {
            // it is a terminal rule

            bool res = message.size() == 1 && message[0] == ch;
            return res;
        }
        else
        {
            // it is a recursive rule
            uint64_t nextRuleId;
            istringstream ss(rule);
            ss >> nextRuleId;

            return matchesRule(rules[nextRuleId], message, rules, cachedResults);
        }
    }

    if (rule.find('|') == string::npos)
    {
        // cout << "list of rules (" << rule << ")" << endl;

        // it is a single list of rules

        // grab the rules from the list
        vector<string> rulesIds;

        istringstream ruleListSs(rule);
        string ruleStr;

        while (getline(ruleListSs, ruleStr, ' '))
        {
            rulesIds.emplace_back(ruleStr);
        }

        ostringstream restOfRulesSs;
        for (size_t i = 1; i < rulesIds.size(); ++i)
        {
            restOfRulesSs << rulesIds[i];
            if (i != rulesIds.size() - 1)
            {
                restOfRulesSs << " ";
            }
        }

        // cout << "found number of rules in list= " << rulesIds.size() << endl;

        // try to find a message split that would satisfy this list of rules
        bool matchesSplit = false;
        for (size_t splitIdx = 1; splitIdx < message.size() && !matchesSplit; ++splitIdx)
        {
            // check that the substring up to the splitIdx matches the first rule, recursively check the remainding
            // of the message against the rest of the rules
            matchesSplit = matchesSplit ||
                            (matchesRule(rulesIds[0], message.substr(0, splitIdx), rules, cachedResults) &&
                            matchesRule(restOfRulesSs.str(), message.substr(splitIdx, message.size() - splitIdx), rules, cachedResults));
        }

        cachedResults[cacheKey] = matchesSplit;

        return matchesSplit;
    }
    else
    {
        // it is a list of lists of rules
        smatch match;
        if (regex_match(rule, match, LIST_OF_LISTS_OF_RULES))
        {
            const auto ruleList1 = cleanRuleList(match[1]);
            const auto ruleList2 = cleanRuleList(match[2]);

            bool listsResult = matchesRule(ruleList1, message, rules, cachedResults) ||
                                matchesRule(ruleList2, message, rules, cachedResults);

            cachedResults[cacheKey] = listsResult;
            return listsResult;
        }
        else
        {
            cout << "Rule " << rule << " did not match list of lists regex!" << endl;
            exit(1);
        }
    }

    // cout << "why we here? rule=" << rule << endl;
    return false;
}

void part1(map<uint64_t, string>& rules, vector<string>& messages)
{
    size_t matchingCount = 0;

    map<pair<string, string>, bool> cachedResults;

    for (const auto m : messages)
    {
        matchingCount += matchesRule(rules[0], m, rules, cachedResults);
    }

    cout << matchingCount << endl;
}

void part2(map<uint64_t, string>& rules, vector<string>& messages)
{
    // replacing rules
    // 8: 42 | 42 8
    // 11: 42 31 | 42 11 31
    rules[8] = "42 | 42 8";
    rules[11] = "42 31 | 42 11 31";

    size_t matchingCount = 0;

    map<pair<string, string>, bool> cachedResults;

    for (const auto m : messages)
    {
        matchingCount += matchesRule(rules[0], m, rules, cachedResults);
    }

    cout << matchingCount << endl;}


int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        cout << "Must specify input file!" << endl;
        return 0;
    }

    cout << "Input file name: " << argv[1] << endl;
    ifstream inputFile(argv[1]);

    map<uint64_t, string> rules;
    vector<string> messages;

    readInput(inputFile, rules, messages);

    // DEBUG
    if (DEBUG)
    {
        cout << "Rules" << endl;
        for (const auto [key, value] : rules)
        {
            cout << key << " ::: " << value << endl;
        }
        cout << "Messages" << endl;
        for (const auto m : messages)
        {
            cout << m << endl;
        }
    }

    // cout << "Part 1: " << endl;
    // part1(rules, messages);

    cout << "Part 2: " << endl;
    part2(rules, messages);

    inputFile.close();

    return 0;
}