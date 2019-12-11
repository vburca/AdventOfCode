#include <iostream>
#include <fstream>

#include <vector>
#include <sstream>
#include <string>

using namespace std;

typedef vector<int> memory_t;

// overload the stream input operator to read comma separated integers from each line of an input file
// into a vector of integers
istream& operator >> (istream& ins, memory_t& data)
{
    // clear the output first
    data.clear();

    // read one line from the file
    string line;
    getline(ins, line);

    // separate the integers from the commas
    stringstream ss(line);
    string sVal;

    while (getline(ss, sVal, ','))
    {
        // we convert each string value from the comma separated values into an integer
        stringstream intAsString(sVal);
        int val = 0;
        intAsString >> val;

        // now push it into the vector
        data.push_back(val);
    }

    // we converted values from the csv line from the file as a vector of ints
    // we can return the argument stream, as per the overload
    return ins;
}

void printMemory(const memory_t& memory)
{
    for (int i = 0; i < memory.size(); i++)
    {
        cout << memory[i] << " ";
    }

    cout << endl;
}

void part1(istream& inputFile)
{
    while (!inputFile.eof())
    {
        string line;
        memory_t memory;

        inputFile >> memory;

        // set memory to "1202 program alarm"
        memory[1] = 12;
        memory[2] = 2;

        // now parse the program instructions
        int i = 0;
        while (memory[i] != 99)
        {
            int a = memory[memory[i + 1]];
            int b = memory[memory[i + 2]];

            switch (memory[i])
            {
                case 1: memory[memory[i + 3]] = a + b; break;
                case 2: memory[memory[i + 3]] = a * b; break;
                default: break;
            }

            i += 4;
        }

        // printMemory(memory);
        cout << memory[0] << endl;
    }
}

void part2(istream& inputFile)
{
    while (!inputFile.eof())
    {
        string line;
        memory_t memory;

        inputFile >> memory;

        int noun = 0;
        int verb = 0;

        bool done = false;
        for (int i = 0; i < 100 && !done; i++)
        {
            noun = i;
            for (int j = 0; j < 100 && !done; j++)
            {
                verb = j;

                memory_t memoryCopy(memory);

                // set noun and verb parameters
                memoryCopy[1] = noun;
                memoryCopy[2] = verb;

                // now parse the program instructions
                int i = 0;
                while (memoryCopy[i] != 99)
                {
                    int a = memoryCopy[memoryCopy[i + 1]];
                    int b = memoryCopy[memoryCopy[i + 2]];

                    switch (memoryCopy[i])
                    {
                        case 1: memoryCopy[memoryCopy[i + 3]] = a + b; break;
                        case 2: memoryCopy[memoryCopy[i + 3]] = a * b; break;
                        default: break;
                    }

                    i += 4;
                }

                if (memoryCopy[0] == 19690720)
                {
                    done = true;
                }
            }
        }

        // printMemory(memory);
        cout << 100 * noun + verb << endl;
    }}


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