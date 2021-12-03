#include <iostream>
#include <fstream>
#include <regex>
#include <sstream>

using namespace std;

void part1(istream& inputFile)
{
    size_t horizontalPos = 0;
    size_t depthPos = 0;

    regex rx("([a-z]+) ([0-9]+)");

    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        smatch match;
        if (regex_match(line, match, rx))
        {
            string direction = match[1].str();

            stringstream stepsStr(match[2].str());
            size_t steps;
            stepsStr >> steps;

            if (direction == "forward")
            {
                horizontalPos += steps;
            }
            else if (direction == "down")
            {
                depthPos += steps;
            }
            else if (direction == "up")
            {
                depthPos -= steps;
            }
        }
        else
        {
            cout << "Regex failed for line: [" << line << "]" << endl;
        }

    }

    size_t result = horizontalPos * depthPos;
    cout << result << endl;
}

void part2(istream& inputFile)
{
    size_t horizontalPos = 0;
    size_t depthPos = 0;
    size_t aim = 0;

    regex rx("([a-z]+) ([0-9]+)");

    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        smatch match;
        if (regex_match(line, match, rx))
        {
            string direction = match[1].str();

            stringstream stepsStr(match[2].str());
            size_t steps;
            stepsStr >> steps;

            if (direction == "forward")
            {
                horizontalPos += steps;
                depthPos += aim * steps;
            }
            else if (direction == "down")
            {
                aim += steps;
            }
            else if (direction == "up")
            {
                aim -= steps;
            }
        }
        else
        {
            cout << "Regex failed for line: [" << line << "]" << endl;
        }
    }

    size_t result = horizontalPos * depthPos;
    cout << result << endl;
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