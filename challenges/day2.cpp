#include <algorithm>
#include <cassert>
#include <istream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <ranges>
#include <regex>
#include <unordered_map>
#include <utility>
#include <vector>

#include "common/input.h"

std::pair<std::string_view, unsigned int> parseBallsString(auto s){
    auto space = std::ranges::find(s, ' ');
    assert(space != s.end());
    return {std::string_view{space+1, s.end()}, std::stoi(std::string{s.begin(), space})};
}

using namespace std::string_literals;
struct Draw {
    Draw(std::string_view text) {
        for (auto balls: std::views::split(text, ", "s)) {
            auto [color, number] = parseBallsString(balls);
            switch (color[0]){
                case 'r': redBalls = number;break;
                case 'g': greenBalls = number;break;
                case 'b': blueBalls = number;break;
            }
        }
    }

    bool isValid() const {
        return redBalls <= 12 && greenBalls <= 13 && blueBalls <= 14;
    }
    unsigned int redBalls = 0;
    unsigned int greenBalls = 0;
    unsigned int blueBalls = 0;

};
struct Game {
    unsigned long id = 0U;
    std::vector<Draw> draws;

    friend std::istream& operator>>(std::istream& is, Game& game) {
        std::string inputString;
        getline(is, inputString);
        if(inputString.empty()){
            is.setstate(std::ios::eofbit);
            return is;
        }
        std::regex r { R"(Game (\d+): (.*))"};
        std::smatch match;
        auto result = std::regex_search(inputString, match, r);
        assert(result && match.size() > 1);
        game = Game{std::stoul(match[1])};
        auto split = match[2].str() | std::views::split("; "s)
                                    | std::views::transform([](auto s){ return std::string_view(s.begin(), s.end()); });
        std::ranges::transform(split, std::back_inserter(game.draws), [](auto s){return Draw(s); }); 
        return is;
    }

    unsigned int getPowerSet() const { 
        unsigned int maxRed = std::ranges::max(draws | std::views::transform([](auto draw){ return draw.redBalls;})); 
        unsigned int maxBlue = std::ranges::max(draws | std::views::transform([](auto draw){ return draw.blueBalls;}));
        unsigned int maxGreen = std::ranges::max(draws | std::views::transform([](auto draw){ return draw.greenBalls;}));
        return maxRed * maxBlue * maxGreen;
    }
    
    bool isValid() const {
        return std::ranges::all_of(draws, [](auto d){ return d.isValid(); });
    }

};

unsigned int getSumOfValidGames(std::span<Game> games){
    auto validGames = std::views::filter(games, [](auto g) { return g.isValid(); });
    return std::accumulate(validGames.begin(), validGames.end(), 0U, [](auto sum, auto game) { return sum + game.id; });
}

unsigned int getCombinedPowerSet(std::span<Game> games){
    return std::accumulate(games.begin(), games.end(), 0U, [](auto sum, auto game) { return sum + game.getPowerSet(); });
}

#include <chrono>
int main() {
    auto games = input::readFromFile<Game>("input/day2.txt");
    std::cout << getSumOfValidGames(games) << "\n";
    std::cout << getCombinedPowerSet(games) << "\n";
    return 0;
}