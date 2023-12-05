#include "../../util/StringUtil.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint>
#include <vector>
#include <map>
#include <regex>
#include <set>
#include <optional>

using namespace std;

regex SEEDS_RX("seeds: (.*)");
regex MAP_RX("([a-z]+)-to-([a-z]+) map:");

const string kSeed = "seed";
const string kSoil = "soil";
const string kFertilizer = "fertilizer";
const string kWater = "water";
const string kLight = "light";
const string kTemperature = "temperature";
const string kHumidity = "humidity";
const string kLocation = "location";

struct Interval {
    size_t start;
    size_t end;
};

struct MappingInfo {
    size_t srcStart;
    size_t srcEnd;
    int64_t delta;
    size_t dstStart;
    size_t dstEnd;
};

typedef map<size_t, MappingInfo> map_id_info_t;

struct Mappings {
    vector<size_t> seedIds;
    map_id_info_t seedToSoil;
    map_id_info_t soilToFertilizer;
    map_id_info_t fertilizerToWater;
    map_id_info_t waterToLight;
    map_id_info_t lightToTemperature;
    map_id_info_t temperatureToHumidity;
    map_id_info_t humidityToLocation;
};

void _printInterval(const Interval &interval) {
    std::cout << "[ " << interval.start << ", " << interval.end << " ]";
}

void _printMappings(const Mappings &mappings) {
    std::cout << "seeds: ";
    for (const auto seedId : mappings.seedIds) {
        std:: cout << seedId << " ";
    }
    std::cout << endl << endl;

    std::cout << "seed-to-soil map:" << endl;
    for (const auto &m : mappings.seedToSoil) {
        const auto mi = m.second;
        cout << "start: " << mi.srcStart << " end: " << mi.srcEnd << " delta: " << mi.delta << endl;
    }
    std::cout << endl;

    std::cout << "soil-to-fertilizer map:" << endl;
    for (const auto &m : mappings.soilToFertilizer) {
        const auto mi = m.second;
        cout << "start: " << mi.srcStart << " end: " << mi.srcEnd << " delta: " << mi.delta << endl;
    }
    std::cout << endl;

    std::cout << "fertilizer-to-water:" << endl;
    for (const auto &m : mappings.fertilizerToWater) {
        const auto mi = m.second;
        cout << "start: " << mi.srcStart << " end: " << mi.srcEnd << " delta: " << mi.delta << endl;
    }
    std::cout << endl;

        std::cout << "water-to-light map:" << endl;
    for (const auto &m : mappings.waterToLight) {
        const auto mi = m.second;
        cout << "start: " << mi.srcStart << " end: " << mi.srcEnd << " delta: " << mi.delta << endl;
    }
    std::cout << endl;

        std::cout << "light-to-temperature map:" << endl;
    for (const auto &m : mappings.lightToTemperature) {
        const auto mi = m.second;
        cout << "start: " << mi.srcStart << " end: " << mi.srcEnd << " delta: " << mi.delta << endl;
    }
    std::cout << endl;

        std::cout << "temperature-to-humidity map:" << endl;
    for (const auto &m : mappings.temperatureToHumidity) {
        const auto mi = m.second;
        cout << "start: " << mi.srcStart << " end: " << mi.srcEnd << " delta: " << mi.delta << endl;
    }
    std::cout << endl;

        std::cout << "humidity-to-location map:" << endl;
    for (const auto &m : mappings.humidityToLocation) {
        const auto mi = m.second;
        cout << "start: " << mi.srcStart << " end: " << mi.srcEnd << " delta: " << mi.delta << endl;
    }
    std::cout << endl;
}

map_id_info_t& getMappingFromString(const string &source, Mappings &mappings) {
    if (source == kSeed) {
        return mappings.seedToSoil;
    } else if (source == kSoil) {
        return mappings.soilToFertilizer;
    } else if (source == kFertilizer) {
        return mappings.fertilizerToWater;
    } else if (source == kWater) {
        return mappings.waterToLight;
    } else if (source == kLight) {
        return mappings.lightToTemperature;
    } else if (source == kTemperature) {
        return mappings.temperatureToHumidity;
    } else {
        return mappings.humidityToLocation;
    }
}

