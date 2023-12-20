#include "../../util/StringUtil.h"
#include "../../util/MathUtil.h"
#include "../../util/Types.h"

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
#include <optional>

using namespace std;
using namespace aoc::util::types;

constexpr size_t kButtonPresses = 1000;

regex ASSIGNMENT_RX("(.*) -> (.*)");
regex MODULE_TYPE_RX("([%&])([a-z]+)");

struct Pulse {
    enum Type {
        Low = 0,
        High
    };

    Type type;
    string srcName;
    string dstName;
};

struct Module {
    enum Type {
        None = 0,
        Output,
        Broadcast,
        FlipFlop,
        Conjunction,
    };

    Type type = Type::None;
    string name;
    vector<string> srcNames;
    vector<string> dstNames;
    bool flipFlopState = false; // initially OFF
    map<string, Pulse::Type> srcNameToPulseType;

    // used for part 2
    bool hasSentHighPulse;

    // used for part 2, in case the source of the desired output module
    // is not a Conjunction, in which case we will just run a simulation
    bool outputActivated = false;

    void initialize() {
        switch (type) {
            case Conjunction: return _initializeConjunction();
            default: return;
        }
    }

    vector<Pulse> processPulse(const Pulse &incomingPulse) {
        switch (type) {
            case Broadcast: return _processPulseBroadcast(incomingPulse);
            case FlipFlop: return _processPulseFlipFlop(incomingPulse);
            case Conjunction: return _processPulseConjunction(incomingPulse);
            case Output: return _processPulseOutput(incomingPulse);
            case None: return vector<Pulse>();
        }

        cout << "Unknown type: " << type << endl;
        exit(1);
    }

    bool isDefaultState() const {
        switch (type) {
            case Broadcast: return true;
            case FlipFlop: return _isDefaultStateFlipFlop();
            case Conjunction: return true;
            case Output: return true;
            case None: return true;
        }

        cout << "Unknown type: " << type << endl;
        exit(1);
    }

    static Module::Type _getType(const char ch) {
        switch (ch) {
            case '%': return Module::Type::FlipFlop;
            case '&': return Module::Type::Conjunction;
            default : {
                cout << "Unknown module type identifier: " << ch << endl;
                exit(1);
            }
        }

        return Module::Type::Broadcast;
    }

private:
    vector<Pulse> _processPulseBroadcast(const Pulse &incomingPulse) {
        hasSentHighPulse = hasSentHighPulse || (incomingPulse.type == Pulse::Type::High);
        return _broadcastPulseType(incomingPulse.type);
    }

    // Flip-flop modules (prefix %) are either on or off; they are initially off.
    // If a flip-flop module receives a high pulse, it is ignored and nothing happens.
    // However, if a flip-flop module receives a low pulse, it flips between on and off.
    // If it was off, it turns on and sends a high pulse. If it was on, it turns off and
    // sends a low pulse.
    vector<Pulse> _processPulseFlipFlop(const Pulse &incomingPulse) {
        if (incomingPulse.type == Pulse::Type::High) {
            return vector<Pulse>();
        }

        flipFlopState = !flipFlopState; // flip state

        const auto pulseTypeToSend = flipFlopState ? Pulse::Type::High : Pulse::Type::Low;
        hasSentHighPulse = hasSentHighPulse || (pulseTypeToSend == Pulse::Type::High);
        return _broadcastPulseType(pulseTypeToSend);
    }

    // Conjunction modules (prefix &) remember the type of the most recent pulse received
    // from each of their connected input modules; they initially default to remembering a
    // low pulse for each input. When a pulse is received, the conjunction module first updates
    // its memory for that input. Then, if it remembers high pulses for all inputs, it sends a
    // low pulse; otherwise, it sends a high pulse.
    vector<Pulse> _processPulseConjunction(const Pulse &incomingPulse) {
        // update memory for the incoming pulse
        srcNameToPulseType[incomingPulse.srcName] = incomingPulse.type;

        const auto shouldSendLow = _checkAllSrcAreHigh();
        const auto pulseTypeToSend = shouldSendLow ? Pulse::Type::Low : Pulse::Type::High;
        hasSentHighPulse = hasSentHighPulse || (pulseTypeToSend == Pulse::Type::High);

        return _broadcastPulseType(pulseTypeToSend);
    }

