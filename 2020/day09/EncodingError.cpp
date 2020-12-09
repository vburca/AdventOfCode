#include <iostream>
#include <fstream>
#include <deque>
#include <set>
#include <vector>
#include <utility>

using namespace std;

const size_t WINDOW_SIZE = 25;

int64_t BAD_NUMBER;

void part1(istream& inputFile)
{
    int64_t badNum;
    bool foundBadNum = false;

    deque<int64_t> nums;
    while (!inputFile.eof() && !foundBadNum)
    {
        int64_t n;
        inputFile >> n;

        if (nums.size() < WINDOW_SIZE)
        {
            nums.push_back(n);
        }
        else
        {
            // assume this is the bad number
            foundBadNum = true;
            for (const auto& a : nums)
            {
                const auto diff = n - a;

                if (diff == a)
                {
                    continue;
                }

                for (const auto& b : nums)
                {
                    if (b == diff)
                    {
                        foundBadNum = false;
                        break;
                    }
                }

                if (!foundBadNum)
                {
                    break;
                }
            }

            if (foundBadNum)
            {
                badNum = n;
                BAD_NUMBER = badNum;
            }

            nums.pop_front();
            nums.push_back(n);
        }

    }

    cout << badNum << endl;
}

void part2(istream& inputFile)
{
    int64_t weaknessSum = 0;

    deque<int64_t> sums;
    deque<pair<int64_t, int64_t>> minMax;  // list of (min, max) for each of the sums

    bool foundWeakness = false;
    while (!inputFile.eof() && !foundWeakness)
    {
        int64_t n;
        inputFile >> n;

        for (size_t i = 0; i < sums.size(); ++i)
        {
            sums[i] += n;

            auto& mm = minMax[i];

            if (n < mm.first)
            {
                mm.first = n;
            }
            else if (n > mm.second)
            {
                mm.second = n;
            }
        }

        sums.push_back(n);
        minMax.emplace_back(n, n);

        while (sums.front() > BAD_NUMBER)
        {
            sums.pop_front();
            minMax.pop_front();
        }

        auto& mm = minMax.front();
        if (sums.front() == BAD_NUMBER && (mm.first != mm.second))
        {
            foundWeakness = true;
            weaknessSum = mm.first + mm.second;
        }
    }

    cout << weaknessSum << endl;
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