map_id_info_t& parseMappings(istream& inputFile, map_id_info_t &mapping) {
    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        if (line.size() == 0) {
            break;
        }

        const auto mapValues = aoc::util::string::parseDelimSeparatedNumbers(line, ' ');
        if (mapValues.size() != 3) {
            std::cout << "Mapping not formatted properly: { " << line << " }" << endl;
            exit(1);
        }

        const auto dstRangeStart = mapValues[0];
        const auto srcRangeStart = mapValues[1];
        const auto rangeLength = mapValues[2];

        // a value x will be mapped according to the function:
        // f(x) = dstRangeStart + (x - srcRangeStart) ===
        // f(x) = x + (dstRangeStart - srcRangeStart) ===
        // f(x) = x + delta, where delta = dstRangeStart - srcRangeStart
        const int64_t delta = dstRangeStart - srcRangeStart;

        mapping[srcRangeStart] = {
            .srcStart = srcRangeStart,                      // inclusive
            .srcEnd = srcRangeStart + rangeLength - 1,      // inclusive
            .delta = delta,
            .dstStart = dstRangeStart,                      // inclusive
            .dstEnd = dstRangeStart + rangeLength - 1,      // inclusive
        };
    }

    return mapping;
}

const Mappings parseInput(istream& inputFile) {
    Mappings mappings;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        if (line.size() == 0) {
            continue;
        }

        smatch seedsMatch;
        if (regex_match(line, seedsMatch, SEEDS_RX)) {
            mappings.seedIds = aoc::util::string::parseDelimSeparatedNumbers(seedsMatch[1].str(), ' ');
            continue;
        }

        smatch mappingMatch;
        if (regex_match(line, mappingMatch, MAP_RX)) {
            auto &mapping = getMappingFromString(mappingMatch[1].str(), mappings);
            parseMappings(inputFile, mapping);
            continue;
        }
    }

    return mappings;
}

size_t getMapping(const size_t id, const map_id_info_t &mapping) {
    if (mapping.contains(id)) {
        return id + mapping.at(id).delta;
    }

    // get iterator to the last key that is less than our id
    map_id_info_t::const_iterator it = mapping.upper_bound(id);
    // if no such key exists, it means that we do not have a special mapping for this id
    if (it == mapping.begin()) {
        // so we return its identity mapping
        return id;
    } else {
        it--;
    }

    if (it->second.srcStart <= id && id <= it->second.srcEnd) {
        // we found its mapping interval
        return id + it->second.delta;
    } else {
        // it does not have a mapping interval
        return id;
    }

    return id;
}

vector<Interval> getMappedIntervals(const vector<Interval> &srcIntervals, const map_id_info_t &dstMapping) {
    vector<Interval> mappedIntervals;
    for (const auto &srcInterval : srcIntervals) {
        // get iterator to the last key that is less than our id
        map_id_info_t::const_iterator it = dstMapping.upper_bound(srcInterval.start);

        // our interval starts before all the mapping intervals
        if (it == dstMapping.begin()) {
            // identity mapping
            mappedIntervals.push_back({
                .start = srcInterval.start,
                .end = min(srcInterval.end, it->second.srcStart - 1),
            });

            if (srcInterval.end < it->second.srcStart) {
                continue;
            }
        } else {
            auto prevIt = it;
            prevIt--;
            if (srcInterval.start <= prevIt->second.srcEnd) {
                it = prevIt;
            // } else if (it == dstMapping.end()) {
            //     // we are at the end of the intervals, add the full source interval and move on
            //     mappedIntervals.push_back({
            //         .start = srcInterval.start,
            //         .end = srcInterval.end,
            //     });

            //     continue;
            } else {
                mappedIntervals.push_back({
                    .start = srcInterval.start,
                    .end = min(srcInterval.end, it->second.srcStart - 1),
                });

                if (srcInterval.end < it->second.srcStart) {
                    continue;
                }
            }
        }

        // find out where does the end of our source interval is
        while (it != dstMapping.end()) {
            if (srcInterval.end <= it->second.srcEnd) {
                mappedIntervals.push_back({
                    .start = max(srcInterval.start, it->second.srcStart) + it->second.delta,
                    .end = srcInterval.end + it->second.delta,
                });
                break;
            }

            // our source interval ends in between 2 mapped intervals, so push the "in between" indices
            // as identity interval
            if (srcInterval.end < it->second.srcStart) {
                auto prevIt = it;
                prevIt--;
                mappedIntervals.push_back({
                    .start = prevIt->second.srcEnd + 1,
                    .end = srcInterval.end,
                });
            }

            mappedIntervals.push_back({
                .start = max(srcInterval.start, it->second.srcStart) + it->second.delta,
                .end = it->second.dstEnd,
            });

            it++;
        }

        // we are at the end, push the remaining indices as identity interval
        if (it == dstMapping.end()) {
            auto prevIt = it;
            prevIt--;
            mappedIntervals.push_back({
                .start = prevIt->second.srcEnd + 1,
                .end = srcInterval.end,
            });
        }
    }

    return mappedIntervals;
}