    vector<Pulse> _processPulseOutput(const Pulse &incomingPulse) {
        if (incomingPulse.type == Pulse::Type::Low) {
            outputActivated = true;
        }

        return vector<Pulse>();
    }

    vector<Pulse> _broadcastPulseType(const Pulse::Type &pulseType) {
        vector<Pulse> outgoingPulses;

        for (const auto &dstName : dstNames) {
            outgoingPulses.emplace_back(Pulse {
                .srcName = name,
                .dstName = dstName,
                .type = pulseType,
            });
        }

        return outgoingPulses;
    }

    void _initializeConjunction() {
        for (const auto &srcName : srcNames) {
            srcNameToPulseType[srcName] = Pulse::Type::Low;
        }
    }

    bool _checkAllSrcAreHigh() {
        for (const auto &p : srcNameToPulseType) {
            if (p.second == Pulse::Type::Low) {
                return false;
            }
        }

        return true;
    }

    bool _isDefaultStateFlipFlop() const {
        return flipFlopState == false;
    }
};

struct Input {
    map<string, Module> nameToModule;

    bool isInitialState() {
        for (const auto &p : nameToModule) {
            const auto &myModule = p.second;
            if (!myModule.isDefaultState()) {
                return false;
            }
        }

        return true;
    }
};

struct PulsesSentState {
    size_t numHighPulses = 0;
    size_t numLowPulses = 0;
};

void _printPulse(const Pulse &pulse) {
    cout << pulse.srcName << " ";
    switch (pulse.type) {
        case Pulse::Type::Low: cout << "-low"; break;
        case Pulse::Type::High: cout << "-high"; break;
        default: {
            cout << "Unknown pulse type: " << pulse.type << endl;
            exit(1);
        }
    }
    cout << "-> " << pulse.dstName;
    cout << endl;
}

void _printInput(const Input &input) {
    for (const auto &p : input.nameToModule) {
        const auto moduleName = p.first;
        const auto myModule = p.second;

        assert(moduleName == myModule.name && "Detected different names");

        if (myModule.type == Module::Type::FlipFlop) {
            cout << "%";
        } else if (myModule.type == Module::Type::Conjunction) {
            cout << "&";
        }
        cout << moduleName << " -> ";

        for (size_t i = 0; i < myModule.dstNames.size(); i++) {
            const auto &dstName = myModule.dstNames[i];
            cout << dstName;

            if (i < myModule.dstNames.size() - 1) {
                cout << ", ";
            }
        }
        if (myModule.srcNames.size() > 0) {
            cout << " \t\t\t(src: ";
            for (size_t i = 0; i < myModule.srcNames.size(); i++) {
                const auto &srcName = myModule.srcNames[i];
                cout << srcName;

                if (i < myModule.srcNames.size() - 1) {
                    cout << ", ";
                }
            }
            cout << ")";
        }
        cout << endl;

        if (myModule.type == Module::Type::Conjunction) {
            assert(myModule.srcNames.size() == myModule.srcNameToPulseType.size());
        }
    }
}

