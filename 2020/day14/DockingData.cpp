#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <utility>
#include <algorithm>
#include <regex>
#include <numeric>
#include <functional>
#include <bitset>
#include <unordered_map>
#include <stack>

using namespace std;

#define MEMORY_SIZE 36

const regex MASK_RX("mask = (.*)");
const regex MEMORY_RX("mem\\[([0-9]+)\\] = ([0-9]+)");

void part1(istream& inputFile)
{
    string rawMask;

    bitset<MEMORY_SIZE> maskOnes;   // have all 0s except at rawMask positions of 1
    bitset<MEMORY_SIZE> maskZeros;  // have all 1s except at rawMask positions of 0

    unordered_map<size_t, uint64_t> memory;
    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        smatch match;
        if (regex_match(line, match, MASK_RX))
        {
            // update mask
            rawMask = match[1].str();

            // reset mask bitsets
            maskOnes.reset();   // all 0s
            maskZeros.reset();
            maskZeros.flip();   // all 1s

            for (size_t i = 0; i < rawMask.size(); ++i)
            {
                const auto& ch = rawMask[i];

                if (ch == '1')
                {
                    maskOnes[i] = 1;
                }
                else if (ch == '0')
                {
                    maskZeros[i] = 0;
                }
            }

            // reverse the masks to have the least significant bit at the beginning
            auto maskStr = maskOnes.to_string();
            reverse(maskStr.begin(), maskStr.end());
            maskOnes = bitset<MEMORY_SIZE>(maskStr);

            maskStr = maskZeros.to_string();
            reverse(maskStr.begin(), maskStr.end());
            maskZeros = bitset<MEMORY_SIZE>(maskStr);
        }
        else if (regex_match(line, match, MEMORY_RX))
        {
            // read initiatlization
            size_t memoryAddr;
            uint64_t memoryValue;

            istringstream ss(match[1].str());
            ss >> memoryAddr;

            ss.clear();
            ss.str(match[2].str());
            ss >> memoryValue;

            uint64_t maskedValue = memoryValue;
            maskedValue = maskedValue | static_cast<uint64_t>(maskOnes.to_ullong());
            maskedValue = maskedValue & static_cast<uint64_t>(maskZeros.to_ullong());

            memory[memoryAddr] = maskedValue;
        }
        else
        {
            cout << "Unknown instruction: " << line << endl;
        }

    }

    const auto sum = accumulate(
        memory.begin(),
        memory.end(),
        static_cast<uint64_t>(0),
        [](const auto& prev, const auto& pair)
        {
            return prev + pair.second;
        });

    cout << sum << endl;
}

void part2(istream& inputFile)
{
    string rawMask;

    bitset<MEMORY_SIZE> maskOnes;   // have all 0s except at rawMask positions of 1

    unordered_map<uint64_t, uint64_t> memory;
    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        smatch match;
        if (regex_match(line, match, MASK_RX))
        {
            // update mask
            rawMask = match[1].str();

            // reset mask bitsets
            maskOnes.reset();   // all 0s

            for (size_t i = 0; i < rawMask.size(); ++i)
            {
                const auto& ch = rawMask[i];

                if (ch == '1')
                {
                    maskOnes[i] = 1;
                }
            }

            // reverse the masks to have the least significant bit at the beginning
            auto maskStr = maskOnes.to_string();
            reverse(maskStr.begin(), maskStr.end());
            maskOnes = bitset<MEMORY_SIZE>(maskStr);
        }
        else if (regex_match(line, match, MEMORY_RX))
        {
            // read initiatlization
            uint64_t memoryAddr;
            uint64_t memoryValue;

            istringstream ss(match[1].str());
            ss >> memoryAddr;

            ss.clear();
            ss.str(match[2].str());
            ss >> memoryValue;

            uint64_t maskedAddr = memoryAddr;
            maskedAddr = maskedAddr | static_cast<uint64_t>(maskOnes.to_ullong());

            bitset<MEMORY_SIZE> maskedAddrBs(maskedAddr);

            stack<bitset<MEMORY_SIZE>> generatedAddresses;
            generatedAddresses.push(maskedAddrBs);
            for (size_t i = 0; i < rawMask.size(); ++i)
            {
                if (rawMask[i] == 'X')
                {
                    stack<bitset<MEMORY_SIZE>> generatedAddressesTemp;
                    while (!generatedAddresses.empty())
                    {
                        const auto address = generatedAddresses.top();
                        generatedAddresses.pop();

                        bitset<MEMORY_SIZE> address0 = address;
                        address0[MEMORY_SIZE - 1 - i] = 0;
                        generatedAddressesTemp.push(address0);

                        bitset<MEMORY_SIZE> address1 = address;
                        address1[MEMORY_SIZE - 1 - i] = 1;
                        generatedAddressesTemp.push(address1);
                    }

                    generatedAddresses.swap(generatedAddressesTemp);
                }
            }

            while (!generatedAddresses.empty())
            {
                const auto address = generatedAddresses.top();
                generatedAddresses.pop();

                memory[static_cast<uint64_t>(address.to_ullong())] = memoryValue;

                // cout << address << " " << memoryValue << endl;
            }
        }
        else
        {
            cout << "Unknown instruction: " << line << endl;
        }

    }

    const auto sum = accumulate(
        memory.begin(),
        memory.end(),
        static_cast<uint64_t>(0),
        [](const auto& prev, const auto& pair)
        {
            return prev + pair.second;
        });

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