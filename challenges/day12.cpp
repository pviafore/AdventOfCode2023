#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <istream>
#include <ranges>
#include <unordered_set>
#include <unordered_map>

#include "common/input.h"

struct Record {
    std::string springs;
    std::vector<unsigned int> damagedGroupings;

    friend std::istream& operator>>(std::istream& is, Record& record){
        std::string damagedString;
        is >> record.springs >> damagedString;
        auto rangeToInt = [](auto r){return std::stoul(std::string{r.begin(), r.end()}); };
        record.damagedGroupings.clear();
        auto groupings = std::views::split(damagedString, ','); 
        std::ranges::transform(groupings, std::back_inserter(record.damagedGroupings), rangeToInt);

        return is;
    }

    void unfold() {
        auto originalSprings = springs;
        auto originalGroupings = damagedGroupings;
        for([[maybe_unused]]auto _ : std::views::iota(0,4)){
           springs += "?" + originalSprings;
           damagedGroupings.insert(damagedGroupings.end(), originalGroupings.begin(), originalGroupings.end());
        }
    }

    uint64_t getArrangements() const { 
        using Candidate = size_t;
        std::unordered_map<Candidate, size_t> candidates;
        auto startingWindow = damagedGroupings.front();
        auto matchesDamage = [](auto c) { return c == '#' || c == '?'; };
        auto firstDamage = springs.find('#');
        auto matchesAllDamage = [matchesDamage](auto indexAndRange) { auto [_index, range] = indexAndRange; return std::all_of(range.begin(), range.end(), matchesDamage); };
        auto firstDamageIter = (firstDamage <= springs.size() - startingWindow) ? springs.begin()+firstDamage+startingWindow : springs.end();
        std::string firstSprings ( springs.begin(), firstDamageIter);
        auto startingPoints  = std::views::enumerate(std::views::slide(firstSprings, startingWindow))
                                | std::views::filter(matchesAllDamage)
                                | std::views::transform([this, startingWindow](auto indexAndRange){ return std::get<0>(indexAndRange) + startingWindow ; });
        
        for(auto startingPoint: startingPoints){
            if(!candidates.contains(startingPoint)){
                candidates[startingPoint] = 0;
            }
            candidates[startingPoint]++;
        }

        for(auto damagedGrouping: damagedGroupings | std::views::drop(1)){
            std::unordered_map<Candidate, size_t> newCandidates;
            for(auto [index, times]: candidates) {

                if(index >= springs.size()){
                    // we have exhausted the end, but not the damaged groups
                    continue;
                }

                if(springs[index] == '.' || springs[index] == '?'){
                    // we have a match, let's look for any matches and add them to new candidates
                    auto rangeToCandidate = [this, damagedGrouping, index](auto indexAndRange) { return std::get<0>(indexAndRange)+index+1 +damagedGrouping; };
                    auto firstDamage = springs.find('#', index+1);
                    auto firstDamageIter = (firstDamage <= springs.size() - damagedGrouping) ? springs.begin()+firstDamage+damagedGrouping : springs.end();
                    std::string substr(springs.begin()+index+1, firstDamageIter);
                    auto matchingPoints = std::views::enumerate(std::views::slide(substr, damagedGrouping))
                                            | std::views::filter(matchesAllDamage)
                                            | std::views::transform(rangeToCandidate)
                                            | std::views::filter([this](auto val){return val <= springs.size(); });
                    for(auto matchingPoint: matchingPoints){
                        if(!newCandidates.contains(matchingPoint)){
                            newCandidates[matchingPoint] = 0;
                        }
                        
                        newCandidates[matchingPoint] += times;

                    }
                }
            }
            std::swap(candidates, newCandidates);
            
        }

        uint64_t arrangements = std::ranges::fold_left(candidates, 0ULL, [this](auto sum, auto p){ 
            return sum + (springs.find('#', p.first) == std::string::npos ? p.second : 0);
        });
        return arrangements;
    }
};
int main(){
    auto records = input::readFromFile<Record>("input/day12.txt");
    auto allPossibleArrangements = std::ranges::fold_left(records, 0ULL, [](auto sum, auto record){ return sum + record.getArrangements(); });
    std::cout << allPossibleArrangements << "\n";

    for(auto& record: records){
        record.unfold();
    }
    allPossibleArrangements = std::ranges::fold_left(records, 0ULL, [](auto sum, auto record){ return sum + record.getArrangements(); });
    std::cout << allPossibleArrangements << "\n";
    return 0;
}