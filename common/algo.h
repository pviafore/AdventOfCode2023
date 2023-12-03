#include <algorithm>
#include <ranges>
namespace algo {
    // returns an iterator to within the haystack where the last element was found
    template <typename T, std::ranges::input_range R>
    std::ranges::subrange<typename T::const_iterator> search_last_of(T haystack, R&& needles) {
        auto lastFoundRange = std::ranges::subrange<typename T::const_iterator>(haystack.end(), haystack.end());
        for(const auto& needle: needles) {
            auto lastNeedleRange = std::ranges::find_end(haystack, needle);
            if(lastFoundRange.begin() == haystack.end() || (lastNeedleRange.begin() != haystack.end() && std::distance(haystack.begin(), lastNeedleRange.begin()) > std::distance(haystack.begin(), lastFoundRange.begin())) ){
                lastFoundRange = lastNeedleRange;
            }
        }
        return lastFoundRange;
    }
    
    template <typename T, std::ranges::input_range R>
    std::ranges::subrange<typename T::const_iterator> search_first_of(T haystack, R&& needles) {
        auto lastFoundRange = std::ranges::subrange<typename T::const_iterator>(haystack.end(), haystack.end());
        for(const auto& needle: needles) {
            auto lastNeedleRange = std::ranges::search(haystack, needle);
            if(lastFoundRange.begin() == haystack.end() || (lastNeedleRange.begin() != haystack.end() && std::distance(haystack.begin(), lastNeedleRange.begin()) < std::distance(haystack.begin(), lastFoundRange.begin())) ){
                lastFoundRange = lastNeedleRange;
            }
        }
        return lastFoundRange;
    }


}