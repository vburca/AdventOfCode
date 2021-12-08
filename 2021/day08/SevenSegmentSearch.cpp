#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <regex>
#include <unordered_set>
#include <bitset>

using namespace std;

#define NUM_SIGNAL_PATTERNS 10
#define NUM_OUTPUT_DIGITS 4
#define NUM_SEGMENTS 7

static const unordered_map<size_t, size_t> DIGIT_TO_ENCODING_LENGTH({
    {0, 6},
    {1, 2},
    {2, 5},
    {3, 5},
    {4, 4},
    {5, 5},
    {6, 6},
    {7, 3},
    {8, 7},
    {9, 6}
});

bitset<NUM_SEGMENTS> getBitEncoding(const string encoding)
{
    bitset<NUM_SEGMENTS> bitEncoding(0x0);

    for (const auto& ch : encoding)
    {
        bitEncoding.set(ch - 'a', true);
    }

    return bitEncoding;
}

void addDigit(  const size_t digit,
                const bitset<NUM_SEGMENTS> bitEncoding,
                unordered_map<size_t, size_t>& intEncodingToDigit,
                unordered_map<size_t, bitset<NUM_SEGMENTS>>& digitToBitEncoding)
{
    intEncodingToDigit.insert({ bitEncoding.to_ulong(), digit });
    digitToBitEncoding.insert({ digit, bitEncoding });
}

void part1(istream& inputFile)
{
    const unordered_set<size_t> digitsToSearch = {1, 4, 7, 8};
    unordered_set<size_t> uniqueSizes;
    for (const auto& digit : digitsToSearch)
    {
        uniqueSizes.insert(DIGIT_TO_ENCODING_LENGTH.at(digit));
    }

    size_t digitsFoundCount = 0;

    regex rx("([a-z]+) ([a-z]+) ([a-z]+) ([a-z]+) ([a-z]+) ([a-z]+) ([a-z]+) ([a-z]+) ([a-z]+) ([a-z]+) \\| ([a-z]+) ([a-z]+) ([a-z]+) ([a-z]+)");
    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        smatch match;
        if (regex_match(line, match, rx))
        {
            vector<string> signalPatterns(NUM_SIGNAL_PATTERNS, "");
            vector<string> outputValue(NUM_OUTPUT_DIGITS, "");

            for (size_t i = 0; i < NUM_SIGNAL_PATTERNS; ++i)
            {
                signalPatterns[i] = match[i + 1];
            }

            for (size_t i = 0; i < NUM_OUTPUT_DIGITS; ++i)
            {
                outputValue[i] = match[NUM_SIGNAL_PATTERNS + i + 1];
            }

            for (const auto& digitEncoding : outputValue)
            {
                const auto& it = uniqueSizes.find(digitEncoding.size()); // get vector of digits that can be represented by this encoding
                if (it != uniqueSizes.end())
                {
                    digitsFoundCount += 1;
                }
            }
        }
        else
        {
            cout << "Regex failed for line: " << line << endl;
            return;
        }
    }

    cout << digitsFoundCount << endl;
}

