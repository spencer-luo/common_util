/**
 * @file test_threadpool.cpp
 * @brief Unit tests for cutl::threadpool.
 */

#include "common_util/threadpool.h"
#include <atomic>
#include <chrono>
#include <gtest/gtest.h>
#include <thread>

TEST(ThreadPoolTest, ExecutesAllTasks)
{
    cutl::threadpool tp("ut_tp", 64);
    tp.start(2);

    std::atomic<int> counter{0};
    constexpr int kTotal = 50;
    for (int i = 0; i < kTotal; ++i)
    {
        EXPECT_TRUE(tp.add_task([&counter]() { counter.fetch_add(1); }));
    }

    // 等待所有任务执行完
    for (int waited = 0; waited < 100 && counter.load() < kTotal; ++waited)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    tp.stop();
    EXPECT_EQ(counter.load(), kTotal);
}

TEST(ThreadPoolTest, AddTaskWithArgsAndReturn)
{
    cutl::threadpool tp("ut_tp_ret", 16);
    tp.start(1);

    auto f = tp.add_task_with_args_and_return([](int a, int b) { return a + b; }, 3, 4);
    EXPECT_EQ(f.get(), 7);

    auto f2 = tp.add_task_with_args_and_return(
      [](const std::string& name) { return std::string("Hello, ") + name; }, std::string("Spencer"));
    EXPECT_EQ(f2.get(), "Hello, Spencer");

    tp.stop();
}
