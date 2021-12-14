#include <iostream>
#include <fstream>
#include <vector>
#include <regex>
#include <unordered_map>
#include <algorithm>
#include <utility>
#include <cmath>
#include <tuple>

using namespace std;

#define NUM_STEPS_PART_1 10
#define NUM_STEPS_PART_2 40

typedef pair<char, char> duplex_element_t;

struct duplex_element_t_equal
{
    bool operator()(const duplex_element_t& e1, const duplex_element_t& e2) const
    {
        return (e1.first == e2.first) && (e1.second == e2.second);
    }
};

struct duplex_element_t_hash
{
    size_t operator()(const duplex_element_t& e) const
    {
        return e.first ^ e.second;
    }
};

typedef unordered_map<duplex_element_t, char, duplex_element_t_hash, duplex_element_t_equal> pair_map_t;

typedef tuple<size_t, char, char> cache_key_t;

struct cache_key_t_equal
{
    bool operator()(const cache_key_t& k1, const cache_key_t& k2) const
    {
        return (get<0>(k1) == get<0>(k2)) && (get<1>(k1) == get<1>(k2)) && (get<2>(k1) == get<2>(k2));
    }
};

struct cache_key_t_hash
{
    size_t operator()(const cache_key_t& k) const
    {
        return get<0>(k) ^ get<1>(k) ^ get<2>(k);
    }
};

typedef unordered_map<char, size_t> element_counts_t;
typedef unordered_map<cache_key_t, element_counts_t, cache_key_t_hash, cache_key_t_equal> iterations_counts_t;


const vector<duplex_element_t> getDuplexElements(const vector<char>& polymer)
{
    const auto numDuplexes = polymer.size() - 1;
    vector<duplex_element_t> duplexes(numDuplexes);
    for (size_t i = 0; i < numDuplexes; ++i)
    {
        duplexes[i] = {polymer[i], polymer[i + 1]};
    }

    return duplexes;
}

void runPolyGrow(
    const string& polyTemplate,
    const pair_map_t& pairMap,
    element_counts_t& elementCounts,
    const size_t kNumIterations)
{
    vector<char> currentPoly(polyTemplate.begin(), polyTemplate.end());

    // initialize element count with the polymer template elements
    for (const auto& element : currentPoly)
    {
        const auto& it = elementCounts.find(element);
        if (it == elementCounts.end())
        {
            elementCounts.insert({element, 0});
        }

        elementCounts[element] += 1;
    }

    // cout << "Template: \t\t\t" << string(currentPoly.begin(), currentPoly.end()) << endl;

    size_t iteration = 0;
    while (iteration < kNumIterations)
    {
        const auto duplexes = getDuplexElements(currentPoly);
        const auto newPolySize = currentPoly.size() + duplexes.size();
        cout << newPolySize << endl;
        vector<char> newPoly(newPolySize);
        size_t i = 0;
        for (size_t duplexId = 0; duplexId < duplexes.size(); ++duplexId)
        {
            const auto duplex = duplexes[duplexId];
            const auto insertionElement = pairMap.at(duplex);
            newPoly[i] = duplex.first;
            newPoly[i + 1] = insertionElement;

            if (duplexId == duplexes.size() - 1)
            {
                newPoly[i + 2] = duplex.second;
            }

            // update element count
            const auto& it = elementCounts.find(insertionElement);
            if (it == elementCounts.end())
            {
                elementCounts.insert({insertionElement, 0});
            }

            elementCounts[insertionElement] += 1;

            i += 2;
        }

        currentPoly = newPoly;

        iteration += 1;

        // cout << "After step " << iteration << ": \t\t\t" << string(currentPoly.begin(), currentPoly.end()) << endl;
    }
}


void mergeDicts(const element_counts_t& elemCountsSrc, element_counts_t& elemCountsDst)
{
    if (elemCountsSrc.empty())
    {
        return;
    }

    for (const auto& elemCounts : elemCountsSrc)
    {
        const auto k = elemCounts.first;
        const auto v = elemCounts.second;

        const auto& it = elemCountsDst.find(k);
        if (it == elemCountsDst.end())
        {
            elemCountsDst.insert({k, v});
        }
        else
        {
            it->second += v;
        }
    }
}

