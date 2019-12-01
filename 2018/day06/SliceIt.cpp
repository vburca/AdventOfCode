#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <tuple>
#include <regex>

using namespace std;

typedef tuple<int, int> key_coord;

struct key_hash : public unary_function<key_coord, size_t>
{
    size_t operator()(const key_coord& k) const
    {
        size_t seed = 0;
        seed ^= hash<int>()(get<0>(k)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= hash<int>()(get<1>(k)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

        return seed;
    }
};

// struct key_equal : public binary_function<key_coord, key_coord, bool>
// {
//     bool operator()(const key_coord& k1, const key_coord& k2) const
//     {
//         return k1 == k2;
//     }
// };

void part1(istream& inputFile)
{
    unordered_map<const key_coord, int, key_hash> usedSqInches;
    regex claimRegex("#(\\d+) @ (\\d+),(\\d+): (\\d+)x(\\d+)[\\r\\n]*");
    smatch claimMatch;

    string claim;
    while (!inputFile.eof())
    {
        // read claim
        getline(inputFile, claim);

        unsigned int claimId = 0;
        unsigned int x = 0;
        unsigned int y = 0;
        unsigned int w = 0;
        unsigned int h = 0;

        // beautiful regex matching for parsing the claim
        if (regex_match(claim, claimMatch, claimRegex))
        {
            for (unsigned int i = 1; i < claimMatch.size(); i++)
            {
                string match = claimMatch[i].str();
                stringstream ss(match);

                switch (i)
                {
                    case 1: ss >> claimId;
                    case 2: ss >> y;
                    case 3: ss >> x;
                    case 4: ss >> w;
                    case 5: ss >> h;
                }
            }
        }
        else
        {
            cerr << "No match for " << claim << endl;
            exit(1);
        }

        // add square inch in the used map, and count
        for (unsigned int j = 0; j < w; j++)
        {
            for (unsigned int i = 0; i < h; i++)
            {
                key_coord k = make_tuple(x + i, y + j);

                if (usedSqInches.find(k) == usedSqInches.end())
                {
                    usedSqInches.insert({k, 1});
                }
                else
                {
                    usedSqInches[k] += 1;
                }
            }
        }
    }

    // count overlapping square inches
    unsigned int count = 0;
    for (pair<key_coord, int> e : usedSqInches)
    {
        // printf("(%d, %d) = %d\n", get<0>(e.first) + 1, get<1>(e.first) + 1, e.second);
        count += e.second > 1 ? 1 : 0;
    }

    cout << count << endl;
}

void part2(istream& inputFile)
{
    unordered_map<const key_coord, int, key_hash> usedSqInches;
    vector<string> inputs;
    regex claimRegex("#(\\d+) @ (\\d+),(\\d+): (\\d+)x(\\d+)[\\r\\n]*");
    smatch claimMatch;

    string claim;
    while (!inputFile.eof())
    {
        // read claim
        getline(inputFile, claim);
        inputs.push_back(claim);

        unsigned int claimId = 0;
        unsigned int x = 0;
        unsigned int y = 0;
        unsigned int w = 0;
        unsigned int h = 0;

        // beautiful regex matching for parsing the claim
        if (regex_match(claim, claimMatch, claimRegex))
        {
            for (unsigned int i = 1; i < claimMatch.size(); i++)
            {
                string match = claimMatch[i].str();
                stringstream ss(match);

                switch (i)
                {
                    case 1: ss >> claimId;
                    case 2: ss >> y;
                    case 3: ss >> x;
                    case 4: ss >> w;
                    case 5: ss >> h;
                }
            }
        }
        else
        {
            cerr << "No match for " << claim << endl;
            exit(1);
        }

        // add square inch in the used map, and count
        for (unsigned int j = 0; j < w; j++)
        {
            for (unsigned int i = 0; i < h; i++)
            {
                key_coord k = make_tuple(x + i, y + j);

                if (usedSqInches.find(k) == usedSqInches.end())
                {
                    usedSqInches.insert({k, 1});
                }
                else
                {
                    usedSqInches[k] += 1;
                }
            }
        }
    }

    for (string claim : inputs)
    {
        unsigned int claimId = 0;
        unsigned int x = 0;
        unsigned int y = 0;
        unsigned int w = 0;
        unsigned int h = 0;

        // beautiful regex matching for parsing the claim
        if (regex_match(claim, claimMatch, claimRegex))
        {
            for (unsigned int i = 1; i < claimMatch.size(); i++)
            {
                string match = claimMatch[i].str();
                stringstream ss(match);

                switch (i)
                {
                    case 1: ss >> claimId;
                    case 2: ss >> y;
                    case 3: ss >> x;
                    case 4: ss >> w;
                    case 5: ss >> h;
                }
            }
        }
        else
        {
            cerr << "No match for " << claim << endl;
            exit(1);
        }

        // check overlapping count for each of its square inches
        bool nonOverlapping = true;
        for (unsigned int j = 0; j < w && nonOverlapping; j++)
        {
            for (unsigned int i = 0; i < h && nonOverlapping; i++)
            {
                key_coord k = make_tuple(x + i, y + j);

                if (usedSqInches.find(k) != usedSqInches.end())
                {
                    nonOverlapping &= (usedSqInches[k] == 1);
                }
                else
                {
                    cerr << "Square inch not marked in map for claim:" << endl << claim << endl;
                    exit(1);
                }
            }
        }

        if (nonOverlapping)
        {
            cout << claimId << endl;
            break;
        }
    }
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