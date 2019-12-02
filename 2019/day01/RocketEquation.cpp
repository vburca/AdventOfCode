#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

void part1(istream& inputFile)
{
    int mass;
    int totalFuel = 0;

    while (!inputFile.eof())
    {
        inputFile >> mass;

        totalFuel += floor(mass / 3) - 2;
    }

    cout << totalFuel << endl;
}

void part2(istream& inputFile)
{
    int mass;
    int totalFuel = 0;

    while (!inputFile.eof())
    {
        inputFile >> mass;

        int fuelToFuel = floor(mass / 3) - 2;
        mass = floor(fuelToFuel / 3) - 2;
        while (mass > 0)
        {
            fuelToFuel += mass;
            mass = floor(mass / 3) - 2;
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