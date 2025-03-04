#include <wheel/thread_pool.hpp>

#include <gtest/gtest.h>

namespace wheel {

TEST(ThreadPoolTest, Basic) {
    wheel::ThreadPool thread_pool(4);

    std::vector<std::future<int>> futures;
    for (int i = 0; i < 10000; ++i) {
        futures.push_back(thread_pool.submit([](int a, int b) { return a + b; }, i, 1));
    }

    std::vector<int> results;
    for (auto& future : futures) {
        results.push_back(future.get());
    }

    ASSERT_EQ(results.size(), 10000);
    int res = 0;
    for (int i = 0; i < 10000; ++i) {
        res += results[i];
    }
    ASSERT_EQ(res, 50005000);
}

}  // namespace wheel
