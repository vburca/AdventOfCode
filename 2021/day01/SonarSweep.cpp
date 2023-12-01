#include <iostream>
#include <fstream>
#include <cmath>
#include <deque>
#include <numeric>
#include <cstdint>

using namespace std;


void part1(istream& inputFile)
{
    int32_t prev, curr;
    bool first = true;
    size_t increases = 0;

    while (!inputFile.eof())
    {
        inputFile >> curr;

        if (!first)
        {
            increases += (curr > prev) ? 1 : 0;
        }

        prev = curr;
        first = false;
    }

    cout << increases << endl;
}

void part2(istream& inputFile)
{
    int32_t num;
    deque<int32_t> window;
    size_t maxWindowSize = 3;
    size_t increases = 0;

    while (!inputFile.eof())
    {
        if (window.size() == maxWindowSize)
        {
            const auto prevSum = accumulate(window.begin(), window.end(), 0);

            inputFile >> num;
            window.pop_front();
            window.push_back(num);

            const auto currSum = accumulate(window.begin(), window.end(), 0);

            increases += (currSum > prevSum) ? 1 : 0;
        }
        else
        {
            inputFile >> num;
            window.push_back(num);
        }
    }

    cout << increases << endl;
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