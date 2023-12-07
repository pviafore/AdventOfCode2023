#include <algorithm>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string_view>
#include <vector>

struct Mapping {
    int64_t source;
    int64_t destination;
    int64_t size;

    friend std::istream& operator>>(std::istream& is, Mapping& mapping){
        is >> mapping.destination >> mapping.source >> mapping.size;
        return is;
    }

    bool contains(int64_t num) const {
        return num >= source && num < source+size;
    }

    int64_t map(int64_t num) const {
        assert(contains(num));
        return num - source + destination;
    }
};

struct Almanac {
    using Mappings = std::vector<Mapping>;
    std::vector<int64_t> seeds;
    std::vector<Mappings> mappings;

    friend std::istream& operator>>(std::istream& is, Almanac& almanac){
        almanac.seeds.clear();
        almanac.mappings.clear();
        std::string seedsString;
        std::string ignoredString;
        is >> ignoredString;
        getline(is, seedsString);
        // ignore first word and space
        auto seedsRange = std::string_view(seedsString.begin()+1, seedsString.end());
        auto splitSeeds = std::views::split(seedsRange, ' ');
        std::ranges::transform(splitSeeds.begin(), splitSeeds.end(), std::back_inserter(almanac.seeds), [](auto s){
            return std::stoull(std::string{s.begin(), s.end()});
        });

        getline(is, ignoredString);
        while (is.good()){
            // label
            getline(is, ignoredString);
            std::string inputLine = "setSoLoopWillEnter";
            Mappings mappings;
            while(is.good() && !inputLine.empty()) {
                getline(is, inputLine);
                if(!inputLine.empty()){
                    std::istringstream iss(inputLine);
                    Mapping mapping;
                    iss >> mapping;
                    mappings.push_back(mapping);
                }
            }
            almanac.mappings.push_back(mappings);
        }

        return is;

    }

    int64_t getClosestSeed() {
        return std::ranges::min(std::views::transform(seeds, [this](auto s){return map(s);}));
    }

    int64_t map(int64_t seed){
        for(const auto& mapping: mappings){
            seed = applyMapping(seed, mapping);
        }
        return seed;
    }

    int64_t applyMapping(int64_t seed, const Mappings& mappings) const {
        for(const auto& mapping: mappings){
            if(mapping.contains(seed)){
                return mapping.map(seed);
            }
        }
        return seed;
    }

    std::vector<std::pair<int64_t, int64_t>> applyRangeMappings(int64_t start, int64_t end) const {
        std::vector<std::pair<int64_t, int64_t>> out { {start, end}};
        for(const auto& mapping: mappings){
            std::vector<std::pair<int64_t, int64_t>> newMappings;
            while(!out.empty()) {
                auto oldMapping = *out.begin();
                out.erase(out.begin());
                bool isMappingMade = false;;
                for(const auto& newMapping: mapping) { 

                    if(oldMapping.first >= newMapping.source && oldMapping.first < newMapping.source + newMapping.size){
                        int64_t offset = oldMapping.first - newMapping.source;
                        int64_t oldSize = oldMapping.second - oldMapping.first;
                        int64_t intendedNewEnd = newMapping.destination + oldSize + offset;
                        int64_t newMappingEnd = newMapping.destination + newMapping.size - 1;
                        newMappings.emplace_back(newMapping.destination + offset, std::min(intendedNewEnd, newMappingEnd));
                        if(intendedNewEnd > newMappingEnd){
                            // not completely in the range - going to the right
                            out.emplace_back(newMapping.source + newMapping.size, oldMapping.second);
                        }
                        isMappingMade = true;
                    }
                    // we are on the left side of the range
                    else if (oldMapping.second >= newMapping.source && oldMapping.second < newMapping.source + newMapping.size) {
                        auto offset = newMapping.destination - newMapping.source;
                        newMappings.emplace_back(newMapping.destination, oldMapping.second + offset);

                        out.emplace_back(oldMapping.first, newMapping.destination-1 - offset);
                        isMappingMade = true;
                    }
                }
                if( !isMappingMade){ 
                    // none of the mapping applied
                    newMappings.push_back(oldMapping);
                }

            }
            std::ranges::sort(newMappings);
            out.clear();
            for(auto iterator = newMappings.begin(); iterator != newMappings.end(); ++iterator){
                out.push_back(*iterator);
                // collapse
                auto next = iterator + 1;
                while(next != newMappings.end() && (next->first == out.rbegin()->second + 1 || next->second == out.rbegin()->second || out.rbegin()->first == next->first )) {
                    out.rbegin()->second = next->second;
                    ++iterator;
                    next = iterator + 1;
                }

            }
        }
        return out;
    }
 
    int64_t getClosestSeedFromRanges() const {
        auto seedLocations = seeds 
                            | std::views::chunk(2)
                            | std::views::transform([this](auto r){ return applyRangeMappings(*r.begin(), *r.begin() + *(r.begin()+1));})
                            | std::views::transform([](auto mappings){ return mappings.begin()->first; });
        return std::ranges::min(seedLocations);
    }
};


int main(){
    std::fstream in("input/day5.txt");
    Almanac almanac;
    in >> almanac;

    std::cout << almanac.getClosestSeed() << "\n";
    std::cout << almanac.getClosestSeedFromRanges() << "\n";
    return 0;
}