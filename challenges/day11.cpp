#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <ranges>
#include <unordered_set>

#include "common/cartesian.h"

bool isEmpty(char c) { 
    return c == '.'; 
}
struct GalaxyMap {
    cartesian::TextGrid grid;
    
    std::vector<unsigned int> columnsWithNoGalaxies;
    std::vector<unsigned int> rowsWithNoGalaxies;

    friend std::istream& operator>>(std::istream& is, GalaxyMap& map){
        is >> map.grid;
        map.columnsWithNoGalaxies.clear();
        map.rowsWithNoGalaxies.clear();
        map.setEmptyColumns();
        map.setEmptyRows();
        return is;
    }

    void setEmptyColumns() {
        auto columns = grid.getColumns();
        for(const auto& [x, column]: std::views::enumerate(columns)){
            if(std::ranges::all_of(column, isEmpty)){
                columnsWithNoGalaxies.push_back(x);
            }

        }
    }

    void setEmptyRows() {
        for(auto [y, line]: grid.getLines() | std::views::enumerate ){
            if(std::ranges::all_of(line, isEmpty)){ 
                rowsWithNoGalaxies.push_back(y);
            }
        }
    }

    unsigned int getDistance(const cartesian::Point& point1, const cartesian::Point& point2, unsigned int expandDistance) const {
        unsigned int sum = std::abs(point1.x - point2.x) + std::abs(point1.y - point2.y) ;

        auto emptyXes = columnsWithNoGalaxies | std::views::filter([point1, point2](auto n) { return n > std::min(point1.x, point2.x) && n < std::max(point1.x, point2.x); });
        auto emptyYes = rowsWithNoGalaxies | std::views::filter([point1, point2](auto n) { return n > std::min(point1.y, point2.y) && n < std::max(point1.y, point2.y); });

        return sum + (expandDistance - 1) * (std::vector<int>{emptyXes.begin(), emptyXes.end()}.size() + std::vector<int>{emptyYes.begin(), emptyYes.end()}.size());
    }


    uint64_t getSumOfAllPairs(unsigned int expandSize) const {
        uint64_t sum = 0;
        auto galaxies = grid.findAll('#');
        for(auto [index, galaxy]: std::views::enumerate(galaxies)){
            if(index > 0 && static_cast<size_t>(index) == galaxies.size() -1 ){
                break;
            }

            sum += std::ranges::fold_left(galaxies.begin()+index+1, galaxies.end(), 0U, [galaxy, expandSize, this](auto sum, auto point) { 
                return sum + getDistance(galaxy, point, expandSize);
            });
        }
        return sum;
    }
};

int main() {
    std::fstream in{"input/day11.txt"};
    GalaxyMap galaxyMap;
    in >> galaxyMap;

    std::cout << galaxyMap.getSumOfAllPairs(2) << "\n";
    std::cout << galaxyMap.getSumOfAllPairs(1'000'000) << "\n";


}