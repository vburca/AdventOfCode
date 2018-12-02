#include <iostream>
#include <fstream>
#include <unordered_set>
#include <unordered_map>

using namespace std;

void part1(istream& inputFile)
{
    int twos = 0;
    int threes = 0;

    string boxId;
    while (!inputFile.eof())
    {
        unordered_map<char, int> counts;
        bool isTwo      = false;
        bool isThree    = false;

        // read box id
        getline(inputFile, boxId);

        // start processing
        for (char& ch : boxId)
        {
            if (counts.find(ch) == counts.end())
            {
                // map doesn't track this character, so add it
                counts.insert({ch, 1});
            }
            else
            {
                // increment this character's count
                counts[ch] += 1;
            }
        }

        for (pair<char, int> element : counts)
        {
            // if we already know that it is both a two and a three boxId,
            // we can early exit from this check
            if (isTwo && isThree)
            {
                break;
            }

            // check which type this boxId is
            if (element.second == 2)
            {
                isTwo = true;
            }
            else if (element.second == 3)
            {
                isThree = true;
            }
        }

        // update counts
        twos    += isTwo ? 1 : 0;
        threes  += isThree ? 1 : 0;
    }

    // print result
    cout << twos * threes << endl;
}

void part2(istream& inputFile)
{
    unordered_set<string> boxIds;
    bool done = false;
    string result("");

    string boxId;
    while (!inputFile.eof() && !done)
    {
        // read box id
        getline(inputFile, boxId);

        for (const auto& prevBoxId : boxIds)
        {
            // error check
            if (prevBoxId.size() != boxId.size())
            {
                cerr << "Box Id lenghts are different: " << prevBoxId << " " << boxId << endl;
                continue;
            }

            // check if it's different than any of the previous box ids by exactly one character (same position)
            int diff = 0;
            // also reconstruct the common parts
            result = "";
            for (int i = 0; i < boxId.size() && (diff < 2); i++)
            {
                if (boxId[i] != prevBoxId[i])
                {
                    diff += 1;
                }
                else
                {
                    // reconstruct
                    result += boxId[i];
                }
            }

            // if it is different by exactly one character,
            if (diff == 1)
            {
                // cout << boxId << endl << prevBoxId << endl;
                done = true;
                break;
            }
        }

        boxIds.insert(boxId);
    }

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