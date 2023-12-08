#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <ranges>
#include <span>
#include <string>
#include <vector>

auto getValues(std::istream& is) {
    std::string line;
    getline(is, line);
    
    auto nums =  line | std::views::split(' ')
                     | std::views::transform([](auto s){ return std::string(s.begin(), s.end()); })
                     | std::views::filter([](auto s) { return !s.empty(); }) 
                     | std::views::drop(1) 
                     | std::views::transform([](auto s){ return std::stoul(std::string(s.begin(), s.end()));});
    return std::vector<unsigned int>{nums.begin(), nums.end()};
}

unsigned int getWaysToBeat(std::tuple<unsigned int, unsigned int> timeAndDistance) {
    auto [time, distance] = timeAndDistance;
    auto filtered = std::views::iota(1u, time) | std::views::transform([time](auto num){ return (time - num)* num; })
                                      | std::views::filter([distance](auto num){ return num > distance; });
    std::vector<unsigned int> chances{ filtered.begin(), filtered.end() };
    return chances.size();
}

unsigned int getWaysToBeatProduct(auto timesAndDistances) {
    return *std::ranges::fold_left_first(timesAndDistances | std::views::transform(getWaysToBeat), std::multiplies<>());
}

int64_t getWaysToBeatProductExtended(auto timesAndDistances){
    std::string timeString, distanceString;
    for(auto [time, distance]: timesAndDistances) {
        timeString += std::to_string(time);
        distanceString += std::to_string(distance);
    }
    int64_t time = std::stoll(timeString);
    int64_t distance = std::stoll(distanceString);

    auto answer1 = (-1*time + sqrt(time*time - 4*distance)) / 2;
    auto answer2 = (-1*time - sqrt(time*time - 4*distance)) / 2;

    return static_cast<int64_t>(std::floor(std::max(answer1,answer2))) - static_cast<int64_t>(std::ceil(std::min(answer1, answer2))) + 1;
}


int main() { 
    std::ifstream in("input/day6.txt");
    auto times = getValues(in);
    auto distances = getValues(in);
    auto timesAndDistances = std::views::zip(times, distances);
    std::cout << getWaysToBeatProduct(timesAndDistances) << "\n";

    std::cout << getWaysToBeatProductExtended(timesAndDistances) << "\n";

    return 0;
}