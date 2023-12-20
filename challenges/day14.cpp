#include <algorithm>
#include <fstream>
#include <iostream>
#include <ranges>
#include <sstream>

#include "common/cartesian.h"

struct Platform {
    cartesian::TextGrid grid;

    friend std::istream& operator>>(std::istream& is, Platform& platform){
        is >> platform.grid;
        return is;
    }

    void slideAll(cartesian::Orientation orientation) {
        //rely on it going from top left to bottom right
        if (orientation == cartesian::Orientation::LEFT || orientation == cartesian::Orientation::UP) {
            for(auto point: grid.findAll('O')){
                grid.slide(point, orientation, '.');
            }
        }
        else {
            for(auto point: std::views::reverse(grid.findAll('O'))){
                grid.slide(point, orientation, '.');
            }
        }
    }

    unsigned int getLoad() const {
        return std::ranges::fold_left(grid.findAll('O'), 0U, [this](auto sum ,auto point){return sum + grid.height() - point.y;});
    }

    void spinCycle(size_t times) {
        std::unordered_map<std::string, size_t> seen;
        for(size_t time = 0; time < times; ++time){
            slideAll(cartesian::Orientation::UP);
            slideAll(cartesian::Orientation::LEFT);
            slideAll(cartesian::Orientation::DOWN);
            slideAll(cartesian::Orientation::RIGHT);

            auto s = toString();
            auto iter = seen.find(s);
            if(iter != seen.end()){
                while(time + (time - iter->second) < times){
                    time += (time - iter->second);
                } 
                seen.clear();
            }
            seen[s] = time;
        }
        
    }

    std::string toString() {
        std::stringstream ss;
        for(auto point: grid.findAll('O')) {
            ss << point.x << "," << point.y << ";";
        }
        return ss.str();
    }
};
int main() {
    Platform platform;
    std::fstream in{"input/day14.txt"}; 
    in >> platform;

    Platform original = platform;
    platform.slideAll(cartesian::Orientation::UP);

    std::cout << platform.getLoad() << "\n";

    original.spinCycle(1'000'000'000LL);
    std::cout << original.getLoad() << "\n";
    return 0;
}