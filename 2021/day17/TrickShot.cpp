#include <iostream>
#include <fstream>
#include <regex>
#include <sstream>
#include <cmath>
#include <string>
#include <unordered_map>

using namespace std;

typedef pair<int16_t, int16_t> point_t;

const int16_t getXPositionAfterSteps(const int16_t vx, const size_t kSteps)
{
    // x_pos(k steps) = 0 + vx + (vx - 1) + (vx - 2) + ... + (vx - (k - 1))  -> x position after k steps
    // -- the caveat is that we can only decrease (or increase) x until it reaches 0, and nothing after
    // -- to simplify the logic, we will deal with vx in absolute value |vx| (i.e. displacement / distance)
    //    and adjust at the end based on its sign
    // -- so if |vx| < k steps, we will need to stop after vx decreases of vx (this is when it reaches 0), not
    //    after k steps; so the sum will have to iterate for min = min(k steps, |vx|) => ** actual number of allowed
    //    steps (that we can subtract) is constrainedK steps = min = min(k steps, |vx|)
    //
    // => x_pos(k steps) = 1/2 * constrainedK * (2 * vy - (constrainedK - 1))

    const auto constrainedKSteps = min(kSteps, abs(vx));
    const auto sign = vx / abs(vx);

    return sign * 0.5 * constrainedKSteps * (2 * vx - (constrainedKSteps - 1));
}

const int16_t getYPositionAfterSteps(const int16_t vy, const size_t kSteps)
{
    // y_pos(k steps) = 0 + y + (vy - 1) + (vy - 2) + ... + (vy - (k - 1))  -> y position after k steps
    //                = 1/2 * k * (2 * vy - (k - 1))

    return 0.5 * kSteps * (2 * vy - (kSteps - 1))
}

const point_t getPositionAfterSteps(const point_t& velocity, const size_t kSteps)
{
    return point_t{getXPositionAfterSteps(velocity.first, kSteps), getYPositionAfterSteps(velocity.second, kSteps)};
}

const map<size_t, vector<int16_t>> getPossibleVX(const point_t& targetX)
{
    map<size_t, vector<int16_t>> possibleVX;

    // the maximum x velocity is the maximum target X, since any initial velocity greater than that will overjump the
    // target area
    for (int16_t vx = 0; vx <= targetX.second; ++vx)
    {
        bool missed = false;
        size_t kSteps = 1;
        while (!missed)
        {
            xPos = getXPositionAfterSteps(vx, kSteps);

            if (targetX.first <= xPos && xPos <= targetX.second)
            {
                // landed in the target area
                possibleVX[kSteps].push_back(vx);
            }

            // outside target area
            if (xPos > targetX.second || xPos < kSteps)
            {
                missed = true;
            }
        }
    }

    return possibleVX;
}


const map<size_t, vector<int16_t>> getPossibleVY(const point_t& targetY)
{
    map<size_t, vector<int16_t>> possibleVY;

    for (int16_t vy = targetY.first; vy <= abs(targetY.first); ++vy)
    {
        bool missed = false;
        size_t kSteps = 1;
        while (!missed)
        {   asdasd
            yPos = getYPositionAfterSteps(vy, kSteps);

            if (targetX.first <= xPos && xPos <= targetX.second)
            {
                // landed in the target area
                possibleVY[kSteps].push_back(vy);
            }

            // outside target area
            if (xPos > targetX.second || xPos < kSteps)
            {
                missed = true;
            }
        }
    }

    return possibleVY;
}


const bool checkTargetHit(const point_t& velocity, const point_t& targetX, const point_t& targetY)
{
    point_t currentPosition = {0, 0};
    point_t currentVelocity = velocity;

    const size_t sign = currentVelocity.first / abs(currentVelocity.first); // +/- 1

    bool missed = false;
    bool landed = false;

    while (!landed && !missed)
    {
        // update position
        currentPosition.first += currentVelocity.first;
        currentPosition.second += currentVelocity.second;

        // update velocity
        currentVelocity.first += (-1) * sign;
        currentVelocity.second -= 1;

        // check if we landed or missed
        if ((targetX.first <= currentPosition.first && currentPosition.first <= targetX.second) &&
            (targetY.first <= currentPosition.second && currentPosition.second <= targetY.second))
        {
            landed = true;
        }

        if (!landed)
        {
            // not landed, check if we missed
            if (currentPosition.first >= targetX.second || currentPosition.second <= targetY.first)
            {
                // we went lower or farther than the target region
                missed = true;
            }
        }
    }

    return landed && !missed;
}


void part1(istream& inputFile)
{
    const regex rx("target area: x=(-?[0-9]+)..(-?[0-9]+), y=(-?[0-9]+)..(-?[0-9]+)");

    point_t targetX, targetY;
    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        smatch match;
        if (regex_match(line, match, rx))
        {
            stringstream xMinStr(match[1].str());
            stringstream xMaxStr(match[2].str());
            stringstream yMinStr(match[3].str());
            stringstream yMaxStr(match[4].str());

            targetX = {stoi(xMinStr.str()), stoi(xMaxStr.str())};
            targetY = {stoi(yMinStr.str()), stoi(yMaxStr.str())};
        }
    }

    const auto maxYDistance = abs(targetY.first) * abs(targetY.first + 1) / 2;
    cout << maxYDistance << endl;
}

void part2(istream& inputFile)
{
    const regex rx("target area: x=(-?[0-9]+)..(-?[0-9]+), y=(-?[0-9]+)..(-?[0-9]+)");

    point_t targetX, targetY;
    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        smatch match;
        if (regex_match(line, match, rx))
        {
            stringstream xMinStr(match[1].str());
            stringstream xMaxStr(match[2].str());
            stringstream yMinStr(match[3].str());
            stringstream yMaxStr(match[4].str());

            targetX = {stoi(xMinStr.str()), stoi(xMaxStr.str())};
            targetY = {stoi(yMinStr.str()), stoi(yMaxStr.str())};
        }
    }

    const auto minX = static_cast<int16_t>(ceil((-1 + sqrt(1 + 8 * targetX.first)) / 2));
    const auto maxX = targetX.second;

    const auto minY = static_cast<int16_t>(ceil((-1 + sqrt(1 + 8 * abs(targetY.second))) / 2));
    const auto maxY = static_cast<int16_t>(ceil((-1 + sqrt(1 + 8 * abs(targetY.first))) / 2));

    cout << minX << " " << maxX << endl;
    cout << minY << " " << maxY << endl;

    size_t numDistinctVelocities = 0;
    for (int16_t x = minX; x <= maxX; ++x)
    {
        for (int16_t y = minY; y <= maxY; ++y)
        {
            const auto velocity = point_t{x, y};
            numDistinctVelocities += (size_t)checkTargetHit(velocity, targetX, targetY);
        }
    }

    cout << numDistinctVelocities << endl;
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