void part1(const Mappings &mappings) {
    size_t minLocation = INT_MAX;
    for (const auto seedId : mappings.seedIds) {
        const auto soilId = getMapping(seedId, mappings.seedToSoil);
        const auto fertilizerId = getMapping(soilId, mappings.soilToFertilizer);
        const auto waterId = getMapping(fertilizerId, mappings.fertilizerToWater);
        const auto lightId = getMapping(waterId, mappings.waterToLight);
        const auto temperatureId = getMapping(lightId, mappings.lightToTemperature);
        const auto humidityId = getMapping(temperatureId, mappings.temperatureToHumidity);
        const auto locationId = getMapping(humidityId, mappings.humidityToLocation);
        minLocation = min(minLocation, locationId);
    }

    cout << minLocation << endl;
}

void part2_Legacy(const Mappings &mappings) {
    size_t minLocation = INT_MAX;

    for (size_t i = 0; i < mappings.seedIds.size() - 1; i += 2) {
        const auto seedIdStart = mappings.seedIds[i];
        const auto seedIdEnd = seedIdStart + mappings.seedIds[i + 1];

        for (size_t seedId = seedIdStart; seedId < seedIdEnd; seedId++) {
            const auto soilId = getMapping(seedId, mappings.seedToSoil);
            const auto fertilizerId = getMapping(soilId, mappings.soilToFertilizer);
            const auto waterId = getMapping(fertilizerId, mappings.fertilizerToWater);
            const auto lightId = getMapping(waterId, mappings.waterToLight);
            const auto temperatureId = getMapping(lightId, mappings.lightToTemperature);
            const auto humidityId = getMapping(temperatureId, mappings.temperatureToHumidity);
            const auto locationId = getMapping(humidityId, mappings.humidityToLocation);
            minLocation = min(minLocation, locationId);
        }
    }

    cout << minLocation << endl;
}

void part2(const Mappings &mappings) {
    vector<map_id_info_t> transformMappings = {
        mappings.seedToSoil,
        mappings.soilToFertilizer,
        mappings.fertilizerToWater,
        mappings.waterToLight,
        mappings.lightToTemperature,
        mappings.temperatureToHumidity,
        mappings.humidityToLocation,
    };

    vector<Interval> intervals;
    for (size_t i = 0; i < mappings.seedIds.size(); i += 2) {
        intervals.push_back({
            .start = mappings.seedIds[i],
            .end = mappings.seedIds[i] + mappings.seedIds[i + 1] - 1,
        });
    }

    for (const auto &transformMapping : transformMappings) {
        intervals = getMappedIntervals(intervals, transformMapping);
    }

    size_t minLocation = INT_MAX;
    for (const auto &interval : intervals) {
        minLocation = min(minLocation, interval.start);
    }

    std::cout << minLocation << endl;
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        std::cout << "Must specify input file!" << endl;
        return 0;
    }

    std::cout << "Input file name: " << argv[1] << endl;
    ifstream inputFile(argv[1]);

    const auto mappings = parseInput(inputFile);
    // _printMappings(mappings);

    std::cout << "Part 1: " << endl;
    part1(mappings);

    inputFile.clear();
    inputFile.seekg(0, ios::beg);

    std::cout << "Part 2: " << endl;
    part2(mappings);

    inputFile.close();

    return 0;
}