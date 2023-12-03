#include <iterator>

#include "input.h"
namespace input {

    std::vector<std::string> readLinesFromFile(std::string_view fileName) {
        return readFromFile<std::string>(fileName);
    }
}