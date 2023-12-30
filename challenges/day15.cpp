#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <ranges>



unsigned char toHash(auto range) {
    return std::ranges::fold_left(range, static_cast<unsigned char>(0U), [](auto sum, auto c){
        return (sum + c)*17;
    });
}

size_t getSumOfHashes(const std::string& line){
    auto hashes = std::views::split(line, ',') 
                    | std::views::transform([](auto r) { return static_cast<size_t>(toHash(r));});
                    
    return std::ranges::fold_left_first(hashes, std::plus<>{}).value();
}

class Boxes {
public:
    Boxes(const std::string& line){
        for(auto box: std::views::split(line, ',')) {
            modify(std::string(box.begin(), box.end()));
        }
    }

    void modify(const std::string& box){
        if(box.ends_with('-')){
            std::string label{box.begin(), box.end() - 1};
            auto boxNumber = toHash(label);
            remove(boxNumber, label);
        }
        else {
            auto equals = box.find('=');
            std::string label {box.begin(), box.begin()+equals};
            auto boxNumber = toHash(label);
            char lens = box[equals+1] - '0';
            insert(boxNumber, label, lens);
        }
    }

    void remove(unsigned char boxNumber, const std::string& label){
        std::erase_if(boxes[boxNumber], [label](auto element){ return element.first == label; });
    }
    
    void insert(unsigned char boxNumber, const std::string& label, char lens){
        auto box = std::ranges::find_if(boxes[boxNumber], [label](auto element){ return element.first == label; });
        if(box == boxes[boxNumber].end()){
            boxes[boxNumber].emplace_back(label, lens);
        }
        else {
            box->second = lens;
        }
    }

    size_t getFocusingPower(unsigned char boxNumber) const { 
        return std::ranges::fold_left(std::views::enumerate(boxes[boxNumber]), 0U, [this, boxNumber](auto sum, auto indexAndLens) { 
            auto [slot, lens] = indexAndLens; 
            auto [label, focalLength] = lens;
            return sum + (1+boxNumber) * (slot+1) * focalLength;
        });
    }

    size_t getTotalFocusingPower() const {
        return std::ranges::fold_left(std::views::iota(0,256), 0U, [this](auto sum, auto boxNumber){ return sum + getFocusingPower(boxNumber); });
    }

private:
    std::array<std::vector<std::pair<std::string, char>>, 256> boxes;
};

int main() {
    std::fstream in{"input/day15.txt"};
    std::string line;
    getline(in, line);
    std::cout << getSumOfHashes(line) << "\n";

    Boxes boxes{line};
    std::cout << boxes.getTotalFocusingPower() << "\n";
    return 0;
}
