#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <string>
#include <cstdint>

using namespace std;

void binarySearch(size_t& low, size_t& high, bool keepLow)
{
    if (keepLow)
    {
        high = (high + low) / 2; // ( 2 * high - (high - low) ) / 2
    }
    else
    {
        low = (high + low + 1) / 2;
    }
}

void part1(istream& inputFile)
{
    size_t highestSeatId = 0;

    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        string rowStr = line.substr(0, 7);
        string colStr = line.substr(7, 3);

        size_t rowLow = 0;
        size_t rowHigh = 127;

        for (auto ch : rowStr)
        {
            binarySearch(rowLow, rowHigh, ch == 'F');
        }

        size_t colLow = 0;
        size_t colHigh = 7;
        for (auto ch : colStr)
        {
            binarySearch(colLow, colHigh, ch == 'L');
        }

        size_t seatId = rowLow * 8 + colLow;
        highestSeatId = seatId > highestSeatId ? seatId : highestSeatId;
    }

    cout << highestSeatId << endl;
}

void part2(istream& inputFile)
{
    vector<size_t> seatIds;

    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        string rowStr = line.substr(0, 7);
        string colStr = line.substr(7, 3);

        size_t rowLow = 0;
        size_t rowHigh = 127;

        for (auto ch : rowStr)
        {
            binarySearch(rowLow, rowHigh, ch == 'F');
        }

        size_t colLow = 0;
        size_t colHigh = 7;
        for (auto ch : colStr)
        {
            binarySearch(colLow, colHigh, ch == 'L');
        }

        size_t seatId = rowLow * 8 + colLow;
        seatIds.push_back(seatId);
    }

    sort(seatIds.begin(), seatIds.end());

    size_t mySeatId = 0;
    for (size_t i = 0; i < seatIds.size() - 1; ++i)
    {
        if (seatIds[i + 1] - seatIds[i] > 1)
        {
            mySeatId = seatIds[i] + 1;
            break;
        }
    }

    cout << mySeatId << endl;
}

void part1Smart(istream& inputFile)
{
    uint16_t highestSeatId = 0;

    constexpr uint16_t bits[] = { 512, 256, 128, 64, 32, 16, 8, 4, 2, 1 };
    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        uint16_t seatId = transform_reduce(
            line.begin(),
            line.end(),
            begin(bits),
            0,
            plus<uint16_t>(),
            [](auto ch, auto bit) { return (ch == 'B' || ch == 'R') * bit; });

        highestSeatId = highestSeatId < seatId ? seatId : highestSeatId;
    }

    cout << highestSeatId << endl;
}

void part2Smart(istream& inputFile)
{
    vector<uint16_t> seatIds;

    constexpr uint16_t bits[] = { 512, 256, 128, 64, 32, 16, 8, 4, 2, 1 };
    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        uint16_t seatId = transform_reduce(
            line.begin(),
            line.end(),
            begin(bits),
            0,
            plus<uint16_t>(),
            [](const auto& ch, const auto& bit) { return (ch == 'B' || ch == 'R') * bit; });

        seatIds.push_back(seatId);
    }

    sort(seatIds.begin(), seatIds.end());
    auto it = adjacent_find(seatIds.begin(), seatIds.end(), [](const auto& l, const auto& r) { return r == l + 2; });

    uint16_t mySeatId = (*it) + 1;
    cout << mySeatId << endl;
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
    part1Smart(inputFile);

    inputFile.clear();
    inputFile.seekg(0, ios::beg);

    cout << "Part 2: " << endl;
    part2Smart(inputFile);

    inputFile.close();

    return 0;
}