#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint>
#include <vector>
#include <regex>
#include <map>
#include <cassert>
#include <queue>
#include <format>
#include <algorithm>
#include <numeric>

#define ALL(x) (x).begin(),(x).end()
#define ALLc(x) (x).cbegin(),(x).cend()
using u64 = unsigned long long;

using namespace std;

enum class ModuleType : char
{
	None,
	FlipFlop,
	Conjunction
};

struct Module
{
	ModuleType type = ModuleType::None;

	bool flipflopOn;
	std::vector<std::string> destinations;
	std::map<std::string, bool> inputs;
};

struct Pulse
{
	std::string from, to;
	bool high;
};

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: AoC23Day20.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << "Could not open inputFilename " << argv[1] << std::endl;
		return -1;
	}

	std::string line, rxInput;
	std::map<std::string, Module> modules;
	std::vector<std::string> conjuctions;
	while (in >> line)
	{
		std::string name = line, out;
		in >> line;
		std::getline(in, line);
		std::stringstream ss(line);

		Module m;

		while (std::getline(ss, out, ','))
			m.destinations.push_back(out.substr(1));

		if (name[0] == '%')
		{
			name = name.substr(1);
			m.type = ModuleType::FlipFlop;
			m.flipflopOn = false;
		}
		else if (name[0] == '&')
		{
			name = name.substr(1);
			m.type = ModuleType::Conjunction;
			conjuctions.push_back(name);
		}
		if (std::find(ALLc(m.destinations), "rx") != m.destinations.cend())
			rxInput = name;
		modules[name] = m;
	}

	// Process conjunctions
	std::sort(ALL(conjuctions));
	for (auto& [name, m] : modules)
		for (const std::string& s : m.destinations)
			if (std::binary_search(ALLc(conjuctions), s))
				modules[s].inputs[name] = false;

	u64 part1 = 0, part2 = 1, lows = 0, highs = 0, pulseCount = 0;
	std::deque<Pulse> pulseQueue;
	std::map<std::string, u64> search;
    cout << "Modules to search for: " << endl;
	for (auto& [input, _] : modules[rxInput].inputs) {
        std::cout << input << " ";
		search[input] = 0;
    }
    std::cout << std::endl;

	auto DoPulse = [&]() ->void {
		pulseQueue.emplace_back(Pulse {
            .from = "button",
            .to = "broadcaster",
            .high = false}); // false low, true high
		while (!pulseQueue.empty())
		{
			Pulse& p = pulseQueue.front();
			if (p.high)
				++highs;
			else
				++lows;
			if (auto iter = modules.find(p.to);
				iter != modules.cend())
			{
                const string &srcName = iter->first;
				Module& m = iter->second;
				switch (m.type)
				{
				case ModuleType::None:
					for (auto& d : m.destinations)
						pulseQueue.emplace_back(Pulse {
                            .from = p.to,
                            .to = d,
                            .high = p.high});
					break;
				case ModuleType::FlipFlop:
					if (!p.high)
					{
						m.flipflopOn = !m.flipflopOn;
						for (auto& d : m.destinations)
							pulseQueue.emplace_back(Pulse {
                                .from = p.to,
                                .to = d,
                                .high = m.flipflopOn});
					}
					break;
				default:
				{
					if (auto iter = m.inputs.find(p.from);
						iter != m.inputs.cend())
						iter->second = p.high;

                    // if (srcName == "pm") {
                    //     cout << pulseCount << " -> " << endl;
                    //     cout << "I am: " << srcName << endl;
                    //     cout << "Inputs: " << endl;
                    // }

					bool allHigh = true;
					for (auto& [name, high] : m.inputs) {
                        if (srcName == "pm" && pulseCount == 3881) {
                            cout << "wtf: " << m.inputs.size() << endl;
                            cout << "(" << name << " -> " << high << ")";
                        }
						allHigh = allHigh && high;
                    }
                    // if (srcName == "pm" && pulseCount == 3881) {
                    //     cout << endl;
                    //     cout << "allHigh: " << allHigh << endl;
                    //     cout << endl << endl;
                    // }

					if (!allHigh) {
						if (auto iter = search.find(p.to);
							iter != search.cend() && iter->second == 0)
						iter->second = pulseCount;

                        if (srcName == "pm") {
                            cout << endl;
                            cout << "Updated search count: " << p.to << " -> " << pulseCount << endl;
                        }
                    }

					for (auto& d : m.destinations)
						pulseQueue.emplace_back(Pulse {
                            .from = p.to,
                            .to = d,
                            .high = !allHigh});
                }
				}
			}
			pulseQueue.pop_front();
		}
	};

	bool hasZero;
	do
	{
		++pulseCount;
		DoPulse();
		if (pulseCount == 1000)
			part1 = lows * highs;

		hasZero = false;
		for (auto& [_, cycle] : search)
			hasZero = hasZero || !cycle;
	} while (hasZero);

    cout << endl;
	for (auto& [name, cycle] : search) {
        std::cout << name << " -> " << cycle << std::endl;
		part2 = std::lcm(part2, cycle);
    }

	std::cout << "Part 1: " << part1 << " Part 2: " << part2 << std::endl;
}