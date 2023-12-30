#include <algorithm>
#include <cassert>
#include <functional>
#include <fstream>
#include <iostream>
#include <istream>
#include <iterator>
#include <ranges>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

struct Part {
    size_t x, m, a,s;

    size_t getRating() const {
        return x + m + a + s;
    }

    friend std::istream& operator>>(std::istream& is, Part& part){
        std::string line;
        getline(is, line);
        if(line.empty()){
            return is;
        }
        auto equals = std::ranges::find(line, '=');
        auto nextToken = std::ranges::find(equals, line.end(), ',');
        part.x = std::stoul(std::string(equals+1, nextToken));
        equals = std::ranges::find(equals+1, line.end(), '=');
        nextToken = std::ranges::find(equals, line.end(), ',');
        part.m = std::stoul(std::string(equals+1, nextToken));
        equals = std::ranges::find(equals+1, line.end(), '=');
        nextToken = std::ranges::find(equals, line.end(), ',');
        part.a = std::stoul(std::string(equals+1, nextToken));
        equals = std::ranges::find(equals+1, line.end(), '=');
        nextToken = std::ranges::find(equals, line.end(), '}');
        part.s = std::stoul(std::string(equals+1, nextToken));
        return is;
    }
};

auto get_x = [](Part p){ return p.x; };
auto get_m = [](Part p){ return p.m; };
auto get_a = [](Part p){ return p.a; };
auto get_s = [](Part p){ return p.s; };

std::unordered_map<char, std::function<size_t(Part)>> partAccessorLookup {
    {'x', get_x},
    {'m', get_m},
    {'a', get_a},
    {'s', get_s}
};

struct Accept{};
struct Reject{};

struct Checker {
    char accessorVal;
    char comparator;
    size_t threshold;
    std::variant<Accept,Reject,std::string> destination;

    
};

using Destination = std::variant<Accept, Reject, std::string>;
using Rule = std::variant<Checker, Destination>;
Destination toDestination(const std::string& text){
    if(text == "A"){
        return Accept{};
    }
    if(text == "R"){
        return Reject{};
    }
    return text;
}

Rule parseRule(std::string text){
    if(text.ends_with('}')){
        text = text.substr(0, text.size()-1);
    }
    auto lt = std::ranges::find(text, '<');
    auto gt = std::ranges::find(text, '>');
    if(lt == text.end() && gt == text.end()){
        return std::visit([](auto arg){ return Rule{arg}; }, toDestination(text));
    }

    auto checker = text[0];
    auto comparator = lt == text.end() ? '>' : '<';
    auto rest = std::string((lt == text.end() ? gt : lt)+1, text.end());
    auto colon = std::ranges::find(rest, ':');
    auto threshold = std::stoul(std::string(rest.begin(), colon));
    auto destination = std::string(colon+1, rest.end());
    return Checker{checker, comparator, threshold, toDestination(destination)};
}

using Range = std::pair<size_t, size_t>;

Range updateRange(Range range, char comparator, size_t threshold) {
    if(comparator == '>'){
        return {std::max(range.first, threshold + 1), range.second};
    }
    return {range.first, std::min(threshold - 1, range.second )};
}

Range excludeRange(Range range, Range excluded){
    if(excluded.first > range.first && excluded.first < range.second){
        return {range.first, excluded.first-1};
    }
    else if(excluded.second > range.first && excluded.second < range.second){
        return {excluded.second+1, range.second};
    }
    else if (range.second > excluded.first && range.second < excluded.second) {
        return {range.first, excluded.first - 1};
    }
    return range;
}

size_t size(Range range){
    return range.second - range.first + 1;
}

struct Machine {
    std::unordered_map<std::string, std::vector<Rule>> workflows;
    std::vector<Part> parts;

    friend std::istream& operator>>(std::istream& is, Machine& machine) {
        std::string line = "ignored";
        while(!line.empty()){
            getline(is, line);
            if(!line.empty()){
                auto openingBrace = std::ranges::find(line, '{');
                std::string name{line.begin(), openingBrace};
                std::string rest{openingBrace+1, line.end()};
                auto rulesSplit = std::views::split(rest, ',');
                std::vector<Rule> rules;
                auto toRule = [](auto r){ return parseRule(std::string{r.begin(), r.end()}); };
                std::ranges::transform(rulesSplit, std::back_inserter(rules), toRule );
                machine.workflows[name] = rules;
            }
        }

        std::copy(std::istream_iterator<Part>{is}, std::istream_iterator<Part>{}, std::back_inserter(machine.parts));
        return is;
    }

    bool isAccepted(Part part) const{
        std::string workflowLocation = "in";
        while(true){
            for(auto workflow: workflows.at(workflowLocation)){
                if(std::holds_alternative<Checker>(workflow)) {
                    auto [accessorChar, comparatorChar, threshold, destination] = std::get<Checker>(workflow);
                    auto comparator = comparatorChar == '<' ? (std::function<bool(size_t, size_t)> { std::less<size_t>{} }) : std::greater<size_t>{};
                    auto accessor = partAccessorLookup[accessorChar];
                    if(comparator(accessor(part), threshold)) {
                        if(std::holds_alternative<Accept>(destination)){
                            return true;
                        }
                        else if(std::holds_alternative<Reject>(destination)){
                            return false;
                        }
                        else {
                            workflowLocation = std::get<std::string>(destination);
                            break;
                        }
                    }
                }
                if (std::holds_alternative<Destination>(workflow)){
                    auto destination = std::get<Destination>(workflow);
                    if(std::holds_alternative<Accept>(destination)){
                        return true;
                    }
                    else if(std::holds_alternative<Reject>(destination)){
                        return false;
                    }
                    else {
                        workflowLocation = std::get<std::string>(destination);
                        break;
                    }
                }
            }
        }
        std::unreachable();
        assert(false);

    }


