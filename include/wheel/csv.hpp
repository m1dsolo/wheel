#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <functional>

#include <wheel/utils.hpp>

namespace wheel {

class Csv {
public:
    template <typename T = std::string>
    static std::vector<std::vector<T>> read(
        std::string_view csv_file_name,
        std::function<T(const std::string&)> transform = [](const std::string& s) { return s; }
    );
};

template <typename T>
std::vector<std::vector<T>> Csv::read(
    std::string_view csv_file_name,
    std::function<T(const std::string&)> transform
) {
    std::vector<std::vector<T>> res;
    std::ifstream file(csv_file_name.data());
    if (!file.good()) {
        return res;
    }

    std::string line;
    while (std::getline(file, line)) {
        res.emplace_back(Utils::split<T>(line, ',', transform));
    }

    file.close();

    return res;
}

}  // namespace wheel
