#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <regex>
#include <utility>

using namespace std;

#define FOLDS_PART_1 1

typedef pair<size_t, size_t> dot_coord_t;

struct dot_coord_t_equal
{
    bool operator()(const dot_coord_t& c1, const dot_coord_t& c2) const
    {
        return (c1.first == c2.first) && (c1.second == c2.second);
    }
};

struct dot_coord_t_hash
{
    size_t operator()(const dot_coord_t& c) const
    {
        return c.first ^ c.second;
    }
};

struct Fold
{
    string axis;
    size_t axisCoord;
};

typedef unordered_set<dot_coord_t, dot_coord_t_hash, dot_coord_t_equal> dot_map_t;

const dot_coord_t computeCoordAfterFold(const dot_coord_t& dotCoord, const Fold& fold)
{
    const auto currentX = dotCoord.first;
    const auto currentY = dotCoord.second;
    size_t newX = 0;
    size_t newY = 0;
    if (fold.axis == "x")
    {
        newX = currentX < fold.axisCoord ? currentX : (fold.axisCoord - (currentX - fold.axisCoord)); // fold right to left
        // newX -= (fold.axisCoord + 1); // shift back to the left
        newY = currentY;
    }
    else if (fold.axis == "y")
    {
        newX = currentX;
        newY = currentY < fold.axisCoord ? currentY : (fold.axisCoord - (currentY - fold.axisCoord)); // fold bottom to up
    }
    else
    {
        cout << "Unknown fold axis: " << fold.axis << " with fold coordinate: " << fold.axisCoord << endl;
        return {};
    }


    return {newX, newY};
}

const dot_map_t performFolds(const dot_map_t& dotMap, const vector<Fold>& folds, const size_t kNumFolds)
{
    dot_map_t currentDotMap = dotMap;

    size_t foldId = 0;
    while (foldId < kNumFolds)
    {
        const auto fold = folds[foldId];

        dot_map_t dotMapAfterFold;
        for (const auto& dotCoord : currentDotMap)
        {
            dotMapAfterFold.insert(computeCoordAfterFold(dotCoord, fold));
        }

        currentDotMap = dotMapAfterFold;

        foldId += 1;
    }

    return currentDotMap;
}

void displayDotMap(const dot_map_t& dotMap)
{
    size_t maxX = 0;
    size_t maxY = 0;

    for (const auto& dot : dotMap)
    {
        maxX = dot.first > maxX ? dot.first : maxX;
        maxY = dot.second > maxY ? dot.second : maxY;
    }

    cout << endl;
    for (size_t y = 0; y <= maxY; ++y)
    {
        for (size_t x = 0; x <= maxX; ++x)
        {
            if (dotMap.find({x, y}) != dotMap.end())
            {
                cout << "#";
            }
            else
            {
                cout << ".";
            }

            cout << " ";
        }

        cout << endl;
    }

    cout << endl;
}

void part1(istream& inputFile)
{
    const regex coordsRx("([0-9]+),([0-9]+)");
    const regex foldRx("fold along ([x|y])=([0-9]+)");

    dot_map_t dotMap;
    vector<Fold> folds;

    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        if (line.size() == 0)
        {
            continue;
        }

        smatch match;
        if (regex_match(line, match, coordsRx))
        {
            stringstream xCoordStr(match[1].str());
            stringstream yCoordStr(match[2].str());

            size_t x, y;
            xCoordStr >> x;
            yCoordStr >> y;

            dotMap.insert({x, y});
        }
        else if (regex_match(line, match, foldRx))
        {
            const auto axis = match[1];
            stringstream axisCoordStr(match[2].str());

            size_t axisCoord;
            axisCoordStr >> axisCoord;

            folds.push_back(Fold{
                .axis = axis,
                .axisCoord = axisCoord});
        }
        else
        {
            cout << "Regex failed for line: " << line << endl;
            return;
        }
    }

    const auto newDotMap = performFolds(dotMap, folds, FOLDS_PART_1);
    cout << newDotMap.size() << endl;
}

void part2(istream& inputFile)
{
    const regex coordsRx("([0-9]+),([0-9]+)");
    const regex foldRx("fold along ([x|y])=([0-9]+)");

    dot_map_t dotMap;
    vector<Fold> folds;

    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        if (line.size() == 0)
        {
            continue;
        }

        smatch match;
        if (regex_match(line, match, coordsRx))
        {
            stringstream xCoordStr(match[1].str());
            stringstream yCoordStr(match[2].str());

            size_t x, y;
            xCoordStr >> x;
            yCoordStr >> y;

            dotMap.insert({x, y});
        }
        else if (regex_match(line, match, foldRx))
        {
            const auto axis = match[1];
            stringstream axisCoordStr(match[2].str());

            size_t axisCoord;
            axisCoordStr >> axisCoord;

            folds.push_back(Fold{
                .axis = axis,
                .axisCoord = axisCoord});
        }
        else
        {
            cout << "Regex failed for line: " << line << endl;
            return;
        }
    }

    const auto newDotMap = performFolds(dotMap, folds, folds.size());
    displayDotMap(newDotMap);
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