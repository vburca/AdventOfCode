#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <vector>
#include <unordered_map>
#include <optional>
#include <cstdint>

using namespace std;

#define TARGET_EPOCH_PART1 2020
#define TARGET_EPOCH_PART2 30000000

void part1(istream& inputFile)
{
    vector<optional<pair<size_t, size_t>>> numsToAge(TARGET_EPOCH_PART1, nullopt);
    size_t epoch = 1;
    uint32_t lastNum = 0;
    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        istringstream ss(line);

        string numStr;
        while (getline(ss, numStr, ','))
        {
            uint32_t num;
            istringstream numSs(numStr);
            numSs >> num;

            numsToAge[num] = pair(epoch, epoch);
            epoch += 1;
            lastNum = num;
        }
    }

    uint32_t currentNum = 0;
    while (epoch <= TARGET_EPOCH_PART1)
    {
        if (!numsToAge[lastNum].has_value())
        {
            // this is the first time the last number was said so we need to say 0
            currentNum = 0;
        }
        else
        {
            // the previous number was previously said, so say its age
            currentNum = numsToAge[lastNum].value().second - numsToAge[lastNum].value().first;
        }

        // add the current number and calculate its age
        if (!numsToAge[currentNum].has_value())
        {
            numsToAge[currentNum] = pair(epoch, epoch);
        }
        else
        {
            auto [first, second] = numsToAge[currentNum].value();
            numsToAge[currentNum] = pair(second, epoch);
        }

        lastNum = currentNum;
        epoch += 1;
    }

    cout << lastNum << endl;
}

void part2(istream& inputFile)
{
    vector<optional<pair<size_t, size_t>>> numsToAge(TARGET_EPOCH_PART2, nullopt);
    size_t epoch = 1;
    uint32_t lastNum = 0;
    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        istringstream ss(line);

        string numStr;
        while (getline(ss, numStr, ','))
        {
            uint32_t num;
            istringstream numSs(numStr);
            numSs >> num;

            numsToAge[num] = pair(epoch, epoch);
            epoch += 1;
            lastNum = num;
        }
    }

    uint32_t currentNum = 0;
    while (epoch <= TARGET_EPOCH_PART2)
    {
        if (!numsToAge[lastNum].has_value())
        {
            // this is the first time the last number was said so we need to say 0
            currentNum = 0;
        }
        else
        {
            // the previous number was previously said, so say its age
            currentNum = numsToAge[lastNum].value().second - numsToAge[lastNum].value().first;
        }

        // add the current number and calculate its age
        if (!numsToAge[currentNum].has_value())
        {
            numsToAge[currentNum] = pair(epoch, epoch);
        }
        else
        {
            auto [first, second] = numsToAge[currentNum].value();
            numsToAge[currentNum] = pair(second, epoch);
        }

        lastNum = currentNum;
        epoch += 1;
    }

    cout << lastNum << endl;
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