const Input _parseInput(istream& inputFile) {
    Input input;

    while (!inputFile.eof()) {
        string line;
        getline(inputFile, line);

        smatch assignmentMatch;
        if (!regex_match(line, assignmentMatch, ASSIGNMENT_RX)) {
            cout << "ASSIGNMENT_RX failed to match line: { " << line << " }" << endl;
            exit(1);
        }

        const auto srcNode = assignmentMatch[1].str();
        const auto dstList = assignmentMatch[2].str();
        const auto dstNames = aoc::util::string::split(dstList, ',', true);

        string srcName;
        Module::Type moduleType;
        if (srcNode == "broadcaster") {
            srcName = srcNode;
            moduleType = Module::Type::Broadcast;
        } else {
            smatch moduleTypeMatch;
            if (!regex_match(srcNode, moduleTypeMatch, MODULE_TYPE_RX)) {
                cout << "MODULE_TYPE_RX failed to match line: { " << srcNode << " }" << endl;
                exit(1);
            }

            srcName = moduleTypeMatch[2].str();
            moduleType = Module::_getType(moduleTypeMatch[1].str()[0]);
        }

        if (!input.nameToModule.contains(srcName)) {
            // create new module stub
            input.nameToModule[srcName] = Module {
                .name = srcName,
                .srcNames = vector<string>(),
                .dstNames = vector<string>(),
                .type = Module::Type::Output,
            };
        }

        auto &srcModule = input.nameToModule.at(srcName);
        srcModule.type = moduleType;

        // go through all destination modules
        for (const auto &dstName : dstNames) {
            // check if dst module exists already
            if (!input.nameToModule.contains(dstName)) {
                // if it does not exist, create stub for it
                input.nameToModule[dstName] = Module {
                    .name = dstName,
                    .srcNames = vector<string>(),
                    .dstNames = vector<string>(),
                    .type = Module::Type::Output,
                };
            }

            // update the sources of the destination module
            auto &dstModule = input.nameToModule.at(dstName);
            dstModule.srcNames.push_back(srcName);

            // update the destinations of the source module
            srcModule.dstNames.push_back(dstName);
        }
    }

    // add button as a module too
    input.nameToModule["button"] = Module {
        .name = "button",
        .srcNames = vector<string>(),
        .dstNames = vector<string>{ "broadcaster" },
        .type = Module::Type::None,
    };
    input.nameToModule.at("broadcaster").srcNames.push_back("button");

    // initialize all the modules (only Conjunction modules need initialization)
    for (auto &p : input.nameToModule) {
        auto &myModule = p.second;
        myModule.initialize();
    }

    return input;
}

PulsesSentState _sendPulse(const Pulse &startPulse, Input &input) {
    PulsesSentState pulsesSent;

    queue<Pulse> pulsesToProcess;
    pulsesToProcess.push(startPulse);

    while (!pulsesToProcess.empty()) {
        const auto currentPulse = pulsesToProcess.front();
        pulsesToProcess.pop();

        // _printPulse(currentPulse);

        if (!input.nameToModule.contains(currentPulse.dstName)) {
            cout << "Can not handle src module name: " << currentPulse.srcName << endl;
            exit(1);
        }

        // count pulse
        switch (currentPulse.type) {
            case Pulse::Type::Low: pulsesSent.numLowPulses += 1; break;
            case Pulse::Type::High: pulsesSent.numHighPulses += 1; break;
            default: {
                cout << "Unknown pulse type: " << currentPulse.type << endl;
                exit(1);
            }
        }

        const auto pulsesToSend = input.nameToModule.at(currentPulse.dstName).processPulse(currentPulse);
        for (const auto &pulseToSend : pulsesToSend) {
            pulsesToProcess.push(pulseToSend);
        }
    }

    return pulsesSent;
}

void part1(const Input &input) {
    Input inputState = input;
    PulsesSentState periodPulsesCount;
    size_t period = kButtonPresses;

    for (size_t i = 0; i < kButtonPresses; i++) {
        const auto currentPulsesSent = _sendPulse(Pulse {
            .srcName = "button",
            .dstName = "broadcaster",
            .type = Pulse::Type::Low,
        }, inputState);

        periodPulsesCount.numLowPulses += currentPulsesSent.numLowPulses;
        periodPulsesCount.numHighPulses += currentPulsesSent.numHighPulses;

        // check if we finished a cycle
        if (inputState.isInitialState()) {
            period = i + 1;
            break;
        }
    }

    const size_t multiplier = kButtonPresses / period;
    const PulsesSentState totalPulsesCount = {
        .numLowPulses = multiplier * periodPulsesCount.numLowPulses,
        .numHighPulses = multiplier * periodPulsesCount.numHighPulses,
    };

    const auto totalPulses = totalPulsesCount.numLowPulses * totalPulsesCount.numHighPulses;

    cout << totalPulses << endl;
}

