#include <iostream>
#include <fstream>
#include <cmath>
#include <unordered_map>
#include <vector>
#include <algorithm>

using namespace std;

const int32_t VAL = 2020;

void part1(istream& inputFile)
{
    unordered_map<int32_t, bool> nums;
    int32_t num;
    bool found = false;
    int32_t a, b;

    while (!found && !inputFile.eof())
    {
        inputFile >> num;

        if (num <= VAL)
        {
            auto diff = VAL - num;
            if (nums.find(diff) != nums.end())
            {
                a = num;
                b = diff;
                found = true;
            }

            nums[num] = true;
        }
    }

    if (found)
    {
        auto prod = a * b;
        cout << prod << endl;
    }
    else
    {
        cout << "Did not find any pair of ints that sum up to " << VAL << endl;
    }
}

void part2(istream& inputFile)
{
    vector<int32_t> nums;
    int32_t num;

    while (!inputFile.eof())
    {
        inputFile >> num;
        nums.push_back(num);
    }

    sort(nums.begin(), nums.end());

    bool found = false;
    int32_t a, b, c;
    for (size_t i = 0; i < nums.size() - 1 && !found; ++i)
    {
        a = nums[i];

        auto start = i + 1;
        auto end = nums.size() - 1;

        while (!found && (start != end))
        {
            b = nums[start];
            c = nums[end];

            if (a + b + c < VAL)
            {
                start += 1;
            }
            else if (a + b + c > VAL)
            {
                end -= 1;
            }
            else
            {
                found = true;
            }
        }
    }

    if (found)
    {
        auto prod = a * b * c;
        cout << prod << endl;
    }
    else
    {
        cout << "Did not find any triplet of ints that sum up to " << VAL << endl;
    }
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