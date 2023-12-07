#include "../../util/StringUtil.h"
#include "../../util/NumbersUtil.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint>
#include <vector>
#include <regex>
#include <unordered_map>
#include <array>
#include <set>
#include <utility>

using namespace std;

regex HAND_RX("([AKQJT2-9]+) ([0-9]+)");

const size_t kDeckSize = 13;
const char kJokerCard = 'J';

const vector<char> cardLabels = {
    '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A',
};

const map<char, size_t> cardRanking = {
    {'2', 0},
    {'3', 1},
    {'4', 2},
    {'5', 3},
    {'6', 4},
    {'7', 5},
    {'8', 6},
    {'9', 7},
    {'T', 8},
    {'J', 9},
    {'Q', 10},
    {'K', 11},
    {'A', 12},
};

const map<char, size_t> cardRankingJoker = {
    {'J', 0},
    {'2', 1},
    {'3', 2},
    {'4', 3},
    {'5', 4},
    {'6', 5},
    {'7', 6},
    {'8', 7},
    {'9', 8},
    {'T', 9},
    {'Q', 10},
    {'K', 11},
    {'A', 12},
};

enum HandType {
    None = 0,
    HighCard,
    OnePair,
    TwoPair,
    ThreeOfKind,
    FullHouse,
    FourOfKind,
    FiveOfKind,
};

struct Hand {
    string cards;
    size_t bid;
};

struct HandComparator {
    bool operator() (const Hand &lhs, const Hand &rhs) const {
        for (size_t i = 0; i < lhs.cards.size(); i++) {
            const auto lCard = lhs.cards[i];
            const auto rCard = rhs.cards[i];

            const auto lRanking = cardRanking.at(lCard);
            const auto rRanking = cardRanking.at(rCard);
            if (lRanking == rRanking) {
                continue;
            }

            return lRanking < rRanking;
        }

        return true;
    }
};

struct HandComparatorJoker {
    bool operator() (const Hand &lhs, const Hand &rhs) const {
        for (size_t i = 0; i < lhs.cards.size(); i++) {
            const auto lCard = lhs.cards[i];
            const auto rCard = rhs.cards[i];

            const auto lRanking = cardRankingJoker.at(lCard);
            const auto rRanking = cardRankingJoker.at(rCard);
            if (lRanking == rRanking) {
                continue;
            }

            return lRanking < rRanking;
        }

        return true;
    }
};

vector<Hand> _parseInput(istream& inputFile) {
    vector<Hand> hands;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        smatch handMatch;
        if (!regex_match(line, handMatch, HAND_RX)) {
            cout << "HAND_RX failed to match line: { " << line << " }" << endl;
            exit(1);
        }

        hands.push_back({
            .cards = handMatch[1].str(),
            .bid = static_cast<size_t>(stoul(handMatch[2].str())),
        });
    }

    return hands;
}

HandType _computeHandTypeLegacy(const Hand &hand, optional<char> jokerReplacement = nullopt) {
    array<size_t, kDeckSize> cardCount { 0 };

    for (const auto c : hand.cards) {
        char card = c;
        if (jokerReplacement && c == kJokerCard) {
            card = *jokerReplacement;
        }
        cardCount[cardRanking.at(card)] += 1;
    }

    size_t threeCount = 0;
    size_t twoCount = 0;
    for (const auto count : cardCount) {
        if (count == 5) {
            return FiveOfKind;
        } else if (count == 4) {
            return FourOfKind;
        } else if (count == 3) {
            threeCount += 1;
        } else if (count == 2) {
            twoCount += 1;
        }
    }

    if (threeCount == 1 && twoCount == 1) {
        return FullHouse;
    } else if (threeCount == 1) {
        return ThreeOfKind;
    }

    if (twoCount == 2) {
        return TwoPair;
    }

    if (twoCount == 1) {
        return OnePair;
    }

    return HighCard;
}

