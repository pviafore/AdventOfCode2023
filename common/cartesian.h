#ifndef CARTESIAN_H_
#define CARTESIAN_H_

#include <algorithm>
#include <functional>
#include <istream>
#include <iterator>
#include <ranges>
#include <unordered_map>
#include <vector>
namespace cartesian {

    enum class Orientation { 
        UP,
        RIGHT,
        DOWN,
        LEFT
    };

    Orientation turnLeft(Orientation orientation);
    Orientation turnRight(Orientation orientation);
    Orientation opposite(Orientation orientation);

    struct Point {
        long x = 0;
        long y = 0;

        Point toLeft(long distance = 1) const;       
        Point toRight(long distance = 1) const;        
        Point toAbove(long distance = 1) const ;       
        Point toBelow(long distance = 1) const;
        Point to(Orientation orientation, long distance = 1) const;
        std::vector<Point> getNeighbors() const;

        friend auto operator<=>(const Point& p1, const Point& p2) = default;

        friend std::ostream& operator<<(std::ostream&  os, const Point& point) {
            os << "(" << point.x << ", " << point.y << ")";
            return os;
        }
    };


    class TextGrid {

    public:
        friend std::istream& operator>>(std::istream& is, TextGrid& grid){
            grid.lines.clear();
            std::copy_if(std::istream_iterator<std::string>(is),
                         std::istream_iterator<std::string>(),
                         std::back_inserter(grid.lines),
                         [](auto s) {return !s.empty(); });
            return is;
        }

        friend std::ostream& operator<<(std::ostream& os, const TextGrid& grid){
            std::ranges::copy(grid.lines, std::ostream_iterator<std::string>(os, "\n"));
            return os;
        }

        // will break if any iterators are invalidated in the middle
        void forEach(std::function<void(Point, char)> f) const;
        // defaults to '.' if out of bounds
        char at(Point p) const;

        void set(Point p, char c);
        
        size_t height() const;
        size_t width() const;

        std::optional<Point> find(char c) const;

        std::vector<std::string> getLines() const;

        std::vector<std::string> getColumns() const;

        TextGrid() = default;
        TextGrid(std::vector<std::string> lines);

        std::vector<Point> findAll(char c) const;

        bool contains(Point point) const;
        void slide(Point point, Orientation orientation, char emptySymbol);

    private:
        std::vector<std::string> lines;

    };

    using Line = std::pair<Point, Point>;
}
template <>
struct std::hash<cartesian::Point> {
    std::size_t operator()(const cartesian::Point& p) const {
        static_assert(sizeof(p.x) && sizeof(p.y) <= sizeof(size_t), "Make sure that we can fit both x and y in a size_t for hashing uniquely");
        return p.x << sizeof(size_t)/2 | p.y;
    }
};

#endif