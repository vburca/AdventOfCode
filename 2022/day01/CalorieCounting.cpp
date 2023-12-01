#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <sstream>
#include <cstdint>

using namespace std;


void part1(istream& inputFile)
{
    priority_queue<uint64_t> totalCalories;

    uint64_t totalCaloriesForCurrentElf = 0;
    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        if (line.length() == 0) {
            // store the total so far in the heap
            totalCalories.push(totalCaloriesForCurrentElf);

            // reset the total
            totalCaloriesForCurrentElf = 0;
        }
        else {
            // parse the calories
            uint64_t foodCalories;
            istringstream iss(line);
            iss >> foodCalories;

            // update total so far
            totalCaloriesForCurrentElf += foodCalories;
        }
    }

    // push last count
    totalCalories.push(totalCaloriesForCurrentElf);

    cout << totalCalories.top() << endl;
}

void part2(istream& inputFile)
{
    priority_queue<uint64_t> totalCalories;

    uint64_t totalCaloriesForCurrentElf = 0;
    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        if (line.length() == 0) {
            // store the total so far in the heap
            totalCalories.push(totalCaloriesForCurrentElf);

            // reset the total
            totalCaloriesForCurrentElf = 0;
        }
        else {
            // parse the calories
            uint64_t foodCalories;
            istringstream iss(line);
            iss >> foodCalories;

            // update total so far
            totalCaloriesForCurrentElf += foodCalories;
        }
    }

    // push last count
    totalCalories.push(totalCaloriesForCurrentElf);

    const auto numElves = 3;
    const auto initialSize = totalCalories.size();
    const auto targetSize = initialSize - numElves;
    uint64_t topCaloriesSum = 0;

    while (totalCalories.size() > targetSize) {
        topCaloriesSum += totalCalories.top();
        totalCalories.pop();
    }

    cout << topCaloriesSum << endl;
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