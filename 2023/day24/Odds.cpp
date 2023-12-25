#include "../../util/StringUtil.h"
#include "../../util/MathUtil.h"
#include "../../util/Types.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint>
#include <vector>
#include <optional>
#include <cassert>

using namespace std;
using namespace aoc::util::types;

struct Hailstone {
    coord3d_signed_t p0;
    coord3d_signed_t dv;

    const double dydxSlope() const {
        if (dv.x == 0) {
            return INT64_MAX;
        }

        return (double)dv.y / (double)dv.x;
    }

    const double dzdxSlope() const {
        if (dv.x == 0) {
            return INT64_MAX;
        }

        return (double)dv.z / (double)dv.x;
    }

    const double cXY() const {
        return p0.y - dydxSlope() * p0.x;
    }

    const double cXZ() const {
        return p0.z - dzdxSlope() * p0.x;
    }
};

struct Input {
    vector<Hailstone> hailstones;
};

void _printHailstone(const Hailstone &hailstone) {
    cout << hailstone.p0.x << ", " << hailstone.p0.y << ", " << hailstone.p0.z << " @ ";
    cout << hailstone.dv.x << ", " << hailstone.dv.y << ", " << hailstone.dv.z;
}

void _printInput(const Input &input) {
    cout << "Hailstones: (" << input.hailstones.size() << ")" << endl;
    for (const auto &hailstone : input.hailstones) {
        _printHailstone(hailstone);
        cout << endl;
    }
    cout << endl;
}

const Input _parseInput(istream& inputFile) {
    Input input;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        const auto parts = aoc::util::string::split(line, '@');
        const auto position = parts[0];
        const auto velocity = parts[1];


        const auto positionCoords = aoc::util::string::split(position, ',');
        const auto velocityCoords = aoc::util::string::split(velocity, ',');

        input.hailstones.emplace_back(Hailstone {
            .p0 = { stoll(positionCoords[0]),
                    stoll(positionCoords[1]),
                    stoll(positionCoords[2]) },
            .dv = { stoll(velocityCoords[0]),
                    stoll(velocityCoords[1]),
                    stoll(velocityCoords[2]) },
        });
    }

    return input;
}

const optional<coord_double_t> _findIntersectionXY(const Hailstone &h1, const Hailstone &h2) {
    const auto m1 = h1.dydxSlope();
    const auto c1 = h1.cXY();
    const auto m2 = h2.dydxSlope();
    const auto c2 = h2.cXY();

    if (m1 == m2) {
        // parallel lines
        return nullopt;
    }

    const auto intersectX = (double)(c2 - c1) / (m1 - m2);
    const auto intersectY = m1 * intersectX + c1;
    const auto intersectT1 = (double)(intersectX - h1.p0.x) / (double)h1.dv.x;
    const auto intersectT2 = (double)(intersectX - h2.p0.x) / (double)h2.dv.x;

    if (intersectT1 < 0 || intersectT2 < 0) {
        // intersection happened in the past
        // cout << "Intersection in the past" << endl;
        return nullopt;
    }

    return coord_double_t{intersectX, intersectY};
}

const optional<coord_double_t> _findIntersectionXZ(const Hailstone &h1, const Hailstone &h2) {
    const auto m1 = h1.dzdxSlope();
    const auto c1 = h1.cXZ();
    const auto m2 = h2.dzdxSlope();
    const auto c2 = h2.cXZ();

    if (m1 == m2) {
        // parallel lines
        return nullopt;
    }

    const auto intersectX = (double)(c2 - c1) / (m1 - m2);
    const auto intersectZ = m1 * intersectX + c1;
    const auto intersectT1 = (double)(intersectX - h1.p0.x) / (double)h1.dv.x;
    const auto intersectT2 = (double)(intersectX - h2.p0.x) / (double)h2.dv.x;

    if (intersectT1 < 0 || intersectT2 < 0) {
        // intersection happened in the past
        return nullopt;
    }

    return coord_double_t{intersectX, intersectZ};
}

void part1(const Input &input) {
    // use these for real input day24.in
    constexpr size_t kMin = 200000000000000;
    constexpr size_t kMax = 400000000000000;
    // use these for test1.in
    // constexpr size_t kMin = 7;
    // constexpr size_t kMax = 27;
    // use these for test3.in
    // constexpr size_t kMin = 1;
    // constexpr size_t kMax = 30;
    const pair<size_t, size_t> intersectionWindowX = {kMin, kMax};
    const pair<size_t, size_t> intersectionWindowY = {kMin, kMax};

    size_t numIntersections = 0;
    for (size_t i = 0; i < input.hailstones.size() - 1; i++) {
        const auto &h1 = input.hailstones[i];
        for (size_t j = i + 1; j < input.hailstones.size(); j++) {
            const auto &h2 = input.hailstones[j];

            const auto intersection = _findIntersectionXY(h1, h2);
            if (!intersection) {
                continue;
            }

            const auto intersectionX = (*intersection).first;
            const auto intersectionY = (*intersection).second;

            if (intersectionWindowX.first <= intersectionX && intersectionX <= intersectionWindowX.second &&
                intersectionWindowY.first <= intersectionY && intersectionY <= intersectionWindowY.second) {
                // cout << "Hailstone " << i << ": ";
                // _printHailstone(h1);
                // cout << endl;
                // cout << "Hailstone " << j << ": ";
                // _printHailstone(h2);
                // cout << endl;

                // cout << "Intersect point: " << intersectionX << ", " << intersectionY << endl << endl;
                numIntersections += 1;
            }
        }
    }

    cout << numIntersections << endl;
}

