#include "../../util/StringUtil.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint>
#include <array>
#include <vector>
#include <regex>
#include <unordered_map>

using namespace std;

enum Color {
    Red = 0,
    Green,
    Blue,

    Count,
};

const unordered_map<string, Color> kStrToColor = {
    {"red", Red},
    {"green", Green},
    {"blue", Blue},
};

const unordered_map<Color, string> kColorToStr = {
    {Red, "red"},
    {Green, "green"},
    {Blue, "blue"},
};

struct Game {
    size_t id;
    vector<array<size_t, Color::Count>> stats;
};

regex GAME_RX("Game ([0-9]+): (.*)");
regex STAT_RX("([0-9]+) (.*)");

const unordered_map<Color, size_t> kP1MaxCounts = {
    {Red, 12},
    {Green, 13},
    {Blue, 14},
};

void printGame(const Game &game) {
    std::cout << "Game " << game.id << ": ";

    for (size_t statIdx = 0; statIdx < game.stats.size(); statIdx++) {
        const auto &stats = game.stats[statIdx];
        for (size_t colorIdx = 0; colorIdx < Color::Count; colorIdx++) {
            if (stats[colorIdx] == 0) {
                continue;
            }

            cout << stats[colorIdx] << " " << kColorToStr.at(static_cast<Color>(colorIdx));

            if (colorIdx != Color::Count - 1) {
                std::cout << ", ";
            }
        }

        if (statIdx != game.stats.size() - 1) {
            std::cout << "; ";
        }
    }

    std::cout << endl;
}

void part1(istream& inputFile) {
    vector<Game> games;

    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        smatch match;
        if (!regex_match(line, match, GAME_RX)) {
            std::cout << "GAME_RX failed for line: { " << line << " } " << endl;
            continue;
        }

        stringstream gameIdStr(match[1].str());
        size_t gameId;
        gameIdStr >> gameId;
        Game game = {
            .id = gameId,
        };

        string records = match[2].str();
        // std::cout << "DEBUG Game " << gameId << " records: " << records << endl;
        const auto splitRecords = aoc::util::string::split(records, ';');
        for (const auto &pullRecord : splitRecords) {
            // std::cout << "DEBUG pullRecord: (" << pullRecord << ")" << endl;

            array<size_t, Color::Count> statCounts = { 0 };
            const auto stats = aoc::util::string::split(pullRecord, ',');
            for (const auto &stat : stats) {
                string trimmedStat = aoc::util::string::trimCopy(stat);
                // std::cout << "DEBUG stat: (" << trimmedStat << ")" << endl;

                smatch statMatch;
                if (!regex_match(trimmedStat, statMatch, STAT_RX)) {
                    std::cout << "STAT_RX failed for stat: { " << trimmedStat << " } " << endl;
                    continue;
                }

                stringstream colorCountStr(statMatch[1].str());
                size_t colorCount;
                colorCountStr >> colorCount;
                const auto colorStr = statMatch[2].str();

                statCounts[kStrToColor.at(colorStr)] = colorCount;
            }

            game.stats.push_back(statCounts);
            // printGame(game);
        }

        games.push_back(game);
    }

    size_t gameIdSum = 0;
    for (const auto &game : games) {
        // printGame(game);
        bool possible = true;
        for (const auto &statCount : game.stats) {
            if (!possible) {
                break;
            }

            for (size_t colorIdx = 0; colorIdx < Color::Count; colorIdx++) {
                if (statCount[colorIdx] > kP1MaxCounts.at(static_cast<Color>(colorIdx))) {
                    possible = false;
                    break;
                }
            }
        }

        if (possible) {
            gameIdSum += game.id;
        }
    }

    std::cout << gameIdSum << endl;
}

void part2(istream& inputFile) {
    vector<Game> games;

    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        smatch match;
        if (!regex_match(line, match, GAME_RX)) {
            std::cout << "GAME_RX failed for line: { " << line << " } " << endl;
            continue;
        }

        stringstream gameIdStr(match[1].str());
        size_t gameId;
        gameIdStr >> gameId;
        Game game = {
            .id = gameId,
        };

        string records = match[2].str();
        // std::cout << "DEBUG Game " << gameId << " records: " << records << endl;
        const auto splitRecords = aoc::util::string::split(records, ';');
        for (const auto &pullRecord : splitRecords) {
            // std::cout << "DEBUG pullRecord: (" << pullRecord << ")" << endl;

            array<size_t, Color::Count> statCounts = { 0 };
            const auto stats = aoc::util::string::split(pullRecord, ',');
            for (const auto &stat : stats) {
                string trimmedStat = aoc::util::string::trimCopy(stat);
                // std::cout << "DEBUG stat: (" << trimmedStat << ")" << endl;

                smatch statMatch;
                if (!regex_match(trimmedStat, statMatch, STAT_RX)) {
                    std::cout << "STAT_RX failed for stat: { " << trimmedStat << " } " << endl;
                    continue;
                }

                stringstream colorCountStr(statMatch[1].str());
                size_t colorCount;
                colorCountStr >> colorCount;
                const auto colorStr = statMatch[2].str();

                statCounts[kStrToColor.at(colorStr)] = colorCount;
            }

            game.stats.push_back(statCounts);
            // printGame(game);
        }

        games.push_back(game);
    }

    size_t sumOfPowers = 0;
    for (const auto &game : games) {
        // printGame(game);
        array<size_t, Color::Count> maxColorCount = { 0 };
        for (size_t colorIdx = 0; colorIdx < Color::Count; colorIdx++) {
            for (const auto &stat : game.stats) {
                maxColorCount[colorIdx] = max(maxColorCount[colorIdx], stat[colorIdx]);
            }
        }

        float power = 1.f;
        for (size_t colorIdx = 0; colorIdx < Color::Count; colorIdx++) {
            power *= maxColorCount[colorIdx];
        }

        sumOfPowers += power;
    }

    std::cout << sumOfPowers << endl;
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