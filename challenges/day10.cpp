#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <utility>

#include "common/cartesian.h"

cartesian::Orientation getNextDirection (char c, cartesian::Orientation currentOrientation) {
    assert(c != '.');
    if(c == '|' || c == '-'){
        return currentOrientation;
    }

    if(c == 'L'){
        return currentOrientation == cartesian::Orientation::DOWN ? cartesian::Orientation::RIGHT : cartesian::Orientation::UP;
    }
    if(c == 'F'){
        return currentOrientation == cartesian::Orientation::UP ? cartesian::Orientation::RIGHT : cartesian::Orientation::DOWN;
    }
    if(c == '7'){
        return currentOrientation == cartesian::Orientation::UP ? cartesian::Orientation::LEFT : cartesian::Orientation::DOWN;
    }
    if(c == 'J'){
        return currentOrientation == cartesian::Orientation::DOWN ? cartesian::Orientation::LEFT : cartesian::Orientation::UP;
    }

    std::unreachable();
    assert(false);

}

std::pair<cartesian::Orientation, cartesian::Orientation> getStartingDirections(cartesian::Point starting, const cartesian::TextGrid& grid) {
    std::optional<cartesian::Orientation> orientation1;
    std::optional<cartesian::Orientation> orientation2;

    auto leftChar = grid.at(starting.toLeft());
    if(leftChar == '-' || leftChar == 'F' || leftChar == 'L'){
        orientation1 = cartesian::Orientation::LEFT;
    }

    auto upChar = grid.at(starting.toAbove());
    if(upChar == '|' || upChar == '7' || upChar == 'F') {
        if(orientation1 ){ 
            return {orientation1.value(), cartesian::Orientation::UP};
        }
        else {
            orientation1 = cartesian::Orientation::UP;
        }
    }

    auto rightChar = grid.at(starting.toRight());
    if(rightChar == '-' || rightChar == '7' || rightChar == 'J'){
        if(orientation1 ){ 
            return {orientation1.value(), cartesian::Orientation::RIGHT};
        }
        else {
            orientation1 = cartesian::Orientation::RIGHT;
        }
    }

    assert(orientation1 && grid.at(starting.toBelow()) != '.' && grid.at(starting.toBelow()) != '-' );

    return {orientation1.value(), cartesian::Orientation::DOWN};
 
}

int getFurthestTileAway(const cartesian::TextGrid& grid){
    auto starting = grid.find('S').value();
    auto [orientation1, orientation2] = getStartingDirections(starting, grid);
    auto location1 = starting;
    auto location2 = starting;
    for(unsigned int steps : std::views::iota(1)){
        location1 = location1.to(orientation1);
        location2 = location2.to(orientation2);

        if(location1 == location2){
            return steps;
        }

        orientation1 = getNextDirection(grid.at(location1), orientation1);
        orientation2 = getNextDirection(grid.at(location2), orientation2);
    }

    std::unreachable();
    assert(false);
   
}

auto getLoopPoints(const cartesian::TextGrid& grid){
    auto starting = grid.find('S').value();
    auto [orientation1, orientation2] = getStartingDirections(starting, grid);
    auto location1 = starting;
    auto location2 = starting;
    std::unordered_set<cartesian::Point> points;
    points.insert(starting);
    while(true){
        location1 = location1.to(orientation1);
        location2 = location2.to(orientation2);
        points.insert(location1);
        points.insert(location2);

        if(location1 == location2){
            return points;
        }

        orientation1 = getNextDirection(grid.at(location1), orientation1);
        orientation2 = getNextDirection(grid.at(location2), orientation2);
    }

    std::unreachable();
    assert(false);
}

unsigned int getPointsInsideLoop(const cartesian::TextGrid& grid){
    auto starting = grid.find('S').value();
    auto [orientation1, orientation2] = getStartingDirections(starting, grid);
    unsigned int numOfPoints = 0;
    auto points = getLoopPoints(grid);
    for(const auto& [y, line]: grid.getLines() | std::views::enumerate){
        bool inside = false;
        char lastHorizontalCharacter = '!';
        for(auto [x, character]: line | std::views::enumerate ) {
            cartesian::Point p {x, y};
            if (p == starting){
                if(orientation1 == cartesian::Orientation::UP && orientation2 == cartesian::Orientation::DOWN){
                    character = '|';
                }
                else if(orientation1 == cartesian::Orientation::LEFT && orientation2 == cartesian::Orientation::UP){
                    character = 'J';
                }
                else if(orientation1 == cartesian::Orientation::UP && orientation2 == cartesian::Orientation::RIGHT){
                    character = 'L';
                }
                else if(orientation1 == cartesian::Orientation::RIGHT && orientation2 == cartesian::Orientation::DOWN){
                    character = 'F';
                }
                else if(orientation1 == cartesian::Orientation::LEFT && orientation2 == cartesian::Orientation::DOWN){
                    character = '7';
                }
                else {
                    assert(false);
                }

            }
            if(inside && !points.contains(p)) { 
                numOfPoints++;
            }
            if(points.contains(p)) {
                if(character == '|') {
                    inside = !inside;
                }
                if(character == 'J') {
                    if(lastHorizontalCharacter == 'F'){
                        inside = !inside;
                    }
                    lastHorizontalCharacter = '!';
                }
                if(character == '7') {
                    if(lastHorizontalCharacter == 'L'){
                        inside = !inside;
                    }
                    lastHorizontalCharacter = '!';
                }
                if(character == 'F' || character == 'L') {
                    lastHorizontalCharacter = character;
                }
            }
        }
    }
    return numOfPoints;
}

int main() {
    std::fstream in{"input/day10.txt"};
    cartesian::TextGrid grid;
    in >> grid;
    std::cout << getFurthestTileAway(grid) << "\n";
    std::cout << getPointsInsideLoop(grid) << "\n";
    

}