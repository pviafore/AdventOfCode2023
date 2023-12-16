#include <algorithm>
#include <cassert>
#include <ranges>
#include <utility>

#include "cartesian.h"

namespace cartesian {

    Point Point::toLeft() const {
        return {x-1, y};
    }
    
    Point Point::toRight() const {
        return {x+1, y};
    }

    // technically this grid is done upside down 
    Point Point::toAbove() const {
        return {x, y-1};
    }
    
    Point Point::toBelow() const {
        return {x, y+1};
    }

    Point Point::to(Orientation orientation) const {
        switch(orientation){
            case Orientation::LEFT: return toLeft();
            case Orientation::UP: return toAbove();
            case Orientation::RIGHT: return toRight();
            case Orientation::DOWN: return toBelow();
        }

        std::unreachable();
        assert(false);
    }

    std::vector<Point> Point::getNeighbors() const{
        return {
            this->toLeft(),
            this->toLeft().toAbove(),
            this->toLeft().toBelow(),
            this->toAbove(),
            this->toBelow(),
            this->toRight(),
            this->toRight().toAbove(),
            this->toRight().toBelow()
        };
    }

    // will break if any iterators are invalidated in the middle
    void TextGrid::forEach(std::function<void(Point, char)> f) const {
        for(auto [y, line] : lines | std::views::enumerate ){
            for(auto [x, character] : line | std::views::enumerate ){
                f(Point{x, y}, character);
            }
        }
    }

    // defaults to '.' if out of bounds
    char TextGrid::at(Point p) const{
        if(p.y < 0 || static_cast<size_t>(p.y) >= lines.size()) {
            return '.';
        }
        std::string line = lines[p.y];
        if(p.x < 0 || static_cast<size_t>(p.x) >= line.size()) {
            return '.';
        }
        return line[p.x];
    }

    std::optional<Point> TextGrid::find(char c) const {
        for(auto [y, line] : lines | std::views::enumerate ){
            for(auto [x, character] : line | std::views::enumerate ){
                if(character == c){
                    return Point{x,y};
                }
            }
        }
        return {};

    }

    std::vector<std::string> TextGrid::getLines() const {
        return lines;
    }

    std::vector<std::string> TextGrid::getColumns() const {
        std::vector<std::string> out;
        size_t lineLength = lines.begin()->size();
        for (auto index : std::views::iota(0U, lineLength)){
            out.push_back(std::ranges::fold_left(lines, std::string{}, [index](auto str, auto line){ return str + line[index]; }));
        } 
        return out;
    }
        
    TextGrid::TextGrid(std::vector<std::string> lines) : lines(std::move(lines)) {

    }
        
    std::vector<Point> TextGrid::findAll(char c) const {
        std::vector<Point> points;
         for(auto [y, line] : lines | std::views::enumerate ){
            for(auto [x, character] : line | std::views::enumerate ){
                if(character == c){
                    points.emplace_back(x,y);
                }
            }
        }
        return points;
    }

}