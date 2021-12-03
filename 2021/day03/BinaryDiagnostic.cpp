#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include <unordered_set>
#include <algorithm>

using namespace std;

void part1(istream& inputFile)
{
    vector<vector<bool>> diagnostic;

    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        vector<bool> diagnosticNumber(line.size(), 0);
        for (size_t i = 0; i < line.size(); ++i)
        {
            diagnosticNumber[i] = line[i] - '0';
        }

        diagnostic.push_back(diagnosticNumber);
    }

    const auto wordSize = diagnostic[0].size();
    vector<bool> gammaBitset(wordSize, 0);
    vector<bool> epsilonBitset(wordSize, 0);

    for (size_t pos = 0; pos < wordSize; ++pos)
    {
        size_t sum = 0;
        for (size_t i = 0; i < diagnostic.size(); ++i)
        {
            sum += diagnostic[i][pos];
        }

        const auto mostCommonBit = (sum > diagnostic.size() / 2.0f);

        gammaBitset[pos] = mostCommonBit;
        epsilonBitset[pos] = 1 - mostCommonBit;
    }

    const size_t gamma = accumulate(gammaBitset.begin(), gammaBitset.end(), 0, [](const int x, const int y) { return (x << 1) + y; });
    const size_t epsilon = accumulate(epsilonBitset.begin(), epsilonBitset.end(), 0, [](const int x, const int y) { return (x << 1) + y; });

    size_t result = gamma * epsilon;
    cout << result << endl;
}


void print(vector<bool> word)
{
    for (const auto& b : word)
    {
        cout << char('0' + b);
    }

    cout << endl;
}

size_t getCandidateId(const vector<vector<bool>>& diagnostic, const bool leastCommon = false)
{
    const auto wordSize = diagnostic[0].size();

    unordered_set<size_t> candidates;
    for (size_t candidateId = 0; candidateId < diagnostic.size(); ++candidateId) // fill with range (0, numWords)
    {
        candidates.insert(candidateId);
    }

    bool foundCandidate = false;
    while (!foundCandidate)
    {
        for (size_t pos = 0; pos < wordSize; ++pos)
        {
            // find most common bit for the current position
            size_t sum = 0;
            for (const auto& candidateId : candidates)
            {
                sum += diagnostic[candidateId][pos];
            }

            const auto mostCommonBit = (sum >= candidates.size() / 2.0f);
            const auto searchedBit = leastCommon ? 1 - mostCommonBit : mostCommonBit;

            // find words that do not have the most common bit at the current position
            unordered_set<size_t> eliminatedCandidates;
            for (const auto& candidateId : candidates)
            {
                if (diagnostic[candidateId][pos] != searchedBit)
                {
                    eliminatedCandidates.insert(candidateId);
                }
            }

            // remove them from the list of candidates
            for (const auto& candidateId : eliminatedCandidates)
            {
                candidates.erase(candidateId);
            }

            // check if we are done (i.e. if we are left with only 1 candidate)
            if (candidates.size() == 1)
            {
                foundCandidate = true;
                break;
            }
        }
    }

    return *candidates.begin();
}

void part2(istream& inputFile)
{
    vector<vector<bool>> diagnostic;

    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        vector<bool> diagnosticNumber(line.size(), 0);
        for (size_t i = 0; i < line.size(); ++i)
        {
            diagnosticNumber[i] = line[i] - '0';
        }

        diagnostic.push_back(diagnosticNumber);
    }

    const auto oxygenRatingId = getCandidateId(diagnostic, false);
    const auto co2RatingId = getCandidateId(diagnostic, true);

    const auto oxygenRatingBitset = diagnostic[oxygenRatingId];
    const auto co2RatingBitset = diagnostic[co2RatingId];;

    const size_t oxygenRating = accumulate(oxygenRatingBitset.begin(), oxygenRatingBitset.end(), 0, [](const int x, const int y) { return (x << 1) + y; });
    const size_t co2Rating = accumulate(co2RatingBitset.begin(), co2RatingBitset.end(), 0, [](const int x, const int y) { return (x << 1) + y; });

    size_t result = oxygenRating * co2Rating;
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