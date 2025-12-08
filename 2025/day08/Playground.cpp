#include "../../util/BasicIncludes.h"
#include "../../util/MathUtil.h"
#include "../../util/StringUtil.h"
#include "../../util/Types.h"

#include <queue>
#include <cmath>
#include <map>
#include <set>

using namespace std;
using namespace aoc::util::types;

struct JunctionBox {
    coord3d_t position;
    size_t idx;
};

struct JunctionBoxDistance {
    size_t value;
    JunctionBox boxA;
    JunctionBox boxB;
};

struct CompareJunctionBoxDistance {
    bool operator()(const JunctionBoxDistance& distanceA, const JunctionBoxDistance &distanceB) {
        return distanceA.value > distanceB.value;
    }
};

struct CompareCircuits {
    bool operator()(const set<size_t> &circuitA, const set<size_t> &circuitB) {
        return circuitA.size() < circuitB.size();
    }
};

const uint64_t calculateDistance(const JunctionBox &boxA, const JunctionBox &boxB) {
    return sqrt(pow(int(boxA.position.x) - int(boxB.position.x), 2) +
                pow(int(boxA.position.y) - int(boxB.position.y), 2) +
                pow(int(boxA.position.z) - int(boxB.position.z), 2));
}

const vector<set<size_t>> generateCircuits(const vector<JunctionBox> &boxes, const size_t kNumConnections = 1000) {
    priority_queue<JunctionBoxDistance, vector<JunctionBoxDistance>, CompareJunctionBoxDistance> distancesPQ;
    for (size_t i = 0; i < boxes.size() - 1; i++) {
        for (size_t j = i + 1; j < boxes.size(); j++) {
            const auto d = calculateDistance(boxes[i], boxes[j]);
            distancesPQ.push(JunctionBoxDistance(d, boxes[i], boxes[j]));
        }
    }

    map<size_t, size_t> boxIdxToCircuitIdx;
    map<size_t, set<size_t>> circuits;
    for (const auto &box : boxes) {
        boxIdxToCircuitIdx[box.idx] = box.idx;
        circuits[box.idx] = {box.idx};
    }

    size_t connectionIdx = 0;
    while (connectionIdx < kNumConnections && !distancesPQ.empty()) {
        const auto boxDistance = distancesPQ.top();
        distancesPQ.pop();

        connectionIdx += 1;

        const auto boxAIdx = boxDistance.boxA.idx;
        const auto boxBIdx = boxDistance.boxB.idx;

        if (boxIdxToCircuitIdx.contains(boxAIdx) &&
            boxIdxToCircuitIdx.contains(boxBIdx) &&
            boxIdxToCircuitIdx[boxAIdx] == boxIdxToCircuitIdx[boxBIdx]) {
                continue;
        }

        // merge circuits
        if (boxIdxToCircuitIdx.contains(boxAIdx) &&
            boxIdxToCircuitIdx.contains(boxBIdx) &&
            boxIdxToCircuitIdx[boxAIdx] != boxIdxToCircuitIdx[boxBIdx]) {
                const auto circuitIdxToRemove = boxIdxToCircuitIdx[boxBIdx];
                const auto circuitIdxToSave = boxIdxToCircuitIdx[boxAIdx];

                for (const auto boxIdx : circuits[circuitIdxToRemove]) {
                    boxIdxToCircuitIdx[boxIdx] = circuitIdxToSave;
                    circuits[circuitIdxToSave].insert(boxIdx);
                }

                circuits.erase(circuitIdxToRemove);

                continue;
        }
    }

    vector<set<size_t>> circuitsVector;
    for (const auto &p : circuits) {
        circuitsVector.push_back(p.second);
    }

    return circuitsVector;
}

