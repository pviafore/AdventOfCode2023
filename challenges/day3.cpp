#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <unordered_map>
#include <unordered_set>
#include <vector>


#include "common/cartesian.h"

struct Schematic {
    cartesian::TextGrid schematic;
    friend std::istream& operator>>(std::istream& is, Schematic& schematic) {
        is >> schematic.schematic;
        return is;
    }

    std::vector<unsigned int> getPartNumbers() const{
        std::vector<unsigned int> out;
        schematic.forEach([this, &out](cartesian::Point p, char c){
            if (isdigit(c)) { 
                if (isdigit(schematic.at(p.toLeft()))) {
                    return; // we've already taken care of this
                }
                bool isValidPartNumber = false;
                cartesian::Point nextPoint = p;
                std::string partNumber;
                while(isdigit(c)) {
                    isValidPartNumber = isValidPartNumber || isPartNumber(nextPoint);
                    partNumber += c;
                    nextPoint = nextPoint.toRight();
                    c = schematic.at(nextPoint);
                }
                if (isValidPartNumber){
                    out.push_back(std::stoi(partNumber));
                }
            }
        });
        return out;
    }

    std::vector<unsigned int> getGearRatios() const{
        std::unordered_map<cartesian::Point, std::vector<unsigned long>> gearToPartMapping;
        schematic.forEach([this, &gearToPartMapping](cartesian::Point p, char c){
            if (isdigit(c)) { 
                if (isdigit(schematic.at(p.toLeft()))) {
                    return; // we've already taken care of this
                }
                std::unordered_set<cartesian::Point> gears;
                cartesian::Point nextPoint = p;
                std::string partNumber;
                while(isdigit(c)) {
                    auto pointsGears = getGears(nextPoint);
                    gears.insert(pointsGears.begin(), pointsGears.end());
                    partNumber += c;
                    nextPoint = nextPoint.toRight();
                    c = schematic.at(nextPoint);
                }
                for(auto gear: gears){
                    gearToPartMapping[gear].push_back(std::stoul(partNumber));
                }
            }
        });
        std::vector<unsigned int> out;
        std::ranges::transform(gearToPartMapping, std::back_inserter(out), [](auto gp){
            return gp.second.size() == 2 ? gp.second[0] * gp.second[1] : 0U;
        });
        return out;
    }


    bool isPartNumber(cartesian::Point p) const{
        return std::ranges::any_of(p.getNeighbors(), [this](auto point){return schematic.at(point) != '.' && !isdigit(schematic.at(point)); });
    }

    std::vector<cartesian::Point> getGears(cartesian::Point p) const{
        auto filtered = std::views::filter(p.getNeighbors(), [this](auto point){ return schematic.at(point) == '*'; });
        return {filtered.begin(), filtered.end()};
    }
};

unsigned int getSumOfAllPartNumbers(const Schematic& schematic) {
    auto partNumbers = schematic.getPartNumbers();
    return std::accumulate(partNumbers.begin(), partNumbers.end(), 0U);
}

unsigned int getSumOfGearRatios(const Schematic& schematic){
    auto gearRatios = schematic.getGearRatios();
    return std::accumulate(gearRatios.begin(), gearRatios.end(), 0U);
}


int main(){
    std::fstream in("input/day3.txt");
    Schematic schematic;
    in >> schematic; 
    std::cout << getSumOfAllPartNumbers(schematic) << "\n";
    std::cout << getSumOfGearRatios(schematic) << "\n";
    return 0;
}