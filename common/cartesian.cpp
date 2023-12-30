#include <algorithm>
#include <cassert>
#include <ranges>
#include <utility>

#include "cartesian.h"

namespace cartesian {

    Point Point::toLeft(long distance) const {
        return {x-distance, y};
    }
    
    Point Point::toRight(long distance) const {
        return {x+distance, y};
    }

    // technically this grid is done upside down 
    Point Point::toAbove(long distance) const {
        return {x, y-distance};
    }
    
    Point Point::toBelow(long distance) const {
        return {x, y+distance};
    }

    Point Point::to(Orientation orientation, long distance) const {
        switch(orientation){
            case Orientation::LEFT: return toLeft(distance);
            case Orientation::UP: return toAbove(distance);
            case Orientation::RIGHT: return toRight(distance);
            case Orientation::DOWN: return toBelow(distance);
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

    bool TextGrid::contains(Point point) const {
        return point.y >= 0 && static_cast<size_t>(point.y) < lines.size() && point.x >= 0 && static_cast<size_t>(point.x) < lines[point.y].size();
    }
        
    void TextGrid::slide(Point point, Orientation orientation, char emptySymbol) {
        Point nextPoint = point;
        while(this->contains(nextPoint)){
            Point newPoint = nextPoint.to(orientation);
            if(this->at(newPoint) != emptySymbol){ 
                break;
            }
            if(!this->contains(newPoint)){
                break;
            }
            nextPoint = newPoint;
        }
        char nextChar = this->at(nextPoint);
        this->set(nextPoint, this->at(point));
        this->set(point, nextChar);
    }
    
    void TextGrid::set(Point p, char c){
        assert(this->contains(p));
        this->lines[p.y][p.x] = c;
    }
        
    size_t TextGrid::height() const {
        return lines.size();
    }
    
    size_t TextGrid::width() const {
        return lines[0].size();
    }
    
    Orientation turnLeft(Orientation orientation){
        switch(orientation) {
            case Orientation::UP: return Orientation::LEFT;
            case Orientation::LEFT: return Orientation::DOWN;
            case Orientation::DOWN: return Orientation::RIGHT;
            case Orientation::RIGHT: return Orientation::UP;
        }
        std::unreachable();
        assert(false);
    }
    
    Orientation turnRight(Orientation orientation){
        switch(orientation) {
            case Orientation::UP: return Orientation::RIGHT;
            case Orientation::RIGHT: return Orientation::DOWN;
            case Orientation::DOWN: return Orientation::LEFT;
            case Orientation::LEFT: return Orientation::UP;
        }
        std::unreachable();
        assert(false);
    }
    
    Orientation opposite(Orientation orientation){
        switch(orientation) {
            case Orientation::UP: return Orientation::DOWN;
            case Orientation::RIGHT: return Orientation::LEFT;
            case Orientation::DOWN: return Orientation::UP;
            case Orientation::LEFT: return Orientation::RIGHT;
        }
        std::unreachable();
        assert(false);
    }

        
}