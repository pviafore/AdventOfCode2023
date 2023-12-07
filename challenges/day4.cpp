#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <istream>
#include <numeric>
#include <ranges>
#include <span>
#include <sstream>
#include <string_view>
#include <vector>

#include <common/input.h>

struct Card {

    std::vector<unsigned long> winners;
    std::vector<unsigned long> numbers;

    unsigned int getWins() const { 
        return std::ranges::count_if(numbers, [this](auto n){ return std::ranges::contains(winners,n); });
    }
    unsigned int getPoints() const {
        return static_cast<unsigned int>(pow(2, getWins() - 1));
    }

    friend std::istream& operator>>(std::istream& is, Card& card){
        card.winners.clear();
        card.numbers.clear();
        
        std::string line;
        getline(is, line);
        if (!line.empty()) {

            auto pipe = std::ranges::find(line, '|');
            assert(pipe != line.end());

            auto colon = std::ranges::find(line.begin(), pipe, ':');
            assert(colon != pipe);

            std::ranges::transform(std::views::split(std::string_view(colon+2, pipe-1), ' ') | std::views::filter([](auto s){return !s.empty();}), std::back_inserter(card.winners), [](auto r){ return std::stoul(std::string{r.begin(), r.end()}); });
            std::ranges::transform(std::views::split(std::string_view(pipe+2, line.end()), ' ')| std::views::filter([](auto s){return !s.empty();}), std::back_inserter(card.numbers), [](auto r){ return std::stoul(std::string{r.begin(), r.end()}); });
        }
        return is;
    }

};

unsigned int getTotalPoints(std::span<Card> cards){
    return std::accumulate(cards.begin(), cards.end(), 0U, [](auto sum, auto card){ return sum + card.getPoints(); });
}

unsigned int getNumberOfScratchTickets(std::span<Card> cards) {
    std::vector<unsigned int> tickets(cards.size(), 1U);
    for (auto index: std::views::iota(0U, cards.size())) {
        auto newTickets = cards[index].getWins();
        for (auto ticketToCopy: std::views::iota(index+1, index+1+newTickets)){
            tickets[ticketToCopy] += tickets[index];
        }
    }
    return std::accumulate(tickets.begin(), tickets.end(), 0U);

}

int main() {
    auto cards = input::readFromFile<Card>("input/day4.txt");
    std::cout << getTotalPoints(cards) << "\n";
    std::cout << getNumberOfScratchTickets(cards) << "\n";
    return 0;
}