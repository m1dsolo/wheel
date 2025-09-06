#pragma once

namespace wheel {

// remember
// 1. private constructor(no parameter)
// 2. delete copy constructor
// 3. friend Singleton<T>
template <typename T>
class Singleton {
public:
    static T& instance() {
        static T instance;
        return instance;
    }

protected:
    Singleton() = default;
    virtual ~Singleton() = default;
};

}  // namespace wheel
