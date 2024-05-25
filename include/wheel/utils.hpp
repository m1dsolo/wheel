#pragma once

#include <algorithm>
#include <string>  // tolower

namespace wheel {


class Utils {
public:
    template <typename T>
    static T&& to_lower(T&& str) {
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
    }
};

}  // namespace wheel
