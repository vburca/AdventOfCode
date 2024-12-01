#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint>
#include <vector>
#include <utility>
#include <optional>
#include <algorithm>
#include <map>

using namespace std;

void part1(istream& inputFile) {
    size_t distance = 0;

    vector<size_t> locationIdsA;
    vector<size_t> locationIdsB;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        istringstream iss(line);
        size_t locationId;
        iss >> locationId;
        locationIdsA.push_back(locationId);
        iss >> locationId;
        locationIdsB.push_back(locationId);
    }

    sort(locationIdsA.begin(), locationIdsA.end());
    sort(locationIdsB.begin(), locationIdsB.end());

    for (size_t i = 0; i < locationIdsA.size(); ++i) {
        distance += abs(static_cast<int>(locationIdsA[i]) - static_cast<int>(locationIdsB[i]));
    }

    cout << distance << endl;
}

void part2(istream& inputFile) {
    size_t similarityScore = 0;

    map<size_t, size_t> locationIdsOccurrencesA;
    map<size_t, size_t> locationIdsOccurrencesB;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        istringstream iss(line);
        size_t locationId;
        iss >> locationId;
        if (locationIdsOccurrencesA.contains(locationId)) {
            locationIdsOccurrencesA[locationId] += 1;
        } else {
            locationIdsOccurrencesA[locationId] = 1;
        }
        iss >> locationId;
        if (locationIdsOccurrencesB.contains(locationId)) {
            locationIdsOccurrencesB[locationId] += 1;
        } else {
            locationIdsOccurrencesB[locationId] = 1;
        }
    }

    for (const auto &[locationId, occurrences] : locationIdsOccurrencesA) {
        similarityScore += occurrences * locationId * (
            locationIdsOccurrencesB.contains(locationId) ?
            locationIdsOccurrencesB[locationId] : 0);
    }

    cout << similarityScore << endl;
}

int main(int argc, char **argv) {
    if (argc <= 1)
    {
        std::cout << "Must specify input file!" << endl;
        return 0;
    }

    std::cout << "Input file name: " << argv[1] << endl;
    ifstream inputFile(argv[1]);

    std::cout << "Part 1: " << endl;
    part1(inputFile);

    inputFile.clear();
    inputFile.seekg(0, ios::beg);

    std::cout << "Part 2: " << endl;
    part2(inputFile);

    inputFile.close();

    return 0;
}