#include <wheel/thread_pool.hpp>

namespace wheel {

ThreadPool::ThreadPool(int num) {
    add_thread(num);
}

void ThreadPool::add_thread(int num) {
    for (int i = 0; i < num; ++i) {
        threads_.emplace_back([this] {
            while (true) {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait(lock, [this] { return stop_ || !task_queue_.empty(); });
                if (stop_ && task_queue_.empty()) {
                    return;
                }
                auto task = std::move(task_queue_.front());
                task_queue_.pop();
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
        if (t.joinable()) {
            t.join();
        }
    }
}

}  // namespace wheel
