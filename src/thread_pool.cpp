#include <wheel/thread_pool.hpp>

namespace wheel {

ThreadPool::ThreadPool(const int num) : stop_(false) {
    for (int i = 0; i < num; ++i) {
        threads_.emplace_back([this] {
            while (true) {
                std::unique_lock<std::mutex> lock(mutex_);
                if (safe_queue_.empty()) {
                    cv_.wait(lock, [this] { return stop_ || !safe_queue_.empty(); });
                }
                if (stop_ && safe_queue_.empty()) {
                    return;
                }
                auto task = std::move(safe_queue_.front());
                safe_queue_.pop();
                lock.unlock();
                task();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    stop_ = true;
    cv_.notify_all();
    for (auto &t : threads_) {
        if (t.joinable()) 
            t.join();
    }
}

}  // namespace wheel
