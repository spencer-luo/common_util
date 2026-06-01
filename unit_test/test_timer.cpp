/**
 * @file test_timer.cpp
 * @brief Unit tests for cutl::set_timeout and cutl::timer.
 */

#include "common_util/timer.h"
#include <atomic>
#include <chrono>
#include <gtest/gtest.h>
#include <thread>

TEST(TimerTest, SetTimeoutFiresOnce)
{
    std::atomic<int> counter{0};
    cutl::set_timeout([&counter]() { counter.fetch_add(1); }, 50);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_EQ(counter.load(), 1);
}

TEST(TimerTest, PeriodicTimerCallsRepeatedly)
{
    std::atomic<int> counter{0};
    cutl::timer t("ut_timer", [&counter]() { counter.fetch_add(1); },
                  std::chrono::milliseconds(50));
    t.start();
    EXPECT_TRUE(t.is_running());
    std::this_thread::sleep_for(std::chrono::milliseconds(280));
    t.stop();
    EXPECT_FALSE(t.is_running());
    // 大约触发 5 次，留较宽松的容忍
    EXPECT_GE(counter.load(), 2);
    EXPECT_LE(counter.load(), 10);
}

TEST(TimerTest, StopThenStartAgain)
{
    std::atomic<int> counter{0};
    cutl::timer t("ut_timer_restart", [&counter]() { counter.fetch_add(1); },
                  std::chrono::milliseconds(40));
    t.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(120));
    t.stop();
    int after_first = counter.load();
    EXPECT_GE(after_first, 1);

    // 再次启动应该继续执行
    t.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(120));
    t.stop();
    EXPECT_GT(counter.load(), after_first);
}
