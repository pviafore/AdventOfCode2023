#include <array>
#include <algorithm>
#include <iostream>
#include <ranges>
#include <span>

#include "common/input.h"
const std::string ranks = "23456789TJQKA";
const std::string ranksWithJoker = "J23456789TQKA";
struct Hand { 
    std::array<char, 5> cards;
    int bid = 0;
    bool useJokers = false;

    friend std::istream& operator>>(std::istream& in, Hand & hand){
        std::string cards;
        int bid;
        in >> cards >> bid;
        std::ranges::copy_n(cards.begin(), 5, hand.cards.begin());
        hand.bid = bid; 
        return in;
    }

    friend std::ostream& operator<<(std::ostream& os, const Hand& hand){
        std::string cards = std::string { hand.cards.begin(), hand.cards.end() };
        os << cards << " "  << hand.bid << " " << hand.getHandStrength();
        return os;
    }

    unsigned int getHandStrength() const { 
        if(hasFiveOfAKind()) { return 6;}
        if(hasFourOfAKind()){ return 5;}
        if(hasFullHouse()){return 4;}
        if(hasThreeOfAKind()){ return 3;}
        if(hasTwoPair()) { return 2; }
        if(hasPair()) { return 1;}
        return 0;
    }

    bool containsCount(size_t count) const{
        return std::ranges::any_of(cards, [count, this](auto c){ return getCount(c) == count; });
    }

    size_t getCount(char character) const {
        return getCount(cards.begin(), character);
    }

    size_t getCount(std::array<char,5>::const_iterator start, char character) const{
        auto matches = [character, this](auto c){ return c == character || (c == 'J' && useJokers); };
        return std::ranges::count_if(start, cards.end(), matches);
    }

    bool hasFiveOfAKind() const { 
        return containsCount(5);
    }
    
    bool hasFourOfAKind() const { 
        return containsCount(4);
    }
    
    bool hasFullHouse() const { 
        auto noJokerHand = Hand{cards};
        noJokerHand.useJokers = false;
        if(useJokers && hasThreeOfAKind() && !noJokerHand.hasThreeOfAKind()){
            // used a joker for three of a kind
            return noJokerHand.hasTwoPair();
        }
        return hasThreeOfAKind() && hasPair(); 
    }
    
    bool hasThreeOfAKind() const { 
        return containsCount(3);
    }

    bool hasTwoPair() const { 
        if(useJokers && std::ranges::contains(cards, 'J')){
            return false; // we can't get two pair with a joker
        }
        return std::ranges::count(cards | std::views::transform([this](auto c){return getCount(c); }), 2) == 4;
    }
    
    bool hasPair() const { 
        return containsCount(2);
    }


    friend std::strong_ordering operator<=>(const Hand& lhs, const Hand& rhs) {
        if (auto cmp = lhs.getHandStrength() <=> rhs.getHandStrength(); cmp != 0){
            return cmp;
        }

        for(auto [lhsRank, rhsRank] : std::views::zip(lhs.cards, rhs.cards)){
            auto haystack = lhs.useJokers ? ranksWithJoker : ranks;
            if (auto cmp = haystack.find(lhsRank) <=> haystack.find(rhsRank); cmp != 0) {
                return cmp;
            }
        }
        return std::strong_ordering::equal;
    }
};

unsigned int getTotalWinnings(std::span<Hand> hands){
    return std::ranges::fold_left(hands | std::views::enumerate, 0U, [](auto sum, auto indexHand){
        auto [index, hand] = indexHand;
        return sum + hand.bid * (index+1);
    });
}



int main() {
    auto hands = input::readFromFile<Hand>("input/day7.txt");
    std::ranges::sort(hands, std::less<>{});
    std::cout << getTotalWinnings(hands) << "\n";

    for(auto& hand: hands){
        hand.useJokers = true;
    }
    std::ranges::sort(hands, std::less<>{});
    std::ranges::copy(hands, std::ostream_iterator<Hand>(std::cout, "\n"));
    std::cout << getTotalWinnings(hands) << "\n";


    return 0;
}