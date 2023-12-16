#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <istream>
#include <numeric>
#include <ranges>
#include <sstream>
#include <utility>
#include <vector>

#include <common/input.h>


struct Readings {
    std::vector<int> values;

    friend std::istream& operator>>(std::istream& is, Readings& readings){
        readings.values.clear();

        std::string line;
        getline(is, line);
        std::istringstream iss(line);
        std::copy(std::istream_iterator<int>(iss), {}, std::back_inserter(readings.values));
        return is;
    }

    unsigned int getExtrapolation() const {
        std::vector<std::vector<int>> readingsStack { values};
        std::vector<int> temp { values };
        while(!values.empty()) {
            std::vector<int> subReadings;
            subReadings.reserve(temp.size() - 1);
            std::adjacent_difference(temp.begin(), temp.end(), std::back_inserter(subReadings));
            subReadings.erase(subReadings.begin());
            if(std::ranges::all_of(subReadings, [](auto val){ return val == 0; })){
                int value = 0;
                while(!readingsStack.empty()) {
                    value += readingsStack.back().back();
                    readingsStack.erase(readingsStack.end() - 1);
                }

                return value;

            }
            readingsStack.push_back(subReadings);
            std::swap(temp, subReadings);
        }

        std::unreachable();
        assert(false);
        
    }

    int64_t getLeftExtrapolation() const {
        std::vector<std::vector<int>> readingsStack { values};
        std::vector<int> temp { values };
        while(!values.empty()) {
            std::vector<int> subReadings;
            subReadings.reserve(temp.size());
            std::adjacent_difference(temp.begin(), temp.end(), std::back_inserter(subReadings));
            subReadings.erase(subReadings.begin());
            if(std::ranges::all_of(subReadings, [](auto val){ return val == 0; })){
                int64_t value = 0;
                while(!readingsStack.empty()) {
                    value = readingsStack.back().front() - value;
                    readingsStack.erase(readingsStack.end() - 1);
                }

                return value;

            }
            readingsStack.push_back(subReadings);
            std::swap(temp, subReadings);
        }

        std::unreachable();
        assert(false);
        
    }
};

int main() {

    auto readings = input::readFromFile<Readings>("input/day9.txt");
    auto sumOfExtrapolations = std::ranges::fold_left_first(readings | std::views::transform([](auto r){ return r.getExtrapolation(); }), std::plus<>());
    std::cout << sumOfExtrapolations.value() << "\n";
    auto sumOfLeftExtrapolations = std::ranges::fold_left_first(readings | std::views::transform([](auto r){ return r.getLeftExtrapolation(); }), std::plus<>());
    std::cout << sumOfLeftExtrapolations.value() << "\n";
    return 0;

}