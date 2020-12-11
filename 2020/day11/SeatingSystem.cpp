#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>

using namespace std;

const char EMPTY_SEAT = 'L';
const char FLOOR = '.';
const char OCCUPIED_SEAT = '#';

void printSeatings(const vector<string>& seatings)
{
    cout << endl;
    for (const auto& line : seatings)
    {
        cout << line << endl;
    }
    cout << endl;
}

size_t neighbors(const vector<string>& seatings, const size_t i, const size_t j)
{
    const auto rows = seatings.size();
    const auto cols = seatings.front().size();

    size_t neighbors = 0;

    // top left
    neighbors += (i > 0 && j > 0) ? seatings[i - 1][j - 1] == OCCUPIED_SEAT : 0;
    // top
    neighbors += (i > 0) ? seatings[i - 1][j] == OCCUPIED_SEAT : 0;
    // top right
    neighbors += (i > 0 && j < cols - 1) ? seatings[i - 1][j + 1] == OCCUPIED_SEAT : 0;
    // right
    neighbors += (j < cols - 1) ? seatings[i][j + 1] == OCCUPIED_SEAT : 0;
    // bottom right
    neighbors += (i < rows - 1 && j < cols - 1) ? seatings[i + 1][j + 1] == OCCUPIED_SEAT : 0;
    // bottom
    neighbors += (i < rows - 1) ? seatings[i + 1][j] == OCCUPIED_SEAT : 0;
    // bottom left
    neighbors += (i < rows - 1 && j > 0) ? seatings[i + 1][j - 1] == OCCUPIED_SEAT : 0;
    // left
    neighbors += (j > 0) ? seatings[i][j - 1] == OCCUPIED_SEAT : 0;

    return neighbors;
}

size_t rayNeighbors(const vector<string>& seatings, const size_t i, const size_t j)
{
    const auto rows = seatings.size();
    const auto cols = seatings.front().size();

    size_t neighbors = 0;

    int32_t ii, jj;

    // top left
    ii = i - 1;
    jj = j - 1;
    while (ii >= 0 && jj >= 0)
    {
        if (seatings[ii][jj] == OCCUPIED_SEAT)
        {
            neighbors += 1;
            break;
        }
        else if (seatings[ii][jj] == EMPTY_SEAT)
        {
            break;
        }

        ii -= 1;
        jj -= 1;
    }

    // top
    ii = i - 1;
    jj = j;
    while (ii >= 0)
    {
        if (seatings[ii][jj] == OCCUPIED_SEAT)
        {
            neighbors += 1;
            break;
        }
        else if (seatings[ii][jj] == EMPTY_SEAT)
        {
            break;
        }

        ii -= 1;
    }

    // top right
    ii = i - 1;
    jj = j + 1;
    while (ii >= 0 && jj < cols)
    {
        if (seatings[ii][jj] == OCCUPIED_SEAT)
        {
            neighbors += 1;
            break;
        }
        else if (seatings[ii][jj] == EMPTY_SEAT)
        {
            break;
        }

        ii -= 1;
        jj += 1;
    }

    // right
    ii = i;
    jj = j + 1;
    while (jj < cols)
    {
        if (seatings[ii][jj] == OCCUPIED_SEAT)
        {
            neighbors += 1;
            break;
        }
        else if (seatings[ii][jj] == EMPTY_SEAT)
        {
            break;
        }

        jj += 1;
    }

    // bottom right
    ii = i + 1;
    jj = j + 1;
    while (ii < rows && jj < cols)
    {
        if (seatings[ii][jj] == OCCUPIED_SEAT)
        {
            neighbors += 1;
            break;
        }
        else if (seatings[ii][jj] == EMPTY_SEAT)
        {
            break;
        }

        ii += 1;
        jj += 1;
    }

    // bottom
    ii = i + 1;
    jj = j;
    while (ii < rows)
    {
        if (seatings[ii][jj] == OCCUPIED_SEAT)
        {
            neighbors += 1;
            break;
        }
        else if (seatings[ii][jj] == EMPTY_SEAT)
        {
            break;
        }

        ii += 1;
    }

    // bottom left
    ii = i + 1;
    jj = j - 1;
    while (ii < rows && jj >= 0)
    {
        if (seatings[ii][jj] == OCCUPIED_SEAT)
        {
            neighbors += 1;
            break;
        }
        else if (seatings[ii][jj] == EMPTY_SEAT)
        {
            break;
        }

        ii += 1;
        jj -= 1;
    }

    // left
    ii = i;
    jj = j - 1;
    while (jj >= 0)
    {
        if (seatings[ii][jj] == OCCUPIED_SEAT)
        {
            neighbors += 1;
            break;
        }
        else if (seatings[ii][jj] == EMPTY_SEAT)
        {
            break;
        }

        jj -= 1;
    }

    return neighbors;
}


void part1(istream& inputFile)
{
    vector<string> seatings;

    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        seatings.push_back(line);
    }

    // printSeatings(seatings);

    const auto rows = seatings.size();
    const auto cols = seatings.front().size();

    bool hasChange = true;

    vector<string> auxSeatings(rows);
    while (hasChange)
    {
        hasChange = false;
        auxSeatings = seatings;

        for (size_t i = 0; i < rows; ++i)
        {
            auto row = seatings[i];
            for (size_t j = 0; j < cols; ++j)
            {
                if (row[j] != FLOOR)
                {
                    const auto numNeighbors = neighbors(seatings, i, j);

                    if (row[j] == EMPTY_SEAT && numNeighbors == 0)
                    {
                        row[j] = OCCUPIED_SEAT;
                        hasChange = true;
                    }
                    else if (row[j] == OCCUPIED_SEAT && numNeighbors >= 4)
                    {
                        row[j] = EMPTY_SEAT;
                        hasChange = true;
                    }
                }
            }

            auxSeatings[i] = row;
        }

        seatings = auxSeatings;
    }

    size_t occupied = 0;
    for_each(
        seatings.begin(),
        seatings.end(),
        [&occupied](const string& row) { occupied += count(row.begin(), row.end(), OCCUPIED_SEAT); });

    cout << occupied << endl;
}

void part2(istream& inputFile)
{
    vector<string> seatings;

    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        seatings.push_back(line);
    }

    // printSeatings(seatings);

    const auto rows = seatings.size();
    const auto cols = seatings.front().size();

    bool hasChange = true;

    vector<string> auxSeatings(rows);
    while (hasChange)
    {
        hasChange = false;
        auxSeatings = seatings;

        for (size_t i = 0; i < rows; ++i)
        {
            auto row = seatings[i];
            for (size_t j = 0; j < cols; ++j)
            {
                if (row[j] != FLOOR)
                {
                    const auto numNeighbors = rayNeighbors(seatings, i, j);

                    if (row[j] == EMPTY_SEAT && numNeighbors == 0)
                    {
                        row[j] = OCCUPIED_SEAT;
                        hasChange = true;
                    }
                    else if (row[j] == OCCUPIED_SEAT && numNeighbors >= 5)
                    {
                        row[j] = EMPTY_SEAT;
                        hasChange = true;
                    }
                }
            }

            auxSeatings[i] = row;
        }

        seatings = auxSeatings;

        // printSeatings(seatings);
    }

    size_t occupied = 0;
    for_each(
        seatings.begin(),
        seatings.end(),
        [&occupied](const string& row) { occupied += count(row.begin(), row.end(), OCCUPIED_SEAT); });

    cout << occupied << endl;
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