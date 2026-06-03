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

TEST(TimerTest, StopWithoutWaitDoesNotBlock)
{
    std::atomic<int> counter{0};
    cutl::timer t("ut_timer_nowait",
                  [&counter]() {
                      counter.fetch_add(1);
                      // 故意让回调慢一些，验证 stop(false) 不会阻塞等待回调结束
                      std::this_thread::sleep_for(std::chrono::milliseconds(80));
                  },
                  std::chrono::milliseconds(20));
    t.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    auto begin = std::chrono::steady_clock::now();
    t.stop(false); // 不等待
    auto elapsed = std::chrono::steady_clock::now() - begin;
    EXPECT_LT(std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count(), 60);
    EXPECT_FALSE(t.is_running());
    // stop(false) 之后底层线程可能还在执行最后一次回调；等它结束后再 start()
    // 一次（start 会先 join 上一次的线程），最后再做一次同步 stop()，
    // 避免析构时残留 joinable 线程导致 std::terminate。
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    t.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    t.stop(); // wait_for_stop = true
}
