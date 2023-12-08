#include <iterator>

#include "input.h"
namespace input {

    std::vector<std::string> readLinesFromFile(const std::string& fileName) {
        return readFromFile<std::string>(fileName);
    }
}