HandType _computeHandType(const Hand &hand, bool replaceJoker = false) {
    array<size_t, kDeckSize> cardCount { 0 };

    for (const auto c : hand.cards) {
        cardCount[cardRanking.at(c)] += 1;
    }

    const auto jokerCount = cardCount[cardRanking.at(kJokerCard)];
    if (replaceJoker && jokerCount > 0) {
        size_t maxCount = 0;
        for (size_t i = 0; i < cardCount.size(); i++) {
            if (i == cardRanking.at(kJokerCard)) {
                continue;
            }

            maxCount = max(maxCount, cardCount[i]);
        }

        // it doesn't matter to which we assign here, custom comparator will deal with it later
        for (size_t i = 0; i < cardCount.size() && maxCount > 0; i++) {
            if (i == cardRanking.at(kJokerCard)) {
                continue;
            }

            if (cardCount[i] == maxCount) {
                cardCount[i] += jokerCount;
                cardCount[cardRanking.at(kJokerCard)] = 0;
                break;
            }
        }
    }

    size_t threeCount = 0;
    size_t twoCount = 0;
    for (const auto count : cardCount) {
        if (count == 5) {
            return FiveOfKind;
        } else if (count == 4) {
            return FourOfKind;
        } else if (count == 3) {
            threeCount += 1;
        } else if (count == 2) {
            twoCount += 1;
        }
    }

    if (threeCount == 1 && twoCount == 1) {
        return FullHouse;
    } else if (threeCount == 1) {
        return ThreeOfKind;
    }

    if (twoCount == 2) {
        return TwoPair;
    }

    if (twoCount == 1) {
        return OnePair;
    }

    return HighCard;
}

void part1(const vector<Hand> &hands) {
    map<HandType, set<Hand, HandComparator>> typeToHands;

    for (const auto &hand : hands) {
        const auto type = _computeHandType(hand);
        typeToHands[type].insert(hand);
    }

    int64_t totalWinnings = 0;
    size_t rank = 1;
    for (const auto &p : typeToHands) {
        for (const auto &hand : p.second) {
            totalWinnings += rank * hand.bid;
            rank += 1;
        }
    }

    cout << totalWinnings << endl;
}

void part2Legacy(const vector<Hand> &hands) {
    map<HandType, set<Hand, HandComparatorJoker>> typeToHands;

    for (const auto &hand : hands) {
        HandType strongestType = _computeHandTypeLegacy(hand);
        if (hand.cards.contains(kJokerCard) && strongestType < FiveOfKind) {
            for (const auto card : cardLabels) {
                if (card == kJokerCard) {
                    continue;
                }

                Hand shapeshiftHand = hand;
                replace(shapeshiftHand.cards.begin(), shapeshiftHand.cards.end(), kJokerCard, card);

                const auto type = _computeHandTypeLegacy(shapeshiftHand);
                if (type > strongestType) {
                    strongestType = type;
                    if (strongestType == FiveOfKind) {
                        break;
                    }
                }
            }
        }

        typeToHands[strongestType].insert(hand);
    }

    int64_t totalWinnings = 0;
    size_t rank = 1;
    for (const auto &p : typeToHands) {
        // cout << "Type: " << p.first << endl;
        for (const auto &hand : p.second) {
            // cout << hand.cards << " -> " << rank << endl;
            totalWinnings += rank * hand.bid;
            rank += 1;
        }
        // cout << endl;
    }

    cout << totalWinnings << endl;
}

void part2(const vector<Hand> &hands) {
    map<HandType, set<Hand, HandComparatorJoker>> typeToHands;

    for (const auto &hand : hands) {
        const auto type = _computeHandType(hand, true);
        typeToHands[type].insert(hand);
    }

    int64_t totalWinnings = 0;
    size_t rank = 1;
    for (const auto &p : typeToHands) {
        // cout << "Type: " << p.first << endl;
        for (const auto &hand : p.second) {
            // cout << hand.cards << " -> " << rank << endl;
            totalWinnings += rank * hand.bid;
            rank += 1;
        }
        // cout << endl;
    }

    cout << totalWinnings << endl;
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        std::cout << "Must specify input file!" << endl;
        return 0;
    }

    std::cout << "Input file name: " << argv[1] << endl;
    ifstream inputFile(argv[1]);

    const auto hands = _parseInput(inputFile);

    std::cout << "Part 1: " << endl;
    part1(hands);

    std::cout << "Part 2: " << endl;
    part2Legacy(hands);

    inputFile.close();

    return 0;
}