#pragma once

#include <algorithm>
#include <functional>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_set>

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

    template <typename T>
    static std::vector<T> intersection(const std::vector<T>& a, const std::vector<T>& b, bool sorted = false) {
        std::vector<T> res;
        if (sorted) {
            std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(res));
        } else {
            std::unordered_set<T> st(a.begin(), a.end());
            std::copy_if(b.begin(), b.end(), std::back_inserter(res), [&st](const T& x) { return st.count(x); });
        }
        return res;
    }

    // boost hash_combine
    struct hash_pair {
        template <typename T, typename U>
        size_t operator() (const std::pair<T, U>& p) const {
            size_t h1 = std::hash<T>{}(p.first);
            size_t h2 = std::hash<U>{}(p.second);
            return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
        }
    };

    template<typename Callable>
    using get_callable_return_type = typename decltype(std::function{std::declval<Callable>()})::result_type;
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
