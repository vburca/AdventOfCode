#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

void part1(istream& inputFile)
{
    uint32_t mass;
    uint32_t totalFuel = 0;

    while (!inputFile.eof())
    {
        inputFile >> mass;
        totalFuel += floor(mass / 3) - 2;
    }

    cout << totalFuel << endl;
}

void part2(istream& inputFile)
{
    uint32_t mass;
    uint32_t totalFuel = 0;

    while (!inputFile.eof())
    {
        inputFile >> mass;

        uint32_t fuelToFuel = 0;
        int32_t requiredFuel = floor(mass / 3) - 2;
        while (requiredFuel > 0)
        {
            fuelToFuel += requiredFuel;
            requiredFuel = floor(requiredFuel / 3) - 2;
        }

        totalFuel += fuelToFuel;
    }

    cout << totalFuel << endl;
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