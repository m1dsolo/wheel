#pragma once

#include <mutex>
#include <queue>

namespace wheel {

template <typename T>
class SafeQueue {
public:
    size_t size() {
        std::unique_lock<std::mutex> lock(mutex_);
        return queue_.size();
    }

    bool empty() {
        std::unique_lock<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    void push(const T& value) {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.push(value);
    }

    void pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.pop();
    }

    T& front() {
        std::unique_lock<std::mutex> lock(mutex_);
        return queue_.front();
    }

private:
    std::queue<T> queue_;
    std::mutex mutex_;
};

}  // namespace wheel
