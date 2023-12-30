#include <algorithm>
#include <cassert>
#include <iostream>
#include <ranges>
#include <span>
#include <sstream>
#include <unordered_set>

#include "common/input.h"
#include "common/cartesian.h"

struct DigInstruction {
    cartesian::Orientation orientation;
    long squares;
    long realSquares;
    cartesian::Orientation realOrientation;

    friend std::istream& operator>>(std::istream& is, DigInstruction& instruction){
        char c;
        std::string squaresText;
        std::string color;
        is >> c >> squaresText >> color;
        if(squaresText.empty()){
            return is;
        }
        switch(c){
            case 'U': instruction.orientation = cartesian::Orientation::UP; break;
            case 'D': instruction.orientation = cartesian::Orientation::DOWN; break;
            case 'L': instruction.orientation = cartesian::Orientation::LEFT; break;
            case 'R': instruction.orientation = cartesian::Orientation::RIGHT; break;
        }
        instruction.squares = std::stol(squaresText);

        instruction.realSquares = std::stol(std::string(color.begin()+2, color.begin()+7), nullptr, 16);
        switch(color[7]){
            case '0': instruction.realOrientation = cartesian::Orientation::RIGHT; break;
            case '1': instruction.realOrientation = cartesian::Orientation::DOWN; break;
            case '2': instruction.realOrientation = cartesian::Orientation::LEFT; break;
            case '3': instruction.realOrientation = cartesian::Orientation::UP; break;
            default: assert(false);
        }
        return is;
    }
};

std::unordered_set<cartesian::Point> dig(std::span<DigInstruction> instructions){
    auto point = cartesian::Point{0,0}.to(cartesian::opposite(instructions.front().orientation));
    std::unordered_set<cartesian::Point> points;
    for(auto instruction: instructions) {
        for(auto i = 0U; i < instruction.squares; ++i){
            point = point.to(instruction.orientation);
            points.insert(point);
        }
    }
    return points;
}

std::unordered_set<cartesian::Point> fill(const std::unordered_set<cartesian::Point>& points) {
    std::unordered_set<cartesian::Point> newPoints{points.begin(), points.end()};
    auto [minX, maxX] = std::ranges::minmax(points | std::views::transform([](auto p){return p.x; }));
    auto [minY, maxY] = std::ranges::minmax(points | std::views::transform([](auto p){return p.y; }));
    for(long y: std::views::iota(minY, maxY+1)){
        bool inside = false;
        
        for(long x: std::views::iota(minX, maxX+1)){
            if(points.contains({x,y})){

                bool straightLine = points.contains({x, y+1}) && points.contains({x, y-1});
                bool above = points.contains({x, y-1});
                bool below = points.contains({x, y+1});
                while(points.contains(cartesian::Point{x+1, y})){
                    ++x;
                }
                if (straightLine || (above && points.contains({x, y+1})) || (below && points.contains({x,y-1}))){
                    inside = !inside;
                }
            }
            else {
                if(inside){
                    newPoints.emplace(x,y);
                }
            }
        }
    }
    return newPoints;
}

std::vector<cartesian::Point> toCoordinates(std::span<DigInstruction> instructions) {
    std::vector<cartesian::Point> points { {0,0}};
    auto lastPoint = points.front();
    for(auto instruction: instructions){
        lastPoint = lastPoint.to(instruction.realOrientation, instruction.realSquares);
        points.push_back(lastPoint);
    }
    assert(points.front() == points.back());
    return points;
}


size_t getSuperBigFillSize(std::span<DigInstruction> instructions) {
    auto coordinates = toCoordinates(instructions);
    auto firstZip = std::views::zip(std::ranges::subrange(coordinates.begin(), coordinates.end()), std::ranges::subrange(coordinates.begin() + 1, coordinates.end()));
    auto firstNums = std::ranges::fold_left(firstZip, 0U, [](auto sum, auto zipped) { auto [num1,num2] = zipped; return sum + num1.x*num2.y; });
    auto secondZip = std::views::zip(std::ranges::subrange(coordinates.begin()+1, coordinates.end()), std::ranges::subrange(coordinates.begin(), coordinates.end()));
    auto secondNums = std::ranges::fold_left(secondZip, 0U, [](auto sum, auto zipped) { auto [num1,num2] = zipped; return sum + num1.x*num2.y; });
    auto leftsAndUps = std::ranges::fold_left(instructions, 0U, [](auto sum, auto instruction){ return sum + (instruction.realOrientation == cartesian::Orientation::LEFT || instruction.realOrientation == cartesian::Orientation::UP ? instruction.realSquares : 0); });
    return  (firstNums - secondNums) / 2 + leftsAndUps + 1; 


}

int main(){
    auto instructions = input::readFromFile<DigInstruction>("input/day18.txt");
    auto points = dig(instructions);
    auto filledIn = fill(points);
    std::cout << filledIn.size() << "\n";

    std::cout << getSuperBigFillSize(instructions) << "\n";
}