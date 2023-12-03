
#include <algorithm>
#include <fstream>
#include <iterator>
#include <string>
#include <string_view>
#include <vector>

namespace input {

    std::vector<std::string> readLinesFromFile(std::string_view fileName); 

    template<typename T>
    std::vector<T> readFromFile(std::string_view fileName){
        std::ifstream in(fileName.begin());
        std::vector<T> lines;
        std::copy(std::istream_iterator<T>(in), std::istream_iterator<T>(), std::back_inserter(lines));
        return lines;
    }
}