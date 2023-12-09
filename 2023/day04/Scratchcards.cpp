#include "../../util/StringUtil.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint>
#include <vector>
#include <regex>
#include <set>
#include <optional>
#include <queue>
#include <math.h>

using namespace std;

regex CARD_RX("Card\\s+([0-9]+): (.*)");
regex NUMBERS_RX("(.*) | (.*)");

struct CardInformation {
    size_t id;
    set<size_t> winningNumbers;
    set<size_t> ourNumbers;
    optional<size_t> ourWinningNumbersCount;
};

optional<CardInformation> parseCardLine(const string &cardLine) {
    smatch gameMatch;
    if (!regex_match(cardLine, gameMatch, CARD_RX)) {
        std::cout << "CARD_RX failed for line: { " << cardLine << " }" << endl;
        return nullopt;
    }

    stringstream cardIdStr(gameMatch[1].str());
    size_t cardId;
    cardIdStr >> cardId;

    string numbersLine = gameMatch[2].str();

    const auto numberLists = aoc::util::string::split(numbersLine, '|');
    if (numberLists.size() != 2) {
        std::cout << "Failed to split number lists from line: { " << numbersLine << " }" << endl;
        return nullopt;
    }

    const auto winningNumbersStr = aoc::util::string::trimCopy(numberLists[0]);
    const auto ourNumbersStr = aoc::util::string::trimCopy(numberLists[1]);

    const auto winningNumbers = aoc::util::string::parseDelimSeparatedNumbers<size_t>(winningNumbersStr, ' ');
    const auto ourNumbers = aoc::util::string::parseDelimSeparatedNumbers<size_t>(ourNumbersStr, ' ');

    return CardInformation {
        .id = cardId - 1,
        .winningNumbers = set<size_t>(winningNumbers.begin(), winningNumbers.end()),
        .ourNumbers = set<size_t>(ourNumbers.begin(), ourNumbers.end()),
    };
}

void _printCards(const vector<CardInformation> &cards) {
    for (const auto &card : cards) {
        std::cout << "Card " << card.id + 1 << ": ";
        for (const auto &winningNumber : card.winningNumbers) {
            std::cout << winningNumber << " ";
        }
        std::cout << "| ";
        for (const auto &ourNumber : card.ourNumbers) {
            std::cout << ourNumber << " ";
        }
        std::cout << endl;
    }
}

size_t getWinningNumberCount(const CardInformation &card) {
    size_t winningNumberCount = 0;
    for (const auto &ourNumber : card.ourNumbers) {
        if (card.winningNumbers.contains(ourNumber)) {
            winningNumberCount += 1;
        }
    }

    return winningNumberCount;
}

void part1(istream& inputFile) {
    vector<CardInformation> cards;

    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        const auto cardInformation = parseCardLine(line);
        if (!cardInformation) {
            continue;
        }

        cards.push_back(*cardInformation);
    }

    // _printCards(cards);
    size_t totalWorth = 0;
    for (const auto &cardInfo : cards) {
        size_t winningNumberCount = getWinningNumberCount(cardInfo);

        if (winningNumberCount > 0) {
            totalWorth += pow(2, winningNumberCount - 1);
        }
    }

    cout << totalWorth << endl;
}

void part2(istream& inputFile) {
    vector<CardInformation> cards;

    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        const auto cardInformation = parseCardLine(line);
        if (!cardInformation) {
            continue;
        }

        cards.push_back(*cardInformation);
    }

    // _printCards(cards);

    queue<size_t> cardIdsToBeProcessed;
    for (const auto &card : cards) {
        cardIdsToBeProcessed.push(card.id);
    }

    size_t totalCardsProcessed = 0;
    while (!cardIdsToBeProcessed.empty()) {
        const size_t cardId = cardIdsToBeProcessed.front();
        cardIdsToBeProcessed.pop();

        if (cardId > cards.size()) {
            std::cout << "Trying to process card id (" << cardId << ") that is greater than total number of cards (" << cards.size() << ")" << endl;
            continue;
        }

        auto &card = cards.at(cardId);

        // check if we pre-calculated its winning count
        if (!card.ourWinningNumbersCount) {
            // calculate / cache the winning count
            card.ourWinningNumbersCount = getWinningNumberCount(card);
        }

        // schedule the next cards to be processed
        for (size_t i = 1; i <= card.ourWinningNumbersCount; i++) {
            cardIdsToBeProcessed.push(cardId + i);
        }

        totalCardsProcessed += 1;
    }

    cout << totalCardsProcessed << endl;
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