const optional<coord_double_t> _findIntersectionForHailstonesXY(const vector<Hailstone> &hailstones) {
    optional<coord_double_t> intersectionXYPoint = nullopt;
    for (size_t i = 0; i < 1; i++) {
        const auto &h1 = hailstones[i];
        for (size_t j = i + 1; j < hailstones.size(); j++) {
            const auto &h2 = hailstones[j];

            const auto currentIntersectionXY = _findIntersectionXY(h1, h2);
            if (!currentIntersectionXY) {
                // cout << "ERROR: No intersection XY found between hailstones: " << endl;
                // cout << "Hailstone " << i << ": ";
                // _printHailstone(h1);
                // cout << endl;
                // cout << "Hailstone " << j << ": ";
                // _printHailstone(h2);
                // cout << endl;

                return nullopt;
            }

            if (!intersectionXYPoint.has_value()) {
                // we found first intersection point
                intersectionXYPoint = currentIntersectionXY;
            } else {
                // check if the current intersection point is the same with the previously found one
                if (fabs((*intersectionXYPoint).first - (*currentIntersectionXY).first) >= 1.0 ||
                    fabs((*intersectionXYPoint).second - (*currentIntersectionXY).second) >= 1.0) {
                    // not good intersection found
                    // cout << "DIFF X: " << fabs((*intersectionXYPoint).first - (*currentIntersectionXY).first) << endl;
                    // cout << "DIFF Y: " << fabs((*intersectionXYPoint).second - (*currentIntersectionXY).second) << endl;
                    // cout << "EPS   : " << numeric_limits<double>::epsilon() << endl;
                    // cout << "Not good intersection XY found between hailstones: " << endl;
                    // cout << "Hailstone " << i << ": ";
                    // _printHailstone(h1);
                    // cout << endl;
                    // cout << "Hailstone " << j << ": ";
                    // _printHailstone(h2);
                    // cout << endl;
                    // cout << "Current  intersection: " << (*currentIntersectionXY).first << ", " << (*currentIntersectionXY).second << endl;
                    // cout << "Previous intersection: " << (*intersectionXYPoint).first << ", " << (*intersectionXYPoint).second << endl;

                    return nullopt;
                }
            }
        }
    }

    return intersectionXYPoint;
}

const optional<coord_double_t> _findIntersectionForHailstonesXZ(const vector<Hailstone> &hailstones) {
    optional<coord_double_t> intersectionXZPoint = nullopt;
    for (size_t i = 0; i < 1; i++) {
        const auto &h1 = hailstones[i];
        for (size_t j = i + 1; j < hailstones.size(); j++) {
            const auto &h2 = hailstones[j];

            const auto currentIntersectionXZ = _findIntersectionXZ(h1, h2);
            if (!currentIntersectionXZ) {
                // cout << "ERROR: No intersection XZ found between hailstones: " << endl;
                // cout << "Hailstone " << i << ": ";
                // _printHailstone(h1);
                // cout << endl;
                // cout << "Hailstone " << j << ": ";
                // _printHailstone(h2);
                // cout << endl;

                return nullopt;
            }

            if (!intersectionXZPoint.has_value()) {
                // we found first intersection point
                intersectionXZPoint = currentIntersectionXZ;
            } else {
                // check if the current intersection point is the same with the previously found one
                if (fabs((*intersectionXZPoint).first - (*currentIntersectionXZ).first) >= 1.0 ||
                    fabs((*intersectionXZPoint).second - (*currentIntersectionXZ).second) >= 1.0) {
                    // not good intersection found
                    // cout << "DIFF X: " << fabs((*intersectionXZPoint).first - (*currentIntersectionXZ).first) << endl;
                    // cout << "DIFF Y: " << fabs((*intersectionXZPoint).second - (*currentIntersectionXZ).second) << endl;
                    // cout << "Not good intersection XZ found between hailstones: " << endl;
                    // cout << "Hailstone " << i << ": ";
                    // _printHailstone(h1);
                    // cout << endl;
                    // cout << "Hailstone " << j << ": ";
                    // _printHailstone(h2);
                    // cout << endl;
                    // cout << "Current  intersection: " << (*currentIntersectionXZ).first << ", " << (*currentIntersectionXZ).second << endl;
                    // cout << "Previous intersection: " << (*intersectionXZPoint).first << ", " << (*intersectionXZPoint).second << endl;

                    return nullopt;
                }
            }
        }
    }

    return intersectionXZPoint;
}