const element_counts_t getCountsForIterations(
    const duplex_element_t& duplexElement,
    const pair_map_t& pairMap,
    iterations_counts_t& iterationsCountsCache,
    const size_t kRemainingIterations)
{
    const auto cacheKey = cache_key_t{kRemainingIterations, duplexElement.first, duplexElement.second};

    // check if we have this value pre-calculated, so we can exit fast
    const auto& it = iterationsCountsCache.find(cacheKey);
    if (it != iterationsCountsCache.end())
    {
        return it->second;
    }

    // base case, when we can simply calculate and return directly
    if (kRemainingIterations == 0)
    {
        element_counts_t result;
        result[duplexElement.first] += 1;
        result[duplexElement.second] += 1;

        return result;
    }

    // expand the current duplex and calculate the counts for its corresponding parts, recursively
    const auto insertionElement = pairMap.at(duplexElement);
    const auto leftDuplex = duplex_element_t{duplexElement.first, insertionElement};
    const auto rightDuplex = duplex_element_t{insertionElement, duplexElement.second};

    const auto leftCounts = getCountsForIterations(leftDuplex, pairMap, iterationsCountsCache, kRemainingIterations - 1);
    const auto rightCounts = getCountsForIterations(rightDuplex, pairMap, iterationsCountsCache, kRemainingIterations - 1);

    auto elementCounts = leftCounts;
    mergeDicts(rightCounts, elementCounts);
    elementCounts[insertionElement] -= 1; // we double counted the middle element

    // update cache
    iterationsCountsCache.insert({cacheKey, elementCounts});

    return elementCounts;
}

void runPolyGrowFast(
    const string& polyTemplate,
    const pair_map_t& pairMap,
    element_counts_t& elementCounts,
    const size_t kNumIterations)
{
    const auto polymerVect = vector<char>(polyTemplate.begin(), polyTemplate.end());
    iterations_counts_t iterationsCountsCache;
    const auto duplexes = getDuplexElements(polymerVect);
    for (size_t i = 0; i < duplexes.size(); ++i)
    {
        const auto& duplex = duplexes[i];
        const auto resultCounts = getCountsForIterations(duplex, pairMap, iterationsCountsCache, kNumIterations);
        mergeDicts(resultCounts, elementCounts);
        if (i != duplexes.size() - 1)
        {
            elementCounts[duplex.second] -= 1; // subtract the element that will be double counted by the next duplex
        }
    }

    // DEBUG
    // for (const auto& elemCounts : elementCounts)
    // {
    //     cout << elemCounts.first << ": " << elemCounts.second << endl;
    // }
}

void part1(istream& inputFile)
{
    const regex rx("([A-Z]+) -> ([A-Z]+)");

    string polyTemplate = "";
    pair_map_t pairMap;
    unordered_map<char, size_t> elementCounts;

    bool readTemplate = false;
    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        if (!readTemplate)
        {
            polyTemplate = line;
            readTemplate = true;
        }
        else
        {
            if (line.empty())
            {
                continue;
            }

            smatch match;
            if (regex_match(line, match, rx))
            {
                const auto a = match[1].str().at(0);
                const auto b = match[1].str().at(1);
                const auto c = match[2].str().at(0);

                const auto duplex = duplex_element_t{a, b};
                pairMap.insert({duplex, c});
            }
            else
            {
                cout << "Regex failed for line: " << line << endl;
                return;
            }
        }
    }

    runPolyGrowFast(polyTemplate, pairMap, elementCounts, NUM_STEPS_PART_1);
    const auto minOccurences = min_element(
        elementCounts.begin(),
        elementCounts.end(),
        [](const auto& e1, const auto& e2)
        {
            return e1.second < e2.second;
        });
    const auto maxOccurences = max_element(
        elementCounts.begin(),
        elementCounts.end(),
        [](const auto& e1, const auto& e2)
        {
            return e1.second < e2.second;
        });

    const auto result = maxOccurences->second - minOccurences->second;
    cout << result << endl;
}

void part2(istream& inputFile)
{
    const regex rx("([A-Z]+) -> ([A-Z]+)");

    string polyTemplate = "";
    pair_map_t pairMap;
    unordered_map<char, size_t> elementCounts;

    bool readTemplate = false;
    while (!inputFile.eof())
    {
        string line;
        getline(inputFile, line);

        if (!readTemplate)
        {
            polyTemplate = line;
            readTemplate = true;
        }
        else
        {
            if (line.empty())
            {
                continue;
            }

            smatch match;
            if (regex_match(line, match, rx))
            {
                const auto a = match[1].str().at(0);
                const auto b = match[1].str().at(1);
                const auto c = match[2].str().at(0);

                const auto duplex = duplex_element_t{a, b};
                pairMap.insert({duplex, c});
            }
            else
            {
                cout << "Regex failed for line: " << line << endl;
                return;
            }
        }
    }

    runPolyGrowFast(polyTemplate, pairMap, elementCounts, NUM_STEPS_PART_2);
    const auto minOccurences = min_element(
        elementCounts.begin(),
        elementCounts.end(),
        [](const auto& e1, const auto& e2)
        {
            return e1.second < e2.second;
        });
    const auto maxOccurences = max_element(
        elementCounts.begin(),
        elementCounts.end(),
        [](const auto& e1, const auto& e2)
        {
            return e1.second < e2.second;
        });

    const auto result = maxOccurences->second - minOccurences->second;
    cout << result << endl;
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