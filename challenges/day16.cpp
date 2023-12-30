#include <algorithm>
#include <cassert>
#include <iostream>
#include <fstream>
#include <ranges>
#include <set>
#include <unordered_set>
#include <utility>

#include "common/cartesian.h"

struct Contraption {
    friend std::istream& operator>>(std::istream& is, Contraption& contraption){
        is >> contraption.grid;
        return is;
    }

    size_t getTilesEnergized(cartesian::Point startingPoint = {0,0}, cartesian::Orientation startingOrientation = cartesian::Orientation::RIGHT) const {
        std::unordered_set<cartesian::Point> energized;
        std::vector<std::pair<cartesian::Point, cartesian::Orientation>> points {{startingPoint, startingOrientation}};
        std::set<std::pair<cartesian::Point, cartesian::Orientation>> seen;
        while(!points.empty()){
            
            auto [point, orientation] = points.back();
            points.pop_back();

            if(grid.contains(point) && !seen.contains({point, orientation})){
                energized.insert(point);
                seen.emplace(point, orientation);

                auto nextPoints = getNextPoints(point, orientation);
                points.insert(points.end(), nextPoints.begin(), nextPoints.end());
            }

        }
        return energized.size();
    }

    std::vector<std::pair<cartesian::Point, cartesian::Orientation>> getNextPoints(cartesian::Point point, cartesian::Orientation orientation) const {
        char c = grid.at(point);
        if(c == '.'){
            return {{point.to(orientation), orientation}};
        }
        if(c == '-'){
            if(orientation == cartesian::Orientation::LEFT || orientation == cartesian::Orientation::RIGHT){
                return {{point.to(orientation), orientation}};
            }
            return {
                {point.toLeft(), cartesian::Orientation::LEFT},
                {point.toRight(), cartesian::Orientation::RIGHT}
            };
        }
        if (c == '|' ){
            if(orientation == cartesian::Orientation::UP || orientation == cartesian::Orientation::DOWN){
                return {{point.to(orientation), orientation}};
            }
            return {
                {point.toAbove(), cartesian::Orientation::UP},
                {point.toBelow(), cartesian::Orientation::DOWN}
            };
        }
        if (c == '/') {
            switch (orientation){
                case cartesian::Orientation::UP: return {{point.toRight(), cartesian::Orientation::RIGHT}};
                case cartesian::Orientation::DOWN: return {{point.toLeft(), cartesian::Orientation::LEFT}};
                case cartesian::Orientation::LEFT: return {{point.toBelow(), cartesian::Orientation::DOWN}};
                case cartesian::Orientation::RIGHT: return {{point.toAbove(), cartesian::Orientation::UP}};
            }
        }
        if (c == '\\') {
            switch (orientation){
                case cartesian::Orientation::UP: return {{point.toLeft(), cartesian::Orientation::LEFT}};
                case cartesian::Orientation::DOWN: return {{point.toRight(), cartesian::Orientation::RIGHT}};
                case cartesian::Orientation::LEFT: return {{point.toAbove(), cartesian::Orientation::UP}};
                case cartesian::Orientation::RIGHT: return {{point.toBelow(), cartesian::Orientation::DOWN}};
            }
        }

        std::unreachable();
        assert(false);
    }

    size_t getMaxFromAnyConfiguration() {
        auto yRange = std::views::iota(0U, grid.height());
        auto fromLeft = std::ranges::max(yRange | std::views::transform([this](auto y) { return getTilesEnergized({0, y}, cartesian::Orientation::RIGHT); }));
        auto fromRight = std::ranges::max(yRange | std::views::transform([this](auto y) { return getTilesEnergized({static_cast<ssize_t>(grid.width()-1), y}, cartesian::Orientation::LEFT); }));
        auto xRange = std::views::iota(0U, grid.width());
        auto fromAbove = std::ranges::max(xRange | std::views::transform([this](auto x) { return getTilesEnergized({x, 0}, cartesian::Orientation::DOWN); }));
        auto fromBelow = std::ranges::max(xRange | std::views::transform([this](auto x) { return getTilesEnergized({x, static_cast<ssize_t>(grid.height()-1)}, cartesian::Orientation::UP); }));
        return std::ranges::max({fromLeft, fromRight, fromAbove, fromBelow});
    }

    cartesian::TextGrid grid;
};

int main() {
    Contraption contraption;
    std::fstream in{"input/day16.txt"};
    in >> contraption;

    std::cout << contraption.getTilesEnergized() << "\n";
    std::cout << contraption.getMaxFromAnyConfiguration() << "\n";
}