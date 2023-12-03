#include <algorithm>
#include <functional>
#include <istream>
#include <iterator>
#include <ranges>
#include <unordered_map>
#include <vector>
namespace cartesian {

    struct Point {
        long x = 0;
        long y = 0;

        Point toLeft() const;       
        Point toRight() const;        
        Point toAbove() const ;       
        Point toBelow() const;
        std::vector<Point> getNeighbors() const;

        friend auto operator<=>(const Point& p1, const Point& p2) = default;
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

        // will break if any iterators are invalidated in the middle
        void forEach(std::function<void(Point, char)> f) const;
        // defaults to '.' if out of bounds
        char at(Point p) const;

    private:
        std::vector<std::string> lines;

    };
}
template <>
struct std::hash<cartesian::Point> {
    std::size_t operator()(const cartesian::Point& p) const {
        static_assert(sizeof(p.x) && sizeof(p.y) <= sizeof(size_t), "Make sure that we can fit both x and y in a size_t for hashing uniquely");
        return p.x << sizeof(size_t)/2 | p.y;
    }
};