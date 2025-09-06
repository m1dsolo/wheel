#pragma once

#include <random>

namespace wheel {

class Random {
public:
    Random() = default;
    ~Random() = default;

    void set_seed(unsigned int seed);

    template <typename T>
    T uniform(T min, T max) {
        if constexpr (std::is_integral_v<T>) {
            std::uniform_int_distribution<T> distribution(min, max);
            return distribution(generator_);
        }
        else if constexpr (std::is_floating_point_v<T>) {
            std::uniform_real_distribution<T> distribution(min, max);
            return distribution(generator_);
        }
    }

private:
    std::mt19937 generator_;
};

}  // namespace wheel
