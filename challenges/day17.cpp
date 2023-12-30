#include <cassert>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <map>
#include <queue>
#include <set>
#include <unordered_set>
#include <vector>

#include "common/cartesian.h"

struct Candidate {
    size_t heatLossIncurred = 0; 
    cartesian::Point point;
    cartesian::Orientation orientation;
    uint8_t stepsTakenInDirection = 0;

    friend auto operator<=>(const Candidate& lhs, const Candidate& rhs) = default;
};

using Seen = std::tuple<cartesian::Point, cartesian::Orientation, size_t>;

size_t getMinimumHeatLoss(const cartesian::TextGrid& grid){
    auto startingPoint = cartesian::Point{0,0};
    auto endingPoint = cartesian::Point(grid.width() -1, grid.height() - 1);

    std::set<Seen> seen;
    std::priority_queue<Candidate, std::vector<Candidate>, std::greater<Candidate>> candidates;
    candidates.emplace(0, startingPoint, cartesian::Orientation::RIGHT, 0);  
    candidates.emplace(0, startingPoint, cartesian::Orientation::DOWN, 0);  
    while(!candidates.empty()){
        auto candidate = candidates.top();
        candidates.pop();


        if(candidate.point == endingPoint){
            return candidate.heatLossIncurred;
        }

        auto newPoint = candidate.point.to(candidate.orientation);
        if(grid.contains(newPoint)){
            auto newHeatLoss = candidate.heatLossIncurred + grid.at(newPoint) - '0';
            if(!seen.contains({newPoint, cartesian::turnLeft(candidate.orientation), 0UL})) {
                seen.emplace(newPoint, cartesian::turnLeft(candidate.orientation), 0);
                candidates.emplace(newHeatLoss, newPoint, cartesian::turnLeft(candidate.orientation), 0);
            }
            if(!seen.contains({newPoint, cartesian::turnRight(candidate.orientation), 0UL})) {
                seen.emplace(newPoint, cartesian::turnRight(candidate.orientation), 0);
                candidates.emplace(newHeatLoss, newPoint, cartesian::turnRight(candidate.orientation), 0);
            }

            if(candidate.stepsTakenInDirection < 2){
                if(!seen.contains({newPoint, candidate.orientation, candidate.stepsTakenInDirection + 1})) {
                    seen.emplace(candidate.point, candidate.orientation, candidate.stepsTakenInDirection + 1);
                    candidates.emplace(newHeatLoss, newPoint, candidate.orientation, candidate.stepsTakenInDirection + 1);
                }
            }
        }
    }
    assert(false);
    return 0;
}

size_t getMinimumHeatLossSuperCrucible(const cartesian::TextGrid& grid){
    auto startingPoint = cartesian::Point{0,0};
    auto endingPoint = cartesian::Point(grid.width() -1, grid.height() - 1);

    std::map<Seen, size_t> seen;
    std::priority_queue<Candidate, std::vector<Candidate>, std::greater<Candidate>> candidates;
    candidates.emplace(0, startingPoint, cartesian::Orientation::RIGHT, 0);  
    candidates.emplace(0, startingPoint, cartesian::Orientation::DOWN, 0);  
    while(!candidates.empty()){
        auto candidate = candidates.top();
        candidates.pop();


        if(candidate.point == endingPoint && (candidate.stepsTakenInDirection >= 4 || candidate.stepsTakenInDirection == 0)){
            return candidate.heatLossIncurred;
        }

        auto newPoint = candidate.point.to(candidate.orientation);
        auto newSteps = candidate.stepsTakenInDirection + 1;
        if(grid.contains(newPoint)){
            auto newHeatLoss = candidate.heatLossIncurred + grid.at(newPoint) - '0';
            if(newSteps >= 4){ 
                auto newOrientation = cartesian::turnLeft(candidate.orientation);
                if(!seen.contains({newPoint, newOrientation, 0UL}) || newHeatLoss < seen[{newPoint, newOrientation, 0}] ) {
                    seen[{newPoint, newOrientation, 0}] = newHeatLoss;
                    candidates.emplace(newHeatLoss, newPoint, newOrientation, 0);
                }
                newOrientation = cartesian::turnRight(candidate.orientation);
                if(!seen.contains({newPoint, newOrientation, 0UL}) || newHeatLoss < seen[{newPoint, newOrientation, 0}]) {
                    seen[{newPoint, newOrientation, 0}] = newHeatLoss;
                    candidates.emplace(newHeatLoss, newPoint, newOrientation, 0);
                }
            }

            if(newSteps <= 9){
                if(!seen.contains({newPoint, candidate.orientation, newSteps}) || newHeatLoss < seen[{newPoint, candidate.orientation, newSteps}]) {
                    seen[{newPoint, candidate.orientation, newSteps}] = newHeatLoss;
                    candidates.emplace(newHeatLoss, newPoint, candidate.orientation, newSteps);
                }
            }
        }
    }
    assert(false);
    return 0;
}


int main() {
    cartesian::TextGrid grid;
    std::fstream in{"input/day17.txt"};
    in >> grid;

    std::cout << getMinimumHeatLoss(grid) << "\n";
    std::cout << getMinimumHeatLossSuperCrucible(grid) << "\n";
    return 0;
}