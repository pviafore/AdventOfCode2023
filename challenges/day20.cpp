#include <algorithm>
#include <cassert>
#include <cstdint>
#include <deque>
#include <iostream>
#include <istream>
#include <optional>
#include <ranges>
#include <span>
#include <sstream>
#include <unordered_map>
#include <variant>

#include "common/input.h"

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

struct Broadcaster {

};

struct FlipFlop {
    bool isOn = false;

};

struct Conjunction {
public:

    bool isAllHigh() const{
        return std::ranges::all_of(inputModules, [](auto p){ return p.second; });
    }

    void set(std::string name, bool val) {
        inputModules[name] = val;
        auto index = indexLookup[name];
        if(val) {
            binaryRepresentation = binaryRepresentation | (1 << index);
        }
        else {
            binaryRepresentation = binaryRepresentation & ~(1 << index);
        }
    }

    void setInputModules(std::unordered_map<std::string, bool> lookup) {
        inputModules = lookup;
        for(auto [index, name]: (std::views::enumerate(lookup | std::views::keys))){
            indexLookup[name] = index;
        }
    }

    size_t getBinaryRepresentation() const {
        return binaryRepresentation;
    }

    size_t size() const {
        return inputModules.size();
    }

    void reset() { 
        for( auto p : inputModules) { p.second = false; } 
    }
private:
    std::unordered_map<std::string, bool> inputModules;
    std::unordered_map<std::string, size_t> indexLookup;
    size_t binaryRepresentation = 0;

};

struct Module { 
    std::string name;
    std::variant<Broadcaster, FlipFlop, Conjunction> moduleOption;
    std::vector<std::string> destinations;


    friend std::istream& operator>>(std::istream& is, Module& m){
        std::string line;
        getline(is, line);
        if(line.empty()){
            return is;
        }

        auto arrow = std::ranges::find_first_of(line, " -> ");
        assert(arrow != line.end());
        
        std::string moduleText = std::string(line.begin(), arrow);
        std::string rest = std::string(arrow+4, line.end());
        if(moduleText == "broadcaster") {
            m.name = moduleText;
            m.moduleOption = Broadcaster();
        }
        else if (moduleText.starts_with("%")){
            m.name = moduleText.substr(1);
            m.moduleOption = FlipFlop();
        }
        else {
            assert(moduleText.starts_with("&"));
            m.name = moduleText.substr(1);
            m.moduleOption = Conjunction();
        }

        m.destinations.clear();
        auto firstIter = rest.begin();
        using namespace std::string_literals;
        while(firstIter != rest.end()) {
            auto pattern = ","s;
            auto comma = std::search(firstIter, rest.end(), pattern.begin(), pattern.end());
            m.destinations.emplace_back(firstIter, comma);
            if(comma != rest.end()){
                firstIter = comma + 2;
            }
            else {
                firstIter = rest.end();
            }
        }

        return is;
    }
};


std::unordered_map<std::string, Module> createModuleLookup(std::span<Module> modules){
    std::unordered_map<std::string, Module> out;
    std::ranges::transform(modules, std::inserter(out, out.begin()), [](auto m){ return std::pair(m.name, m); });

    for(auto m: modules) {
        for(auto destination: m.destinations){
            if(std::holds_alternative<Conjunction>(out[destination].moduleOption)){
                std::get<Conjunction>(out[destination].moduleOption).set(m.name, false);
            }
        }
    }
    return out;
}

size_t toState(std::unordered_map<std::string, Module> modules, std::vector<std::string> units) {
    size_t value = 0;
    for(auto unit: units) {
        if(std::holds_alternative<FlipFlop>(modules[unit].moduleOption)){
            value = (value << 2) | std::get<FlipFlop>(modules[unit].moduleOption).isOn ? 1 : 0;
        }
        else if (std::holds_alternative<Conjunction>(modules[unit].moduleOption)){
            auto conjunction = std::get<Conjunction>(modules[unit].moduleOption);
            value = (value << conjunction.size()) | conjunction.getBinaryRepresentation();
        }
    }
    return value;
}

