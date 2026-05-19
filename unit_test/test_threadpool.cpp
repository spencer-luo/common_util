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

TEST(ThreadPoolTest, AddTaskWithTimeoutSuccess)
{
    cutl::threadpool tp("ut_tp_timeout_ok", 8);
    tp.start(1);
    std::atomic<int> counter{0};
    auto ok = tp.add_task([&counter]() { counter.fetch_add(1); },
                          std::chrono::milliseconds(200));
    EXPECT_TRUE(ok);
    for (int i = 0; i < 100 && counter.load() < 1; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    EXPECT_EQ(counter.load(), 1);
    tp.stop();
}

TEST(ThreadPoolTest, AddTaskWithTimeoutAfterStopReturnsFalse)
{
    // 线程池停止后再投递任务，无论是否带超时，都应返回 false
    cutl::threadpool tp("ut_tp_timeout_stopped", 8);
    tp.start(1);
    tp.stop();
    auto ok = tp.add_task([]() {}, std::chrono::milliseconds(50));
    EXPECT_FALSE(ok);
}

TEST(ThreadPoolTest, AddTaskWithTimeoutTimesOutWhenQueueFull)
{
    // 队列容量 1，且线程一直被一个长任务阻塞，再投递一个任务 + 短超时，应返回 false
    cutl::threadpool tp("ut_tp_timeout_full", /*max_task_size=*/1);
    tp.start(1);

    std::atomic<bool> release{false};
    EXPECT_TRUE(tp.add_task([&release]() {
        while (!release.load())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }));
    // 此时唯一的线程正在执行长任务，再投递的任务会进入队列
    EXPECT_TRUE(tp.add_task([]() {}));
    // 队列已满（容量 1），再投递必定阻塞，配合短超时应返回 false
    auto ok = tp.add_task([]() {}, std::chrono::milliseconds(50));
    EXPECT_FALSE(ok);

    release.store(true);
    tp.stop();
}
