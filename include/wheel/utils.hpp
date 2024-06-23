#pragma once

#include <algorithm>
#include <functional>
#include <string>
#include <sstream>
#include <vector>

namespace wheel {

class Utils {
public:
    template <typename T>
    static T&& to_lower(T&& str);

    template <typename T = std::string>
    static std::vector<T> split(
        std::string_view sv,
        char delim,
        std::function<T(const std::string&)> transform = [](const std::string& s) { return s; }
    );

    static std::string trim(const std::string& str);
};

template <typename T>
T&& Utils::to_lower(T&& str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

template <typename T>
std::vector<T> Utils::split(
    std::string_view sv,
    char delim,
    std::function<T(const std::string&)> transform
) {
    std::vector<T> res;
    std::stringstream ss(sv.data());
    std::string item;
    while (std::getline(ss, item, delim)) {
        if (item.length()) {
            res.emplace_back(transform(item));
        }
    }

    return res;
}

}  // namespace wheel
