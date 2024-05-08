#include <wheel/thread_pool.hpp>

#include <gtest/gtest.h>

namespace wheel {

TEST(ThreadPool, Basic) {
    wheel::ThreadPool thread_pool(1);

    std::vector<std::future<int>> futures;
    for (int i = 0; i < 10; ++i) {
        futures.push_back(thread_pool.submit([](int a, int b) { return a + b; }, i, 1));
    }

    std::vector<int> results;
    for (auto& future : futures) {
        results.push_back(future.get());
    }

    ASSERT_EQ(results.size(), 10);
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(results[i], i + 1);
    }
}

}  // namespace wheel
