#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>

using namespace std;

#define MAX_DAYS_PART1 80
#define MAX_DAYS_PART2 256
#define NEW_CHILD_TIMER_VALUE 8
#define NEW_CHILD_INTERVAL 7

void print(const vector<size_t>& states, const size_t maxPrintCount = -1)
{
    size_t count = 0;
    for (const auto& state : states)
    {
        cout << state << " ";
        count += 1;

        if (count >= maxPrintCount)
        {
            break;
        }
    }
    cout << endl;
}

size_t getNumAncestors(const size_t fishTimer, const size_t kMaxDays, unordered_map<size_t, size_t>& timerToAncestors)
{
    // if we cached the result for this timer, just return it and we are done
    const auto& it = timerToAncestors.find(fishTimer);
    if (it != timerToAncestors.end())
    {
        return it->second;
    }

    // terminal cases
    if (fishTimer >= kMaxDays)
    {
        // can not produce any children in the timeframe
        return 0;
    }

    // we didn't find the ancestor count for the current fish timer, and the current fish can produce at least 1 ancestor
    // so we need to calculate how many and update the cache...

    // current fish will generate at least 1 (since fishTimer + 1 < kMaxDays) + the amount of times the remainder is
    // divisible by NEW_CHILD_INTERVAL
    // we subtract the -1 because it will take an additional day to have the child born, after the fishTimer gets to 0
    const size_t childrenFishTimers = 1 + (kMaxDays - fishTimer - 1) / NEW_CHILD_INTERVAL;

    size_t numAncestors = childrenFishTimers; // we know how many immediate children fish this fish is going to generate
                                              // so add them to the total from now

    // precalculate the child timer (from present to when it will be born) and add them to the children vector
    size_t timerOffset = fishTimer + 1; // we start when the first child would be born
    size_t generatedChild = 0;
    while (generatedChild < childrenFishTimers)
    {
        // we compute the child timer (from present to when it will be born) the following way: a + 1 + b, where:
        // a  = the time offset (in days) when the current fish (parent) would reach 0
        // +1 = the next day after the offset, i.e. when the child actually gets born
        // b  = the actual timer the child will start with when it will be born
        const size_t childTimer = timerOffset + NEW_CHILD_TIMER_VALUE;

        // update the timer offset with the number of days it will take the current fish to create a new fish
        timerOffset += NEW_CHILD_INTERVAL;

        // recursively calculate the number of ancestors for this child
        numAncestors += getNumAncestors(childTimer, kMaxDays, timerToAncestors);

        generatedChild += 1;
    }

    // update the cache
    timerToAncestors.insert({fishTimer, numAncestors});

    return numAncestors;
}

void part1(istream& inputFile)
{
    constexpr size_t kMaxDays = MAX_DAYS_PART1;

    vector<size_t> fishTimers;
    unordered_map<size_t, size_t> timerToAncestors;

    while (!inputFile.eof())
    {
        string numStr;
        getline(inputFile, numStr, ',');

        fishTimers.push_back(stoi(numStr));
    }

    size_t totalFish = fishTimers.size(); // start with initial population
    for (const auto& fishTimer : fishTimers)
    {
        totalFish += getNumAncestors(fishTimer, kMaxDays, timerToAncestors);
    }

    cout << totalFish << endl;
}

void part2(istream& inputFile)
{
    constexpr size_t kMaxDays = MAX_DAYS_PART2;

    vector<size_t> fishTimers;
    unordered_map<size_t, size_t> timerToAncestors;

    while (!inputFile.eof())
    {
        string numStr;
        getline(inputFile, numStr, ',');

        fishTimers.push_back(stoi(numStr));
    }

    size_t totalFish = fishTimers.size(); // start with initial population
    for (const auto& fishTimer : fishTimers)
    {
        totalFish += getNumAncestors(fishTimer, kMaxDays, timerToAncestors);
    }

    cout << totalFish << endl;}

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