uint64_t _runSimulation(const string targetModuleName, Input &input) {
    if (!input.nameToModule.contains(targetModuleName)) {
        cout << "Unknown target module name: " << targetModuleName << endl;
        exit(1);
    }

    const auto &targetModule = input.nameToModule.at(targetModuleName);
    uint64_t iter = 0;
    while (!targetModule.outputActivated) {
        (void)_sendPulse(Pulse {
            .srcName = "button",
            .dstName = "broadcaster",
            .type = Pulse::Type::Low,
        }, input);

        iter += 1;
    }

    return iter;
}

void part2(const Input &input) {
    // NOTE: by inspecting the graph (day20.png), we see that the target module "rx" has a Conjunction
    // parent ("vf"); we know that the Conjunction node only sends a Low pulse when all of its sources
    // sent a High pulse previously; we also notice that all of "vf"'s sources are 4 other Conjunction
    // modules - therefore, in order for these 4 to send High pulse (and activate "vf" to send a Low pulse),
    // these 4 need to _NOT_ have all of their sources send High pulses previously.
    // we will thus track after how many button presses each of these 4 nodes could send a High pulse
    // (i.e. when not all of their sources had previously sent a High pulse)

    uint64_t fewestButtonPresses = 0;

    const auto targetModuleName = "rx";
    const auto targetModule = input.nameToModule.at(targetModuleName);

    // NOTE: just trying to make the solution _somewhat_ general - whenever the source
    // of the target module is not a Conjunction, just run a brute force simulation to
    // figure out the fewest number of button presses before the target module receives
    // a low pulse
    if (targetModule.srcNames.size() == 1 &&
        input.nameToModule.at(targetModule.srcNames[0]).type == Module::Type::Conjunction) {
        const auto &targetRootModule = input.nameToModule.at(targetModule.srcNames[0]);

        vector<uint64_t> cycleLengths = vector<uint64_t>(targetRootModule.srcNames.size(), 0);

        Input inputState = input;

        // run simulation until all the sources of the target's ("rx") root (i.e. the Conjunction
        // module that is the parent of our target module, "rx") have sent a High pulse
        bool allSourcesOfTargetRootSentHighPulse = false;
        size_t cycleLength = 0;
        while (!allSourcesOfTargetRootSentHighPulse) {
            (void)_sendPulse(Pulse {
                .srcName = "button",
                .dstName = "broadcaster",
                .type = Pulse::Type::Low,
            }, inputState);

            cycleLength += 1;

            // assume all have sent a high pulse
            allSourcesOfTargetRootSentHighPulse = true;
            for (size_t i = 0; i < targetRootModule.srcNames.size(); i++) {
                const auto &conjunctionSrc = targetRootModule.srcNames[i];
                auto &conjunctionModule = inputState.nameToModule.at(conjunctionSrc);

                if (!conjunctionModule.hasSentHighPulse) {
                    // we found one that didn't send a high pulse
                    allSourcesOfTargetRootSentHighPulse = false;
                } else if (cycleLengths[i] == 0) {
                    cycleLengths[i] = cycleLength;
                }
            }
        }

        fewestButtonPresses = aoc::util::math::lcm(cycleLengths);
    } else {
        Input inputState = input;
        fewestButtonPresses = _runSimulation(targetModuleName, inputState);
    }


    cout << fewestButtonPresses << endl;
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        std::cout << "Must specify input file!" << endl;
        return 0;
    }

    std::cout << "Input file name: " << argv[1] << endl;
    ifstream inputFile(argv[1]);

    const auto input = _parseInput(inputFile);
    // _printInput(input);

    std::cout << "Part 1: " << endl;
    part1(input);

    std::cout << "Part 2: " << endl;
    part2(input);

    inputFile.close();

    return 0;
}