size_t processPulses(std::unordered_map<std::string, Module> modules, bool lookForRx=false){
    size_t lowPulses = 0;
    size_t highPulses = 0;
    std::deque<std::tuple<std::string, bool, std::string>> pulses;
    auto range = lookForRx ? std::views::iota(1UL, UINT64_MAX) : std::views::iota(1UL, 1001UL);
    std::vector<std::string> circuit1 { "fv", "nt", "zp", "kj", "gx", "lv", "tp", "rk", "tt", "sx", "mt", "xg", "dv", "kz"};
    std::vector<std::string> circuit2 { "cr", "vz", "hf", "jn", "tl", "br", "fk", "gk", "xt", "gh", "dj", "fj", "bl", "km" };
    std::vector<std::string> circuit3 { "fn", "rt", "rj", "mf", "gb", "jp", "tx", "dd", "qx", "bx", "bf", "vx", "sk", "xj"};
    std::vector<std::string> circuit4 { "mg", "ps", "ph", "tk", "xz", "cm", "bj", "nn", "nh", "ls", "cf", "nz", "dp", "qs"};
    std::optional<std::pair<size_t, size_t>> circuit1Interval;
    std::optional<std::pair<size_t, size_t>> circuit2Interval;
    std::optional<std::pair<size_t, size_t>> circuit3Interval;
    std::optional<std::pair<size_t, size_t>> circuit4Interval;
    std::unordered_map<size_t, size_t> stateMap;
    for(auto buttonsPressed : range) {
        if(circuit1Interval && circuit2Interval && circuit3Interval && circuit4Interval) {
            // assuming that we get back to an initial state (because thats what the puzzle has)
            return circuit1Interval->second * circuit2Interval->second * circuit3Interval->second * circuit4Interval->second;

        }
        if(buttonsPressed % 100'000'000 == 0){
            std::cout << buttonsPressed << "\n";
        }
        pulses.emplace_back("broadcaster", false, "button");
        
        while(!pulses.empty()){
            auto [location, isHigh, sender] = pulses.front();
            if(location == "rx" && lookForRx && !isHigh){
                return buttonsPressed;
            }
            if(isHigh) {
                highPulses++;
            }
            else { 
                lowPulses++;
            }
            pulses.pop_front();
            if(modules.find(location) == modules.end()) {
                continue;
            }

            auto& [name, option, destinations] = modules[location];
            bool isSent = true;
            bool sendHighPulse = isHigh;
            if(std::holds_alternative<FlipFlop>(option)){
                auto& flipflop = std::get<FlipFlop>(option);
                if(sendHighPulse){
                    isSent = false;
                } 
                else {
                    flipflop.isOn = !flipflop.isOn;
                    sendHighPulse = flipflop.isOn;
                }

            }
            else if (std::holds_alternative<Conjunction>(option)){
                auto& conjunction = std::get<Conjunction>(option);
                conjunction.set(sender, isHigh);
                sendHighPulse = !conjunction.isAllHigh();
            }
            if(isSent){
                for(auto destination: destinations){
                    pulses.emplace_back(destination, sendHighPulse, name);
                }

                if(name == circuit1.back() && !isHigh && !circuit1Interval){
                    auto stateKey = (toState(modules, circuit1)) << 1 | 0;
                    if(stateMap.contains(stateKey)){
                        circuit1Interval = {buttonsPressed, buttonsPressed - stateMap[stateKey]};
                    }
                    else {
                        stateMap[stateKey] = buttonsPressed;
                    }
                }
                if(name == circuit2.back() && !isHigh && !circuit2Interval){
                    auto stateKey = (toState(modules, circuit2)) << 1 | 1;
                    if(stateMap.contains(stateKey)){
                        circuit2Interval = {buttonsPressed, buttonsPressed - stateMap[stateKey]};
                    }
                    stateMap[stateKey] = buttonsPressed;
                }
                if(name == circuit3.back() && !isHigh && !circuit3Interval){
                    auto stateKey = (toState(modules, circuit3)) << 1 | 2;
                    if(stateMap.contains(stateKey)){
                        circuit3Interval = {buttonsPressed, buttonsPressed - stateMap[stateKey]};
                    }
                    stateMap[stateKey] = buttonsPressed;
                }
                if(name == circuit4.back() && !isHigh && !circuit4Interval){
                    auto stateKey = (toState(modules, circuit4)) << 1 | 3;
                    if(stateMap.contains(stateKey)){
                        circuit4Interval = {buttonsPressed, buttonsPressed - stateMap[stateKey]};
                    }
                    stateMap[stateKey] = buttonsPressed;
                }
            }

        }
    }

    return lowPulses * highPulses;
}


int main() {
    auto modules = input::readFromFile<Module>("input/day20.txt");
    auto moduleLookup = createModuleLookup(modules);

    std::cout << processPulses(moduleLookup) << "\n";

    for(auto p: moduleLookup) {
        auto [name, m] = p;
        std::visit(overloaded {
            [](Broadcaster){},
            [](FlipFlop& flop) { flop.isOn = false;},
            [](Conjunction& c){ c.reset(); }
        }, m.moduleOption);

    }
    std::cout << processPulses(moduleLookup, true) << "\n";
    return 0;
}