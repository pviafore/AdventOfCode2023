
#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <ranges>
#include <span>
#include <unordered_map>
#include <utility>

#include "common/algo.h"
#include "common/input.h"

std::vector<std::string> extendedDigits {
    "0", "1", "2","3","4","5","6","7","8","9",
    "zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"
};

std::unordered_map<std::string, unsigned int> digitMapping { 
    {"zero", 0},
    {"one", 1},
    {"two", 2},
    {"three", 3},
    {"four", 4},
    {"five", 5},
    {"six", 6},
    {"seven", 7},
    {"eight", 8},
    {"nine", 9},
};
std::string digitString = "0123456789";
const auto ZERO = std::ranges::find(extendedDigits, "zero");

unsigned int getCalibrationValue(std::string_view sv) {
    auto num1 = std::ranges::find_first_of(sv, digitString);
    auto num2 = std::ranges::find_first_of(sv | std::views::reverse, digitString);
    return (*num1- '0') * 10 + *num2 - '0';
}

template <std::ranges::forward_range R>
unsigned int toNum(R numberSubRange) {
    if(numberSubRange.size() == 1) {
        return *numberSubRange.begin() - '0';
    }
    auto digitWords = std::span{ZERO, extendedDigits.end()};
    for (const auto& digitWord: digitWords){
        if (std::ranges::search(numberSubRange, digitWord).begin() == numberSubRange.begin()) {
            return digitMapping.at(digitWord);
        }
    }
    throw std::runtime_error("Should not reach here");
    std::unreachable();
}

unsigned int getExtendedCalibrationValue(std::string_view sv) {
    auto num1Range = algo::search_first_of(sv, extendedDigits);
    auto num2Range = algo::search_last_of(sv, extendedDigits);
    return toNum(num1Range)*10 + toNum(num2Range);
}

unsigned int getTotalCalibrationValues(std::span<std::string> lines){
    return std::accumulate(lines.begin(), lines.end(), 0U, [](auto sum, auto line){ return sum + getCalibrationValue(line); });
}

unsigned int getTotalCalibrationValuesExtended(std::span<std::string> lines){
    return std::accumulate(lines.begin(), lines.end(), 0U, [](auto sum, auto line){ return sum + getExtendedCalibrationValue(line); });
}

int main() {
    std::vector<std::string> lines = input::readLinesFromFile("input/day1.txt");
    std::cout << getTotalCalibrationValues(lines) << "\n";
    std::cout << getTotalCalibrationValuesExtended(lines) << "\n";

}