const pair<size_t, size_t> getLastConnectedPair(const vector<JunctionBox> &boxes) {
    priority_queue<JunctionBoxDistance, vector<JunctionBoxDistance>, CompareJunctionBoxDistance> distancesPQ;
    for (size_t i = 0; i < boxes.size() - 1; i++) {
        for (size_t j = i + 1; j < boxes.size(); j++) {
            const auto d = calculateDistance(boxes[i], boxes[j]);
            distancesPQ.push(JunctionBoxDistance(d, boxes[i], boxes[j]));
        }
    }

    map<size_t, size_t> boxIdxToCircuitIdx;
    map<size_t, set<size_t>> circuits;
    for (const auto &box : boxes) {
        boxIdxToCircuitIdx[box.idx] = box.idx;
        circuits[box.idx] = {box.idx};
    }

    pair<size_t, size_t> lastConnectedBoxes;
    while (!distancesPQ.empty() && circuits.size() != 1) {
        const auto boxDistance = distancesPQ.top();
        distancesPQ.pop();

        const auto boxAIdx = boxDistance.boxA.idx;
        const auto boxBIdx = boxDistance.boxB.idx;

        lastConnectedBoxes = {boxAIdx, boxBIdx};

        if (boxIdxToCircuitIdx.contains(boxAIdx) &&
            boxIdxToCircuitIdx.contains(boxBIdx) &&
            boxIdxToCircuitIdx[boxAIdx] == boxIdxToCircuitIdx[boxBIdx]) {
                continue;
        }

        // merge circuits
        if (boxIdxToCircuitIdx.contains(boxAIdx) &&
            boxIdxToCircuitIdx.contains(boxBIdx) &&
            boxIdxToCircuitIdx[boxAIdx] != boxIdxToCircuitIdx[boxBIdx]) {
                const auto circuitIdxToRemove = boxIdxToCircuitIdx[boxBIdx];
                const auto circuitIdxToSave = boxIdxToCircuitIdx[boxAIdx];

                for (const auto boxIdx : circuits[circuitIdxToRemove]) {
                    boxIdxToCircuitIdx[boxIdx] = circuitIdxToSave;
                    circuits[circuitIdxToSave].insert(boxIdx);
                }

                circuits.erase(circuitIdxToRemove);

                continue;
        }
    }

    return lastConnectedBoxes;
}

void part1(istream& inputFile) {
    vector<JunctionBox> boxes;
    size_t i = 0;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        const auto coords = aoc::util::string::split(line, ',');
        boxes.push_back({.position = {stoull(coords[0]), stoull(coords[1]), stoull(coords[2])},
                         .idx = i});
        i += 1;
    }

    const auto circuitsVector = generateCircuits(boxes);

    // cout << "num circuits= " << circuitsVector.size() << endl;
    // for (const auto &circuit : circuitsVector) {
    //     cout << "circuit (" << circuit.size() << ")= { ";
    //     for (const auto boxIdx : circuit) {
    //         cout << "[ " << boxes[boxIdx].position.x << " " << boxes[boxIdx].position.y << " " << boxes[boxIdx].position.z << " ] ";
    //     }
    //     cout << "}" << endl;
    // }

    priority_queue<set<size_t>, vector<set<size_t>>, CompareCircuits> circuitsPQ;
    for (const auto &circuit : circuitsVector) {
        circuitsPQ.push(circuit);
    }

    uint64_t largestCircuitsSize = 1;
    const size_t kNumCircuits = 3;
    size_t circuitPopIdx = 0;
    while (circuitPopIdx < kNumCircuits && !circuitsPQ.empty()) {
        const auto circuit = circuitsPQ.top();
        circuitsPQ.pop();

        largestCircuitsSize *= circuit.size();

        circuitPopIdx += 1;
    }

    cout << largestCircuitsSize << endl;
}

void part2(istream& inputFile) {
    vector<JunctionBox> boxes;
    size_t i = 0;
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        const auto coords = aoc::util::string::split(line, ',');
        boxes.push_back({.position = {stoull(coords[0]), stoull(coords[1]), stoull(coords[2])},
                         .idx = i});
        i += 1;
    }

    const auto lastConnectedBoxes = getLastConnectedPair(boxes);

    cout << boxes[lastConnectedBoxes.first].position.x * boxes[lastConnectedBoxes.second].position.x << endl;
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