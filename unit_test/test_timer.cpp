/**
 * @file test_timer.cpp
 * @brief Unit tests for cutl::set_timeout and cutl::timer.
 */

#include "common_util/timer.h"
#include <atomic>
#include <chrono>
#include <gtest/gtest.h>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

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
    cutl::timer t("ut_tm_restart", [&counter]() { counter.fetch_add(1); },
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
    // stop(false) 之后回调可能还在执行。start() 会先 join 上一次的线程再启动。
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    t.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    t.stop(); // wait_for_stop = true
}

TEST(TimerTest, DestructorJoinsAfterStopWithoutWait)
{
    std::atomic<int> counter{ 0 };
    {
        cutl::timer t("ut_tm_dtor",
                      [&counter]() {
                          counter.fetch_add(1);
                          std::this_thread::sleep_for(std::chrono::milliseconds(80));
                      },
                      std::chrono::milliseconds(20));
        t.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        t.stop(false);
        EXPECT_FALSE(t.is_running());
    }
    EXPECT_GE(counter.load(), 1);
}

TEST(TimerTest, StopFromCallbackDoesNotDeadlock)
{
    std::atomic<int> counter{ 0 };
    cutl::timer* self = nullptr;
    cutl::timer t(
      "ut_tm_selfstop",
      [&counter, &self]() {
          counter.fetch_add(1);
          self->stop(true);
      },
      std::chrono::milliseconds(30));
    self = &t;
    t.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_FALSE(t.is_running());
    EXPECT_EQ(counter.load(), 1);
}

TEST(TimerTest, StartFromCallbackIsIgnored)
{
    std::atomic<int> counter{ 0 };
    cutl::timer* self = nullptr;
    cutl::timer t(
      "ut_tm_selfstart",
      [&counter, &self]() {
          counter.fetch_add(1);
          self->start();
      },
      std::chrono::milliseconds(40));
    self = &t;
    t.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    t.stop();
    EXPECT_GE(counter.load(), 2);
}

TEST(TimerTest, TaskExceptionDoesNotStopTimer)
{
    std::atomic<int> counter{ 0 };
    cutl::timer t(
      "ut_tm_throw",
      [&counter]() {
          const int n = counter.fetch_add(1);
          if (n == 0)
          {
              throw std::runtime_error("timer boom");
          }
      },
      std::chrono::milliseconds(40));
    t.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    t.stop();
    EXPECT_GE(counter.load(), 2);
}

TEST(TimerTest, SetTimeoutExceptionIsSwallowed)
{
    std::atomic<int> counter{ 0 };
    cutl::set_timeout(
      [&counter]() {
          counter.fetch_add(1);
          throw std::runtime_error("set_timeout boom");
      },
      30);
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    EXPECT_EQ(counter.load(), 1);
}

TEST(TimerTest, OverrunWaitsAnotherInterval)
{
    std::mutex stamps_mutex;
    std::vector<std::chrono::steady_clock::time_point> starts;
    cutl::timer t(
      "ut_tm_overrun",
      [&]() {
          {
              std::lock_guard<std::mutex> lock(stamps_mutex);
              starts.push_back(std::chrono::steady_clock::now());
          }
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
      },
      std::chrono::milliseconds(40));
    t.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(450));
    t.stop();

    std::lock_guard<std::mutex> lock(stamps_mutex);
    ASSERT_GE(starts.size(), 2u);
    for (size_t i = 1; i < starts.size(); ++i)
    {
        const auto gap =
          std::chrono::duration_cast<std::chrono::milliseconds>(starts[i] - starts[i - 1]);
        // 回调 100ms，超时后还要再等 40ms，两次开始之间应明显大于回调本身。
        EXPECT_GE(gap.count(), 120);
    }
}

TEST(TimerTest, NonPositiveIntervalDoesNotStart)
{
    cutl::timer t("ut_tm_zero", []() {}, std::chrono::microseconds(0));
    t.start();
    EXPECT_FALSE(t.is_running());
}

TEST(TimerTest, EmptyTaskDoesNotStart)
{
    cutl::timer t("ut_tm_empty", cutl::timer::Task{}, std::chrono::milliseconds(50));
    t.start();
    EXPECT_FALSE(t.is_running());
}

