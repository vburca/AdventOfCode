#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <numeric>
#include <cmath>

using namespace std;

void part1(istream& inputFile)
{
    vector<size_t> positions;

    while (!inputFile.eof())
    {
        string numStr;
        getline(inputFile, numStr, ',');

        positions.push_back(stoi(numStr));
    }

    sort(positions.begin(), positions.end());

    const size_t midPoint = positions.size() / 2;
    const size_t median = positions[midPoint];

    size_t totalFuelCost = 0;
    for (const auto& pos : positions)
    {
        totalFuelCost += (pos > median) ? pos - median : median - pos;
    }

    cout << totalFuelCost << endl;
}

void part2(istream& inputFile)
{
    vector<size_t> positions;

    while (!inputFile.eof())
    {
        string numStr;
        getline(inputFile, numStr, ',');

        positions.push_back(stoi(numStr));
    }

    const float averageF = accumulate(positions.begin(), positions.end(), 0.f) / positions.size();
    const size_t average = floor(averageF + 0.5f);

    // we are trying to minimize a function and find the best position based on that; check the notes attached to this
    // solution (pngs) - after minimizing, we are left to checking only 3 possible values for the best position, i.e.
    // {average - 1, average, average + 1}
    // we then calculate the total distance for each of these averages and take the smallest of them

    const auto average1 = average - 1;
    const auto average2 = average;
    const auto average3 = average + 1;

    size_t totalFuelCost1 = 0; // for avg - 1
    size_t totalFuelCost2 = 0; // for avg
    size_t totalFuelCost3 = 0; // for avt + 1
    for (const auto& pos : positions)
    {
        const size_t delta1 = (pos > average1) ? pos - average1 : average1 - pos;
        const size_t delta2 = (pos > average2) ? pos - average2 : average2 - pos;
        const size_t delta3 = (pos > average3) ? pos - average3 : average3 - pos;

        totalFuelCost1 += delta1 * (delta1 + 1) / 2;
        totalFuelCost2 += delta2 * (delta2 + 1) / 2;
        totalFuelCost3 += delta3 * (delta3 + 1) / 2;
    }

    const auto totalFuelCost = min(min(totalFuelCost1, totalFuelCost2), totalFuelCost3);
    cout << totalFuelCost << endl;
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