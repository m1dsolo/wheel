// TODO: thread safety
#pragma once

#include <span>
#include <cstring>
#include <cstdint>

namespace wheel {

template <typename T, uint32_t N = 2048>
class RingBuffer {
    static_assert((N & (N - 1)) == 0, "N must be a power of 2");

public:
    uint32_t size() {
        return in_ - out_;
    }

    uint32_t peek(std::span<T> buf, uint32_t size) {
        size = std::min(size, this->size());
        auto first_size = std::min(size, N - (out_ & (N - 1)));
        memcpy(buf.data(), buf_ + (out_ & (N - 1)), first_size * sizeof(T));
        memcpy(buf.data() + first_size, buf_, (size - first_size) * sizeof(T));
        return size;
    }

    uint32_t get(std::span<T> buf, uint32_t size) {
        peek(buf, size);
        out_ += size;
        return size;
    }

    uint32_t put(std::span<T> buf, uint32_t size) {
        size = std::min(size, N - this->size());
        auto first_size = std::min(size, N - (in_ & (N - 1)));
        memcpy(buf_ + (in_ & (N - 1)), buf.data(), first_size * sizeof(T));
        memcpy(buf_, buf.data() + first_size, (size - first_size) * sizeof(T));
        in_ += size;
        return size;
    }

    uint32_t pop(uint32_t size) {
        size = std::min(size, this->size());
        out_ += size;
        return size;
    }

private:
    T buf_[N];
    uint32_t in_ = 0;
    uint32_t out_ = 0;
};

}  // namespace wheel
