#include <algorithm>
#include <cassert>
#include <iostream>
#include <istream>
#include <ranges>
#include <sstream>

#include "common/input.h"
#include "common/cartesian.h"

auto findSmudge(const std::string& lhs, const std::string& rhs){
    auto [left, right] = std::ranges::mismatch(lhs, rhs);
    if(left == lhs.end()){
        return left;
    }
    auto[newLeft, newRight] = std::ranges::mismatch(left+1, lhs.end(), right+1, rhs.end());
    return newLeft == lhs.end() ? left : lhs.end();
}

unsigned int getLineOfReflection(auto lines, bool isSmudged){
    auto counter = 1U;
    assert(!lines.empty());
    for(auto iter = lines.begin() + 1; iter != lines.end(); ++iter, ++counter){
        size_t smudgesApplied = 0;
        auto leftIter = std::make_reverse_iterator(iter);
        auto rightIter = iter;
        bool matches = true;
        while(leftIter != lines.rend() && rightIter != lines.end() && matches){
            std::string left = *leftIter;
            std::string right = *rightIter;
            if(isSmudged){
                auto smudgePosition = findSmudge(left, right);
                if(smudgePosition != left.end()){
                    left[std::distance(left.cbegin(), smudgePosition)] = (*smudgePosition == '.') ? '#' : '.';
                    smudgesApplied++; 
                }
            }
            if(smudgesApplied == 2){
                matches = false;
            }
            matches = matches && (left == right);
            leftIter++;
            rightIter++;
        }
        if(matches && (!isSmudged || smudgesApplied == 1)) {
            return counter;
        }
    }
    return 0;
 
}

struct Pattern {
    cartesian::TextGrid grid;

    friend std::istream& operator>>(std::istream& is, Pattern& pattern){
        std::string line = "ignored";
        std::string buf;
        do {
            getline(is, line);
            buf += line + "\n";
        } while(is.good() && !line.empty());

        pattern.grid = cartesian::TextGrid();
        std::stringstream ss(buf);
        ss >> pattern.grid;
        return is;
    }

    unsigned int getColumnOfReflection() const {
        return getLineOfReflection(grid.getColumns(), smudged);
    }

    unsigned int getRowOfReflection() const {
        return getLineOfReflection(grid.getLines(), smudged);
    }

    unsigned int getReflection() const {
        //std::cout << getColumnOfReflection() << " " << getRowOfReflection() << "\n";
        return getColumnOfReflection() + 100*getRowOfReflection();
    }

    void smudge() {
        smudged = true;
    }

    bool smudged = false;

};

int main() {
    auto patterns = input::readFromFile<Pattern>("input/day13.txt");
    std::cout << std::ranges::fold_left(patterns, 0U, [](auto sum, auto pattern) { return sum + pattern.getReflection(); }) << "\n";

    for(auto &pattern: patterns){
        pattern.smudge();
    }
    std::cout << std::ranges::fold_left(patterns, 0U, [](auto sum, auto pattern) { return sum + pattern.getReflection(); }) << "\n";
    return 0;
}