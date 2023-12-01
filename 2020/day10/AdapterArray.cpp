#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <cstdint>

using namespace std;

void part1(istream& inputFile)
{
    vector<int32_t> adapters;
    size_t diff_1 = 0;
    size_t diff_3 = 0;

    while (!inputFile.eof())
    {
        int32_t joltage;
        inputFile >> joltage;

        adapters.push_back(joltage);
    }

    adapters.push_back(0);  // add the outlet
    sort(adapters.begin(), adapters.end());
    adapters.push_back(adapters.back() + 3);  // add device adapter

    size_t n = adapters.size();

    for (size_t i = 0; i < n - 1; ++i)
    {
        if (adapters[i + 1] - adapters[i] == 1)
        {
            diff_1 += 1;
        }
        else if (adapters[i + 1] - adapters[i] == 3)
        {
            diff_3 += 1;
        }
        else
        {
            cout << "Found joltage difference: " << adapters[i + 1] - adapters[i]
                    << " on adapters ( " << adapters[i] << ", " << adapters[i + 1] << " )" << endl;
        }
    }

    cout << diff_1 * diff_3 << endl;
}

void part2(istream& inputFile)
{
    vector<int32_t> adapters;
    size_t diff_1 = 0;
    size_t diff_3 = 0;

    while (!inputFile.eof())
    {
        int32_t joltage;
        inputFile >> joltage;

        adapters.push_back(joltage);
    }

    adapters.push_back(0);  // add the outlet
    sort(adapters.begin(), adapters.end());
    adapters.push_back(adapters.back() + 3);  // add device adapter

    size_t n = adapters.size();

    vector<size_t> counts;
    counts.push_back(1);

    for (size_t i = 1; i < n; ++i)
    {
        size_t count = 0;
        if (i > 2)
        {
            count = counts[i - 1] * ((adapters[i] - adapters[i - 1]) <= 3)
                    + counts[i - 2] * ((adapters[i] - adapters[i - 2]) <= 3)
                    + counts[i - 3] * ((adapters[i] - adapters[i - 3]) <= 3);
        }
        else if (i > 1)
        {
            count = counts[i - 1] * ((adapters[i] - adapters[i - 1]) <= 3)
                    + counts[i - 2] * ((adapters[i] - adapters[i - 2]) <= 3);
        }
        else if (i > 0)
        {
            count = counts[i - 1] * ((adapters[i] - adapters[i - 1]) <= 3);
        }

        counts.push_back(count);
    }

    cout << counts.back() << endl;
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