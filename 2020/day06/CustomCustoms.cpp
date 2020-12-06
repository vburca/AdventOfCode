#include <iostream>
#include <fstream>
#include <string>
#include <numeric>
#include <unordered_set>
#include <unordered_map>

using namespace std;

void part1(istream& inputFile)
{
    size_t totalAnswers = 0;

    unordered_set<char> answers;
    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        if (line.size() == 0)
        {
            totalAnswers += answers.size();
            answers.clear();
        }
        else
        {
            for (auto ch : line)
            {
                answers.insert(ch);
            }
        }
    }

    totalAnswers += answers.size();

    cout << totalAnswers << endl;
}

void part2(istream& inputFile)
{
    size_t totalAnswers = 0;

    unordered_map<char, size_t> answers;
    size_t groupSize = 0;

    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        if (line.size() == 0)
        {
            totalAnswers += accumulate(
                answers.begin(),
                answers.end(),
                0,
                [groupSize](const auto& accum, const auto& elem)
                {
                    if (elem.second == groupSize)
                    {
                        return accum + 1;
                    }

                    return accum;
                });

            answers.clear();
            groupSize = 0;
        }
        else
        {
            groupSize += 1;

            for (auto ch : line)
            {
                if (answers.find(ch) == answers.end())
                {
                    answers[ch] = 0;
                }

                answers[ch] += 1;
            }
        }
    }

    totalAnswers += accumulate(
        answers.begin(),
        answers.end(),
        0,
        [groupSize](const auto& accum, const auto& elem)
        {
            if (elem.second == groupSize)
            {
                return accum + 1;
            }

            return accum;
        });

    cout << totalAnswers << endl;
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