    size_t getTotalAcceptedRatings() const {
        auto acceptedParts = std::views::filter(parts, [this](auto p){ return isAccepted(p); });
        return std::ranges::fold_left(acceptedParts, 0U, [](auto sum ,auto part) { return sum + part.getRating(); });
    }

    struct Candidate {
        std::string location;
        Range xrange { 1, 4000};
        Range mrange { 1, 4000};
        Range arange { 1, 4000};
        Range srange { 1, 4000};
    };

    size_t getCombinations() const {
        std::vector<Candidate> candidates { {"in"}};
        size_t totalSize = 0U;
        while(!candidates.empty()){
            auto [location, possibleXs, possibleMs, possibleAs, possibleSs] = candidates.back();
            candidates.pop_back();
            if(possibleXs.second < possibleXs.first || possibleMs.second < possibleMs.first || possibleAs.second < possibleAs.first || possibleSs.second < possibleSs.first){
                continue;
            }
            for(auto workflow: workflows.at(location)){
                if(std::holds_alternative<Checker>(workflow)){
                    auto [accessorChar, comparatorChar, threshold, destination] = std::get<Checker>(workflow);
                    if(accessorChar == 'x') {
                        auto newRange = updateRange(possibleXs, comparatorChar, threshold);
                        if(std::holds_alternative<Accept>(destination)) {
                            totalSize += size(newRange) * size(possibleMs) * size(possibleAs) * size(possibleSs);
                        }
                        else if (std::holds_alternative<Reject>(destination)) {
                            // pass
                        }
                        else {
                            candidates.emplace_back(std::get<std::string>(destination), newRange, possibleMs, possibleAs, possibleSs );
                        }
                        possibleXs = excludeRange(possibleXs, newRange);
                    }
                    else if(accessorChar == 'm') {
                        auto newRange = updateRange(possibleMs, comparatorChar, threshold);
                        if(std::holds_alternative<Accept>(destination)) {
                            totalSize += size(possibleXs) * size(newRange) * size(possibleAs) * size(possibleSs);
                        }
                        else if (std::holds_alternative<Reject>(destination)) {
                            //pass
                        }
                        else {
                            candidates.emplace_back(std::get<std::string>(destination), possibleXs, newRange, possibleAs, possibleSs );
                        }
                        possibleMs = excludeRange(possibleMs, newRange);
                    }
                    else if(accessorChar == 'a') {
                        auto newRange = updateRange(possibleAs, comparatorChar, threshold);
                        if(std::holds_alternative<Accept>(destination)) {
                            totalSize += size(possibleXs) * size(possibleMs) * size(newRange) * size(possibleSs);
                        }
                        else if (std::holds_alternative<Reject>(destination)) {
                            //pass
                        }
                        else {
                            candidates.emplace_back(std::get<std::string>(destination), possibleXs, possibleMs, newRange, possibleSs );
                        }
                        possibleAs = excludeRange(possibleAs, newRange);
                    }
                    else if(accessorChar == 's') {
                        auto newRange = updateRange(possibleSs, comparatorChar, threshold);
                        if(std::holds_alternative<Accept>(destination)) {
                            totalSize += size(possibleXs) * size(possibleMs) * size(possibleAs) * size(newRange);
                        }
                        else if (std::holds_alternative<Reject>(destination)) {
                            //passs
                        }
                        else {
                            candidates.emplace_back(std::get<std::string>(destination), possibleXs, possibleMs, possibleAs, newRange);
                        }
                        possibleSs = excludeRange(possibleSs, newRange);
                    }
                    else { assert(false); }
                }
                else {
                    auto destination = std::get<Destination>(workflow);
                    if(std::holds_alternative<Accept>(destination)){
                        totalSize += size(possibleXs) * size(possibleMs) * size(possibleAs) * size(possibleSs);
                    }
                    else if(std::holds_alternative<Reject>(destination)){
                        continue;
                    }
                    else {
                        candidates.emplace_back(std::get<std::string>(destination), possibleXs, possibleMs, possibleAs, possibleSs);
                    }
                }
            }
        }

        return totalSize;


    }

};

int main(){
    std::fstream in{"input/day19.txt"};
    Machine machine;
    in >> machine;
    assert(excludeRange(Range(1,4000), Range(1,2)) == Range(3,4000));
    assert(excludeRange(Range(1,4000), Range(0,2)) == Range(3,4000));
    assert(excludeRange(Range(1,4000), Range(3998,4000)) == Range(1,3997));
    assert(excludeRange(Range(1,4000), Range(2,4000)) == Range(1,1));
    assert(excludeRange(Range(1,4000), Range(2,4005)) == Range(1,1));
    std::cout << machine.getTotalAcceptedRatings() << "\n";
    std::cout << machine.getCombinations() << "\n";
    return 0;
}