void AddDelta(std::atomic<int>* counter, int delta)
{
    counter->fetch_add(delta);
}

TEST(TimerTest, SetTimeoutForwardsArguments)
{
    std::atomic<int> counter{ 0 };
    cutl::set_timeout(AddDelta, 30, &counter, 3);
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    EXPECT_EQ(counter.load(), 3);
}

TEST(TimerTest, SetTimeoutUnknownExceptionIsSwallowed)
{
    std::atomic<int> counter{ 0 };
    cutl::set_timeout(
      [&counter]() {
          counter.fetch_add(1);
          throw 1;
      },
      30);
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    EXPECT_EQ(counter.load(), 1);
}

TEST(TimerTest, UnknownExceptionDoesNotStopTimer)
{
    std::atomic<int> counter{ 0 };
    cutl::timer t(
      "ut_tm_unknown",
      [&counter]() {
          const int n = counter.fetch_add(1);
          if (n == 0)
          {
              throw 1;
          }
      },
      std::chrono::milliseconds(40));
    t.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    t.stop();
    EXPECT_GE(counter.load(), 2);
}

TEST(TimerTest, NegativeIntervalDoesNotStart)
{
    cutl::timer t("ut_tm_neg", []() {}, std::chrono::microseconds(-1));
    t.start();
    EXPECT_FALSE(t.is_running());
}

TEST(TimerTest, StopBeforeStartAndSecondStopAreSafe)
{
    cutl::timer t("ut_tm_idle", []() {}, std::chrono::milliseconds(40));
    t.stop();
    t.stop(false);
    EXPECT_FALSE(t.is_running());

    t.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    t.stop();
    t.stop();
    EXPECT_FALSE(t.is_running());
}

TEST(TimerTest, SecondStartWhileRunningIsIgnored)
{
    std::mutex id_mutex;
    std::thread::id worker;
    std::atomic<int> other_threads{ 0 };
    std::atomic<int> counter{ 0 };
    cutl::timer t(
      "ut_tm_twice",
      [&]() {
          const auto self = std::this_thread::get_id();
          {
              std::lock_guard<std::mutex> lock(id_mutex);
              if (worker == std::thread::id())
              {
                  worker = self;
              }
              else if (worker != self)
              {
                  other_threads.fetch_add(1);
              }
          }
          counter.fetch_add(1);
      },
      std::chrono::milliseconds(40));
    t.start();
    t.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(160));
    t.stop();
    EXPECT_GE(counter.load(), 2);
    EXPECT_EQ(other_threads.load(), 0);
}

TEST(TimerTest, DestructorStopsRunningTimer)
{
    std::atomic<int> counter{ 0 };
    {
        cutl::timer t(
          "ut_tm_drun",
          [&counter]() {
              counter.fetch_add(1);
              std::this_thread::sleep_for(std::chrono::milliseconds(30));
          },
          std::chrono::milliseconds(20));
        t.start();
        EXPECT_TRUE(t.is_running());
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }
    const int stopped_at = counter.load();
    std::this_thread::sleep_for(std::chrono::milliseconds(80));
    EXPECT_GE(stopped_at, 1);
    EXPECT_EQ(counter.load(), stopped_at);
}

TEST(TimerTest, FastTaskKeepsFixedPeriod)
{
    std::mutex stamps_mutex;
    std::vector<std::chrono::steady_clock::time_point> starts;
    cutl::timer t(
      "ut_tm_rate",
      [&]() {
          {
              std::lock_guard<std::mutex> lock(stamps_mutex);
              starts.push_back(std::chrono::steady_clock::now());
          }
          std::this_thread::sleep_for(std::chrono::milliseconds(20));
      },
      std::chrono::milliseconds(80));
    t.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(360));
    t.stop();

    std::lock_guard<std::mutex> lock(stamps_mutex);
    ASSERT_GE(starts.size(), 3u);
    for (size_t i = 1; i < starts.size(); ++i)
    {
        const auto gap =
          std::chrono::duration_cast<std::chrono::milliseconds>(starts[i] - starts[i - 1]);
        // 任务 20ms 算在 80ms 周期内，两次开始大约间隔 80ms，而不是 100ms。
        EXPECT_GE(gap.count(), 60);
        EXPECT_LE(gap.count(), 100);
    }
}
