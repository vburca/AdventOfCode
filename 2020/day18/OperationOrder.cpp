#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stack>
#include <utility>
#include <cstdint>

using namespace std;

uint64_t apply(const char op, const uint64_t a, const uint64_t b)
{
    switch (op)
    {
        case '+': return a + b; break;
        case '*': return a * b; break;
        default:
            cout << "Unknown operator: " << op << endl;
    }

    return 0;
}

void part1(istream& inputFile)
{
    uint64_t result = 0;
    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        stack<uint64_t> intermediaryResults;
        stack<char> intermediaryOperators;

        size_t i = 0;
        while (i < line.size())
        {
            auto ch = line[i];

            if (ch == ' ')
            {
                i += 1;

                continue;
            }

            if (ch == '+' || ch == '*' || ch == '(')
            {
                i += 1;

                intermediaryOperators.emplace(ch);
            }
            else if (ch == ')')
            {
                i += 1;

                if (intermediaryOperators.top() != '(')
                {
                    cout << "( was not found when closing )" << endl;
                    exit(1);
                }
                else
                {
                    // pop the open parenthesis
                    intermediaryOperators.pop();

                    if (!intermediaryOperators.empty() && intermediaryOperators.top() != '(')
                    {
                        const auto res1 = intermediaryResults.top();
                        intermediaryResults.pop();
                        const auto res2 = intermediaryResults.top();
                        intermediaryResults.pop();
                        const auto op = intermediaryOperators.top();
                        intermediaryOperators.pop();

                        const auto res = apply(op, res2, res1);

                        // cout << res2 << " " << op << " " << res1 << " = " << res << endl;

                        intermediaryResults.emplace(res);
                    }
                }
            }
            else
            {
                // construct the number
                ostringstream ss;
                while (i < line.size() && ch != '+' && ch != '*' && ch != '(' && ch != ')')
                {
                    ss << ch;
                    i += 1;
                    ch = line[i];
                }

                string element = ss.str();

                // we are reading a number
                uint64_t num;
                istringstream numSs(element);
                numSs >> num;

                if (intermediaryOperators.empty() || intermediaryOperators.top() == '(')
                {
                    // we just previously read a paranthesis, so start a new intermediary result
                    intermediaryResults.emplace(num);
                }
                else
                {
                    // we didn't just previously read a paranthesis, we can accumulate the result so far
                    const auto res = intermediaryResults.top();
                    intermediaryResults.pop();
                    const auto op = intermediaryOperators.top();
                    intermediaryOperators.pop();

                    const auto newRes = apply(op, res, num);

                    // cout << res << " " << op << " " << num << " = " << newRes << endl;

                    intermediaryResults.emplace(newRes);
                }
            }
        }

        const auto res = intermediaryResults.top();
        intermediaryResults.pop();

        // cout << res << endl;

        result += res;
    }

    cout << result << endl;
}

void part2(istream& inputFile)
{
    uint64_t result = 0;
    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        stack<uint64_t> intermediaryResults;
        stack<char> intermediaryOperators;

        size_t i = 0;
        while (i < line.size())
        {
            auto ch = line[i];

            if (ch == ' ')
            {
                i += 1;

                continue;
            }

            if (ch == '+' || ch == '*' || ch == '(')
            {
                i += 1;

                intermediaryOperators.emplace(ch);
            }
            else if (ch == ')')
            {
                i += 1;

                while (intermediaryOperators.top() != '(')
                {
                    const auto res1 = intermediaryResults.top();
                    intermediaryResults.pop();
                    const auto res2 = intermediaryResults.top();
                    intermediaryResults.pop();
                    const auto op = intermediaryOperators.top();
                    intermediaryOperators.pop();

                    const auto res = apply(op, res2, res1);

                    // cout << res2 << " " << op << " " << res1 << " = " << res << endl;

                    intermediaryResults.emplace(res);
                }

                // pop the open parenthesis
                intermediaryOperators.pop();

                if (!intermediaryOperators.empty() && intermediaryOperators.top() == '+')
                {
                    const auto res1 = intermediaryResults.top();
                    intermediaryResults.pop();
                    const auto res2 = intermediaryResults.top();
                    intermediaryResults.pop();
                    const auto op = intermediaryOperators.top();
                    intermediaryOperators.pop();

                    const auto res = apply(op, res2, res1);

                    // cout << res2 << " " << op << " " << res1 << " = " << res << endl;

                    intermediaryResults.emplace(res);
                }
            }
            else
            {
                // construct the number
                ostringstream ss;
                while (i < line.size() && ch != '+' && ch != '*' && ch != '(' && ch != ')')
                {
                    ss << ch;
                    i += 1;
                    ch = line[i];
                }

                string element = ss.str();

                // we are reading a number
                uint64_t num;
                istringstream numSs(element);
                numSs >> num;

                if (intermediaryOperators.empty() ||
                    intermediaryOperators.top() == '(' ||
                    intermediaryOperators.top() == '*')
                {
                    // we just previously read a paranthesis, so start a new intermediary result
                    intermediaryResults.emplace(num);
                }
                else if (intermediaryOperators.top() == '+')
                {
                    // we didn't just previously read a paranthesis, we can accumulate the result so far
                    const auto res = intermediaryResults.top();
                    intermediaryResults.pop();
                    const auto op = intermediaryOperators.top();
                    intermediaryOperators.pop();

                    const auto newRes = apply(op, res, num);

                    // cout << res << " " << op << " " << num << " = " << newRes << endl;

                    intermediaryResults.emplace(newRes);
                }
            }
        }

        while (!intermediaryOperators.empty())
        {
            const auto res1 = intermediaryResults.top();
            intermediaryResults.pop();
            const auto res2 = intermediaryResults.top();
            intermediaryResults.pop();
            const auto op = intermediaryOperators.top();
            intermediaryOperators.pop();

            const auto res = apply(op, res2, res1);

            // cout << res2 << " " << op << " " << res1 << " = " << res << endl;

            intermediaryResults.emplace(res);
        }

        const auto res = intermediaryResults.top();
        intermediaryResults.pop();

        // cout << res << endl;

        result += res;
    }

    cout << result << endl;
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