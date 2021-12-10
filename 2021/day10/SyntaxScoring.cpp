#include <iostream>
#include <fstream>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>

using namespace std;

const unordered_map<char, char> openClosedMapping = {
    {'(', ')'},
    {'[', ']'},
    {'{', '}'},
    {'<', '>'}
};

const unordered_map<char, size_t> errorScoreTable = {
    {')', 3},
    {']', 57},
    {'}', 1197},
    {'>', 25137}
};

#define COMPLETION_SCORE_MULTIPLIER 5
const unordered_map<char, size_t> completionScoreTable = {
    {')', 1},
    {']', 2},
    {'}', 3},
    {'>', 4}
};

const unordered_set<char> closed = {')', ']', '}', '>'};

void part1(istream& inputFile)
{
    size_t errorScore = 0;

    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        stack<char> s;
        for (const auto& ch : line)
        {
            if (closed.find(ch) == closed.end())
            {
                // open parenthesis, add it
                s.push(ch);
                continue;
            }
            else if (s.empty())
            {
                cout << "Found " << ch << " but stack is empty!" << endl;
                break;
            }

            // closed parenthesis, see if it matches the open one
            const auto& expectedClosedIt = openClosedMapping.find(s.top());
            if (expectedClosedIt == openClosedMapping.end())
            {
                cout << "Unknown parenthesis: " << s.top() << endl;
                break;
            }

            const auto expectedClosed = expectedClosedIt->second;
            if (expectedClosed == ch)
            {
                // we are good, continue
                s.pop(); // pop the open parenthesis
                continue;
            }

            // not good - calculate error and continue to next line
            const auto& it = errorScoreTable.find(ch);
            if (it == errorScoreTable.end())
            {
                cout << "Unknown parenthesis: " << ch << endl;
                break;
            }

            errorScore += it->second;
            break; // go to the next line
        }
    }

    cout << errorScore << endl;
}

void part2(istream& inputFile)
{
    vector<size_t> completionScores;

    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        size_t completionScore = 0;

        stack<char> s;
        bool corruptLine = false;
        for (const auto& ch : line)
        {
            if (closed.find(ch) == closed.end())
            {
                // open parenthesis, add it
                s.push(ch);
                continue;
            }
            else if (s.empty())
            {
                cout << "Found " << ch << " but stack is empty!" << endl;
                corruptLine = true;
                break;
            }

            // closed parenthesis, see if it matches the open one
            const auto& expectedClosedIt = openClosedMapping.find(s.top());
            if (expectedClosedIt == openClosedMapping.end())
            {
                cout << "Unknown parenthesis: " << s.top() << endl;
                break;
            }

            const auto expectedClosed = expectedClosedIt->second;
            if (expectedClosed == ch)
            {
                // we are good, continue
                s.pop(); // pop the open parenthesis
                continue;
            }

            // not good - calculate error and continue to next line
            const auto& it = errorScoreTable.find(ch);
            if (it == errorScoreTable.end())
            {
                cout << "Unknown parenthesis: " << ch << endl;
                break;
            }

            corruptLine = true;
            break; // go to the next line
        }

        if (!corruptLine)
        {
            // check if incomplete
            while (!s.empty())
            {
                const auto& expectedClosedIt = openClosedMapping.find(s.top());
                if (expectedClosedIt == openClosedMapping.end())
                {
                    cout << "Unknown parenthesis: " << s.top() << " in Open Closed Mapping" << endl;
                    break;
                }

                const auto& completionScoreIt = completionScoreTable.find(expectedClosedIt->second);
                if (completionScoreIt == completionScoreTable.end())
                {
                    cout << "Unknown parenthesis: " << expectedClosedIt->second << " in Completion Score Table" << endl;
                    break;
                }

                s.pop();
                completionScore = completionScore * COMPLETION_SCORE_MULTIPLIER + completionScoreIt->second;
            }

            if (completionScore > 0)
            {
                completionScores.push_back(completionScore);
            }
        }
    }

    const auto medianId = completionScores.size() / 2;
    nth_element(completionScores.begin(), completionScores.begin() + medianId, completionScores.end());
    cout << completionScores[medianId] << endl;
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