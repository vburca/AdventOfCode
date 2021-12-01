#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include <numeric>
#include <map>

using namespace std;

#define DEBUG 1

const regex TILE_NUM("Tile ([0-9]+):");


struct BorderEncoding
{
    BorderEncoding() {}
    BorderEncoding(uint16_t e1, uint16_t e2) : encoding1(e1), encoding2(e2) {}

    uint16_t encoding1 = 0;
    uint16_t encoding2 = 0;

    bool operator==(const BorderEncoding& border) const
    {
        return  encoding1 == border.encoding1 ||
                encoding1 == border.encoding2 ||
                encoding2 == border.encoding1 ||
                encoding2 == border.encoding2;
    }
};

struct Tile
{
    Tile() {}
    Tile(uint32_t tileId) : id(tileId) {}

    uint32_t id = 0;
    vector<uint32_t> borders;
    vector<string> raw;

    void generateBorderEncodings()
    {
        if (raw.size() == 0)
        {
            return;
        }

        const auto n = raw.size();

        vector<vector<bool>> binaryBorders(4, vector<bool>(n));

        for (size_t i = 0; i < n; ++i)
        {
            // top
            binaryBorders[0][i] = raw[0][i] == '#' ? 1 : 0;
            // right
            binaryBorders[1][i] = raw[i][n-1] == '#' ? 1 : 0;
            // left
            binaryBorders[2][i] = raw[i][0] == '#' ? 1 : 0;
            // bottom
            binaryBorders[3][i] = raw[n-1][i] == '#' ? 1 : 0;
        }

        for (const auto& b : binaryBorders)
        {
            const uint32_t e1 = encoding(b, false);
            const uint32_t e2 = encoding(b, true);

            borders.push_back(e1 * e2);
        }
    }

    private:
        uint16_t encoding(const vector<bool>& border, bool reverse)
        {
            if (reverse)
            {
                return static_cast<uint16_t>(
                    accumulate(
                        border.rbegin(),
                        border.rend(),
                        static_cast<uint16_t>(0),
                        [](const uint16_t res, const uint16_t e)
                        {
                            return (res << 1) + e;
                        }));
            }

            return static_cast<uint16_t>(
                accumulate(
                    border.begin(),
                    border.end(),
                    static_cast<uint16_t>(0),
                    [](const uint16_t res, const uint16_t e)
                    {
                        return (res << 1) + e;
                    }));
        }
};


uint16_t encoding(const vector<bool>& border, const bool reverse)
{
    if (reverse)
    {
        return static_cast<uint16_t>(
            accumulate(
                border.rbegin(),
                border.rend(),
                static_cast<uint16_t>(0),
                [](const uint16_t res, const uint16_t e)
                {
                    return (res << 1) + e;
                }));
    }

    return static_cast<uint16_t>(
        accumulate(
            border.begin(),
            border.end(),
            static_cast<uint16_t>(0),
            [](const uint16_t res, const uint16_t e)
            {
                return (res << 1) + e;
            }));
}

vector<uint32_t> generateBorderEncodings(const vector<string>& rawTile)
{
    if (rawTile.size() == 0)
    {
        return vector<uint32_t>();
    }

    const auto n = rawTile.size();

    vector<vector<bool>> binaryBorders(4, vector<bool>(n));
    for (size_t i = 0; i < n; ++i)
    {
        // top
        binaryBorders[0][i] = rawTile[0][i] == '#' ? 1 : 0;
        // right
        binaryBorders[1][i] = rawTile[i][n-1] == '#' ? 1 : 0;
        // left
        binaryBorders[2][i] = rawTile[i][0] == '#' ? 1 : 0;
        // bottom
        binaryBorders[3][i] = rawTile[n-1][i] == '#' ? 1 : 0;
    }

    vector<uint32_t> borderEncodings;
    for (const auto& b : binaryBorders)
    {
        const uint32_t e1 = encoding(b, false);
        const uint32_t e2 = encoding(b, true);

        borderEncodings.push_back(e1 * e2);
    }

    return borderEncodings;
}

void part1(istream& inputFile)
{
    map<uint32_t, vector<uint32_t>> borderEncodingToTiles;
    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        smatch match;
        if (regex_match(line, match, TILE_NUM))
        {
            uint32_t tileId;
            istringstream ss(match[1].str());
            ss >> tileId;

            // read the actual tile
            vector<string> rawTile;
            while (!inputFile.eof() && line.size() > 0)
            {
                getline(inputFile, line);

                if (line.size() > 0)
                {
                    rawTile.emplace_back(line);
                }
            }

            const auto borderEncodings = generateBorderEncodings(rawTile);

            for (const auto& encoding : borderEncodings)
            {
                if (borderEncodingToTiles.find(encoding) == borderEncodingToTiles.end())
                {
                    borderEncodingToTiles[encoding] = vector<uint32_t>();
                }

                borderEncodingToTiles[encoding].push_back(tileId);
            }
        }
    }

    map<uint32_t, size_t> tileIdToMissingNeighborCount;
    for (const auto [_, tileIds] : borderEncodingToTiles)
    {
        if (tileIds.size() == 1)
        {
            tileIdToMissingNeighborCount[tileIds[0]] += 1;
        }
    }

    if (DEBUG)
    {
        for (const auto [borderEncoding, tileIds] : borderEncodingToTiles)
        {
            cout << borderEncoding << " : ( ";
            for (const auto& tileId : tileIds)
            {
                cout << tileId << " ";
            }
            cout << ")" << endl;
        }

        // cout << endl;

        // for (const auto [tileId, missingNeighborCount] : tileIdToMissingNeighborCount)
        // {
        //     cout << tileId << " : " << missingNeighborCount << endl;
        // }
    }

    uint64_t prod = 1;
    for (const auto [tileId, missingNeighborCount] : tileIdToMissingNeighborCount)
    {
        if (missingNeighborCount == 2)
        {
            prod *= tileId;
        }
    }

    cout << prod << endl;
}

void part2(istream& inputFile)
{
    return;
    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);
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