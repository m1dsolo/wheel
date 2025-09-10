#pragma once

#include <wheel/singleton.hpp>

#include <random>

namespace wheel {

class Random : public Singleton<Random> {
    friend class Singleton<Random>;
     
public:
    void set_seed(unsigned int seed) {
        generator_.seed(seed);
    }

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
    Random() : generator_(std::random_device{}()) {}
    Random(const Random&) = delete;

    std::mt19937 generator_;
};

}  // namespace wheel
