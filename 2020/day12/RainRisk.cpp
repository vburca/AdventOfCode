#include <iostream>
#include <fstream>
#include <algorithm>
#include <math.h>
#include <vector>
#include <utility>

using namespace std;

#define PI 3.14159265

void part1(istream& inputFile)
{
    pair<int32_t, int32_t> position(0, 0);

    const vector<pair<int32_t, int32_t>> headings{ pair(1, 0), pair(0, -1), pair(-1, 0), pair(0, 1) };
    size_t headingIdx = 0;

    while (!inputFile.eof())
    {
        char ch;
        int32_t d;

        inputFile >> ch;
        inputFile >> d;

        switch (ch)
        {
            case 'N':
                position.second += d;
                break;
            case 'S':
                position.second -= d;
                break;
            case 'E':
                position.first += d;
                break;
            case 'W':
                position.first -= d;
                break;
            case 'F':
                position.first += headings[headingIdx].first * d;
                position.second += headings[headingIdx].second * d;
                break;
            case 'L':
                headingIdx = (headingIdx - d / 90) % headings.size();
                break;
            case 'R':
                headingIdx =  (headingIdx + d / 90) % headings.size();
                break;
            default:
                cout << "Unknown command: ( " << ch << ", " << d << " )" << endl;
        }
    }

    const size_t dist = abs(position.first) + abs(position.second);

    cout << dist << endl;
}

void part2(istream& inputFile)
{
    pair<int32_t, int32_t> position(0, 0);
    pair<int32_t, int32_t> waypoint(10, 1);

    while (!inputFile.eof())
    {
        char ch;
        int32_t d;

        inputFile >> ch;
        inputFile >> d;

        switch (ch)
        {
            case 'N':
                waypoint.second += d;
                break;
            case 'S':
                waypoint.second -= d;
                break;
            case 'E':
                waypoint.first += d;
                break;
            case 'W':
                waypoint.first -= d;
                break;
            case 'F':
                position.first += waypoint.first * d;
                position.second += waypoint.second * d;
                break;
            case 'L':
            {
                const int32_t cosd = cos(d * PI / 180);
                const int32_t sind = sin(d * PI / 180);

                waypoint = pair(
                    cosd * waypoint.first - sind * waypoint.second,
                    sind * waypoint.first + cosd * waypoint.second);

                break;
            }
            case 'R':
            {
                const int32_t cosd = cos(d * PI / 180);     // cos(-theta) = cos(theta)
                const int32_t sind = -sin(d * PI / 180);    // sin(-theta) = -sin(theta)

                waypoint = pair(
                    cosd * waypoint.first - sind * waypoint.second,
                    sind * waypoint.first + cosd * waypoint.second);

                break;
            }
            default:
                cout << "Unknown command: ( " << ch << ", " << d << " )" << endl;
        }
    }

    const size_t dist = abs(position.first) + abs(position.second);

    cout << dist << endl;
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