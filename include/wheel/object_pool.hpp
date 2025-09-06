#pragma once

#include <array>
#include <cstddef>

namespace wheel {

template <typename T, int N>
class ObjectPool {
public:
    ObjectPool() = default;
    ~ObjectPool() = default;

    T* allocate() {
        if (top_) {
            return stack_[--top_];
        }
        if (p < N) {
            return &data_[p++];
        }

        throw std::bad_alloc();
    }

    void deallocate(void* p) {
        stack_[top_++] = static_cast<T*>(p);
    }

private:
    T data_[N];
    std::array<T*, N> stack_;
    size_t p = 0;
    size_t top_ = 0;
};

}  // namespace wheel