void part2(istream& inputFile)
{
    size_t sum = 0;

    regex rx("([a-z]+) ([a-z]+) ([a-z]+) ([a-z]+) ([a-z]+) ([a-z]+) ([a-z]+) ([a-z]+) ([a-z]+) ([a-z]+) \\| ([a-z]+) ([a-z]+) ([a-z]+) ([a-z]+)");
    while (!inputFile.eof())
    {
        unordered_map<size_t, size_t> intEncodingToDigit;
        unordered_map<size_t, bitset<NUM_SEGMENTS>> digitToBitEncoding;

        string line;
        getline(inputFile, line);

        smatch match;
        if (regex_match(line, match, rx))
        {
            vector<string> signalPatterns(NUM_SIGNAL_PATTERNS, "");
            vector<string> outputValue(NUM_OUTPUT_DIGITS, "");

            for (size_t i = 0; i < NUM_SIGNAL_PATTERNS; ++i)
            {
                signalPatterns[i] = match[i + 1];
            }

            for (size_t i = 0; i < NUM_OUTPUT_DIGITS; ++i)
            {
                outputValue[i] = match[NUM_SIGNAL_PATTERNS + i + 1];
            }

            // Look for 1, 4, 7, 8
            for (const auto& encoding : signalPatterns)
            {
                const auto bitEncoding = getBitEncoding(encoding);
                size_t digit = 0;
                bool found = true;
                switch(encoding.size())
                {
                    case 2: digit = 1; break; // found 1
                    case 4: digit = 4; break; // found 4
                    case 3: digit = 7; break; // found 7
                    case 7: digit = 8; break; // found 8
                    default: found = false; break;
                }

                if (found)
                {
                    addDigit(digit, bitEncoding, intEncodingToDigit, digitToBitEncoding);
                }
            }

            // Look for 5
            for (const auto& encoding : signalPatterns)
            {
                if (encoding.size() == DIGIT_TO_ENCODING_LENGTH.at(5))
                {
                    // 5 needs to have the corner that for makes (i.e. the lines that 4 has and 1 doesn't have: 4 - 1 = 4 & !1)
                    auto one = digitToBitEncoding[1];
                    const auto& pattern = digitToBitEncoding[4] & one.flip();

                    const auto bitEncoding = getBitEncoding(encoding);
                    if ((bitEncoding & pattern) == pattern)
                    {
                        // we found the specific pattern in the current encoding
                        addDigit(5, bitEncoding, intEncodingToDigit, digitToBitEncoding);
                    }
                }
            }

            // Look for 9
            for (const auto& encoding : signalPatterns)
            {
                if (encoding.size() == DIGIT_TO_ENCODING_LENGTH.at(9))
                {
                    // 9 has everything that 5 UNION 1 have
                    auto pattern = digitToBitEncoding[5] | digitToBitEncoding[1];

                    const auto bitEncoding = getBitEncoding(encoding);
                    if ((bitEncoding & pattern.flip()) == 0)
                    {
                        // we found the specific pattern in the current encoding
                        addDigit(9, bitEncoding, intEncodingToDigit, digitToBitEncoding);
                        break;
                    }
                }
            }

            // Look for 6
            for (const auto& encoding : signalPatterns)
            {
                if (encoding.size() == DIGIT_TO_ENCODING_LENGTH.at(6))
                {
                    // 6 is a 5 with the extra segment that comes from 8 - 9 = 8 & !9
                    auto nine = digitToBitEncoding[9];
                    auto pattern = digitToBitEncoding[8] & nine.flip();
                    pattern = digitToBitEncoding[5] | pattern;

                    const auto bitEncoding = getBitEncoding(encoding);
                    if ((bitEncoding & pattern.flip()) == 0)
                    {
                        // we found the specific pattern in the current encoding
                        addDigit(6, bitEncoding, intEncodingToDigit, digitToBitEncoding);
                        break;
                    }
                }
            }

            // Look for 0
            for (const auto& encoding : signalPatterns)
            {
                if (encoding.size() == DIGIT_TO_ENCODING_LENGTH.at(6))
                {
                    // 0 is the only other encoding of the same length as 6 and 9
                    const auto bitEncoding = getBitEncoding(encoding);
                    if ((bitEncoding != digitToBitEncoding[6]) &&
                        (bitEncoding != digitToBitEncoding[9]))
                    {
                        // we found the specific pattern in the current encoding
                        addDigit(0, bitEncoding, intEncodingToDigit, digitToBitEncoding);
                        break;
                    }
                }
            }

            // Look for 3
            for (const auto& encoding : signalPatterns)
            {
                if (encoding.size() == DIGIT_TO_ENCODING_LENGTH.at(3))
                {
                    // 3 is the only encoding of length 5 that also contains the encoding of 1 inside of it
                    const auto bitEncoding = getBitEncoding(encoding);
                    if ((bitEncoding & digitToBitEncoding[1]) == digitToBitEncoding[1])
                    {
                        // we found the specific pattern in the current encoding
                        addDigit(3, bitEncoding, intEncodingToDigit, digitToBitEncoding);
                        break;
                    }
                }
            }

            // Look for 2
            for (const auto& encoding : signalPatterns)
            {
                if (encoding.size() == DIGIT_TO_ENCODING_LENGTH.at(2))
                {
                    // 2 is the only other encoding of the same length as 3 and 5
                    const auto bitEncoding = getBitEncoding(encoding);
                    if ((bitEncoding != digitToBitEncoding[3]) &&
                        (bitEncoding != digitToBitEncoding[5]))
                    {
                        // we found the specific pattern in the current encoding
                        addDigit(2, bitEncoding, intEncodingToDigit, digitToBitEncoding);
                        break;
                    }
                }
            }

            // Now parse the output value
            size_t number = 0;
            for (const auto& encoding : outputValue)
            {
                const auto bitEncoding = getBitEncoding(encoding);
                number = number * 10 + intEncodingToDigit[bitEncoding.to_ulong()];
            }

            sum += number;
        }
        else
        {
            cout << "Regex failed for line: " << line << endl;
            return;
        }
    }

    cout << sum << endl;
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