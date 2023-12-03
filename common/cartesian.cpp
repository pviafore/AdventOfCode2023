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

}