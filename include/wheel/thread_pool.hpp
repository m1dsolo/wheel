#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <thread>
#include <vector>

#include <wheel/safe_queue.hpp>

namespace wheel {

class ThreadPool {
public:
    explicit ThreadPool(const int num);
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    template<typename F, typename ...Args>
    auto submit(F&& f, Args&& ...args) -> std::future<decltype(f(args...))> {
        auto func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);
        safe_queue_.push([task_ptr] { (*task_ptr)(); });
        cv_.notify_one();

        return task_ptr->get_future();
    }

private:
    std::atomic<bool> stop_;
    SafeQueue<std::function<void()>> safe_queue_;
    std::vector<std::thread> threads_;
    std::condition_variable cv_;
    std::mutex mutex_;
};

}  // namespace wheel
