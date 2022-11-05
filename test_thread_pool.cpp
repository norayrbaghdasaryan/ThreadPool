#include "gtest/gtest.h"
#include "thread_pool.hpp"
#include <future>
#include <chrono>

TEST(ThreadPool, execution2) {
    constexpr size_t THREAD_COUNT = 25;
    // create thread pool consisting of 25 threads
    ThreadPool pool (THREAD_COUNT);
    constexpr size_t JOB_COUNT = 1000;
    std::promise<int> p[JOB_COUNT];
    // add jobs in thread pool queue
    for (int i = 0; i < JOB_COUNT; ++i) {
        pool.addJob([&p, i]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            p[i].set_value(i);
        });
    }
    for (int i = 0; i < JOB_COUNT; ++i) {
        std::future f = p[i].get_future();
        EXPECT_EQ(f.get(), i);
    }
}