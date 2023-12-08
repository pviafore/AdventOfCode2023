
#include <algorithm>
#include <fstream>
#include <iterator>
#include <string>
#include <string_view>
#include <vector>

namespace input {

    std::vector<std::string> readLinesFromFile(const std::string& fileName); 

    template<typename T>
    std::vector<T> readFromFile(const std::string& fileName){
        std::ifstream in(fileName.c_str());
        std::vector<T> lines;
        std::copy(std::istream_iterator<T>(in), std::istream_iterator<T>(), std::back_inserter(lines));
        return lines;
    }
}