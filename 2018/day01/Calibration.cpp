#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>

using namespace std;

void part1(istream& inputFile)
{
    int frequencyChange;
    int resultingFrequency = 0;

    while (!inputFile.eof())
    {
        inputFile >> frequencyChange;
        resultingFrequency += frequencyChange;
    }

    cout << resultingFrequency << endl;
}

void part2(istream& inputFile)
{
    vector<int> inputs;
    unordered_set<int> previousFrequencies;

    int frequencyChange;
    int resultingFrequency = 0;
    unsigned int i = 0;
    bool done = false;

    done = inputFile.eof(); // check for empty files
    while (!done)
    {
        // get next element
        // either from file
        if (!inputFile.eof())
        {
            inputFile >> frequencyChange;
            inputs.push_back(frequencyChange);
        }
        // or start over the inputs again
        else
        {
            if (inputs.size() <= 0)
            {
                cout << "No cached inputs! Exiting." << endl;
                return;
            }

            frequencyChange = inputs[i++ % inputs.size()];
        }

        // calculate new frequency
        resultingFrequency += frequencyChange;

        // check if we've seen this frequency before
        if (previousFrequencies.find(resultingFrequency) == previousFrequencies.end())
        {
            previousFrequencies.insert(resultingFrequency);
        }
        else
        {
            // we are done, we found a repeating frequency
            done = true;
        }
    }

    cout << resultingFrequency << endl;
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