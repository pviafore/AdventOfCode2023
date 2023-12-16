#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <regex>
#include <map>
#include <utility>

#include "common/input.h"
#include "common/graph.h"

std::pair<std::string, graph::Graph<std::string>> readFromFile(std::string fileName){ 
    auto graph =  graph::Graph<std::string>{};
    std::fstream in(fileName);
    std::string instructions;
    getline(in, instructions);

    std::string ignored;
    getline(in, ignored);
    while(in.good()){
        std::string line;
        getline(in, line);
        if (line.empty()) {
            break;
        }
        std::regex pattern {R"((\w{3}) = \((\w{3}), (\w{3})\))"};
        std::smatch match;
        std::regex_match(line, match, pattern);

        graph.addEdge(match[1], match[2]);
        graph.addEdge(match[1], match[3]);

    }

    return {instructions, graph};
}


std::pair<std::string, unsigned int> walk(const std::string& instructions, const graph::Graph<std::string>& graph,  const std::string& starting, std::function<bool(std::string)> predicate){
    std::string location = starting;
     for (int64_t step: std::views::iota(0)){

        int64_t instruction = instructions[step % instructions.size()];
        auto index = (instruction == 'L') ? 0U : 1U;

        location = *(graph.at(location).begin() + index);
        if(predicate(location)) {
            return {location, step+1};
        }
        
    }
    std::unreachable();
    assert(false);
}

unsigned int getStepsToZZZ(const std::string instructions, const graph::Graph<std::string>& graph) {
    auto atEnd = [](auto s) { return s == "ZZZ"; };
    auto [end, steps] = walk(instructions, graph, "AAA", atEnd);
    return steps;
}

using StartCycleLength = std::pair<unsigned int, unsigned int>;
StartCycleLength toCycle(const std::string& instructions, const graph::Graph<std::string>& graph, std::string startingLocation){
    auto endsWithZ = [](std::string s){ return s.ends_with("Z"); };
    auto [location, steps] = walk(instructions, graph, startingLocation, endsWithZ);
    auto newInstructions = instructions;

    // given a location and the set of instructions it will take, say the last steps we were at there
    std::ranges::rotate(newInstructions, newInstructions.begin() + steps % instructions.size());
    auto [newLocation, stepsSeenSoFar] = walk(newInstructions, graph, location, endsWithZ);
    assert(newLocation == location);
    return {steps,stepsSeenSoFar};
}

int64_t getSimultaneousGhostPath(const std::string instructions, const graph::Graph<std::string>& graph){
    auto keys = graph.getKeys();
    auto endsWithA = [](const std::string& k) { return k.ends_with('A'); };
    auto cycles  = std::views::filter(keys, endsWithA) | std::views::transform([&instructions, &graph](auto s) { return toCycle(instructions, graph, s); });
    uint64_t value = 1;
    for(auto [start, length]: cycles){
        value *= (length / std::gcd(value, length));
    }
    return value;

}

int main() {
    auto [instructions, graph] = readFromFile("input/day8.txt");
    std::cout << getStepsToZZZ(instructions, graph) << "\n";
    std::cout << getSimultaneousGhostPath(instructions, graph) << "\n";
    return 0;
}