const vector<Hailstone> _getRockRelativeAdjustedHailstonesXY(const coord_signed_t &dxdyRock, const Input &input, const size_t maxHailstones) {
    const auto maxUsableHailstones = min(input.hailstones.size(), maxHailstones);

    vector<Hailstone> adjustedHailstones;
    for (size_t i = 0; i < maxUsableHailstones; i++) {
        const auto &hailstone = input.hailstones[i];

        adjustedHailstones.emplace_back(Hailstone {
            .p0 = hailstone.p0,
            .dv = {
                .x = hailstone.dv.x - dxdyRock.first,
                .y = hailstone.dv.y - dxdyRock.second,
                .z = hailstone.dv.z,
            }
        });
    }

    return adjustedHailstones;
}

const vector<Hailstone> _getRockRelativeAdjustedHailstonesXZ(const coord_signed_t &dxdzRock, const Input &input, const size_t maxHailstones) {
    const auto maxUsableHailstones = min(input.hailstones.size(), maxHailstones);

    vector<Hailstone> adjustedHailstones;
    for (size_t i = 0; i < maxUsableHailstones; i++) {
        const auto &hailstone = input.hailstones[i];

        adjustedHailstones.emplace_back(Hailstone {
            .p0 = hailstone.p0,
            .dv = {
                .x = hailstone.dv.x - dxdzRock.first,
                .y = hailstone.dv.y,
                .z = hailstone.dv.z - dxdzRock.second,
            }
        });
    }

    return adjustedHailstones;
}

// Reference: https://www.reddit.com/r/adventofcode/comments/18pptor/comment/kepufsi/?utm_source=reddit&utm_medium=web2x&context=3
// Reference: https://www.reddit.com/r/adventofcode/comments/18pptor/comment/keqwg08/?utm_source=reddit&utm_medium=web2x&context=3
void part2(const Input &input) {
    constexpr size_t kMaxHailstonesUsed = 20;

    coord3d_signed_t rockPosition;
    bool done = false;
    for (size_t i = 0; i < SIZE_T_MAX && !done; i++) {
        for (size_t j = 0; j <= i && !done; j++) {
            for (const auto [dxRockAbs, dyRockAbs] : vector<pair<size_t, size_t>>{ {i, j}, {j, i} }) {
                // generate all possible combinations of velocities, starting from the absolute values `dxRock` and `dyRock`
                for (const auto [signX, signY] : vector<coord_signed_t>{ {1, 1} , {1, -1}, {-1, 1}, {-1, -1} }) {
                // for (const auto [signX, signY] : vector<coord_signed_t>{ {-1, 1} }) {
                    const int64_t dxRock = (int64_t)dxRockAbs * signX;
                    const int64_t dyRock = (int64_t)dyRockAbs * signY;

                    if (dxRock == -99 && dyRock == -269) {
                        cout << "Got XY" << endl;
                    }

                    const auto adjustedHailstonesXY = _getRockRelativeAdjustedHailstonesXY({dxRock, dyRock}, input, kMaxHailstonesUsed);

                    const auto intersectionXY = _findIntersectionForHailstonesXY(adjustedHailstonesXY);
                    if (!intersectionXY) {
                        continue;
                    }

                    // we found intersection on XY plane, now search for an intersection on XZ plane; this will guarantee our intersection
                    // on XYZ
                    for (size_t dzRockAbs = 0; dzRockAbs < SIZE_T_MAX && !done; dzRockAbs++) {
                        for (const auto signZ : {1, -1}) {
                            const int64_t dzRock = (int64_t)dzRockAbs * signZ;

                            const auto adjustedHailstonesXZ = _getRockRelativeAdjustedHailstonesXZ({dxRock, dzRock}, input, kMaxHailstonesUsed);
                            const auto intersectionXZ = _findIntersectionForHailstonesXZ(adjustedHailstonesXZ);
                            if (!intersectionXZ) {
                                continue;
                            }

                            // we found intersection on XZ plane, so now we are done
                            rockPosition.x = (*intersectionXY).first;
                            rockPosition.y = (*intersectionXY).second;
                            rockPosition.z = (*intersectionXZ).second;

                            done = true;
                            break;
                        }
                    }
                }
            }
        }
    }

    if (!done) {
        cout << "Failed to find rock position coords" << endl;
        exit(1);
    }

    const auto rockPositionCoordsSum = rockPosition.x + rockPosition.y + rockPosition.z;

    cout << rockPositionCoordsSum << endl;
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        std::cout << "Must specify input file!" << endl;
        return 0;
    }

    std::cout << "Input file name: " << argv[1] << endl;
    ifstream inputFile(argv[1]);

    const auto input = _parseInput(inputFile);
    _printInput(input);

    std::cout << "Part 1: " << endl;
    part1(input);

    std::cout << "Part 2: " << endl;
    part2(input);

    inputFile.close();

    return 0;
}