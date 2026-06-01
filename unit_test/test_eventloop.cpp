/**
 * @file test_eventloop.cpp
 * @brief Unit tests for cutl::eventloop / singlethread_eventloop / multithread_eventloop.
 *
 * 关于 start() 的阻塞行为：
 * - `singlethread_eventloop::start()` 内部启动独立线程后立即返回，可在主线程中调用。
 * - `eventloop::start()` 与 `multithread_eventloop::start()` 会阻塞当前线程直到 stop()
 *   被调用，因此在测试中需要把它们放在独立线程里。
 */

#include "common_util/eventloop.h"
#include <atomic>
#include <chrono>
#include <gtest/gtest.h>
#include <thread>

namespace
{

// 等待若干毫秒，确保后台线程已经把 is_running 置为 true。
void wait_for_running(const cutl::eventloop& loop, int timeout_ms = 200)
{
    auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);
    while (std::chrono::steady_clock::now() < deadline &&
           !const_cast<cutl::eventloop&>(loop).is_running())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

} // namespace

TEST(EventLoopTest, SingleThreadEventLoopExecutesEvents)
{
    cutl::singlethread_eventloop loop("ut_single_loop");
    std::atomic<int> normal_count{0};
    std::atomic<int> timer_count{0};

    loop.post_event([&normal_count]() { normal_count.fetch_add(1); });
    loop.post_event([&normal_count]() { normal_count.fetch_add(1); });
    loop.post_timer_event(
      "tick",
      [&timer_count]() { timer_count.fetch_add(1); },
      std::chrono::milliseconds(50),
      3);

    loop.start();
    wait_for_running(loop);
    EXPECT_TRUE(loop.is_running());
    std::this_thread::sleep_for(std::chrono::milliseconds(400));
    loop.stop();
    EXPECT_FALSE(loop.is_running());

    EXPECT_GE(normal_count.load(), 2);
    EXPECT_GE(timer_count.load(), 2);
    EXPECT_LE(timer_count.load(), 5);
}

TEST(EventLoopTest, TimerTaskCancellation)
{
    cutl::singlethread_eventloop loop("ut_cancel_loop");
    std::atomic<int> counter{0};

    auto handle = loop.post_timer_event(
      "ticker",
      [&counter]() { counter.fetch_add(1); },
      std::chrono::milliseconds(30));

    loop.start();
    wait_for_running(loop);
    std::this_thread::sleep_for(std::chrono::milliseconds(120));
    EXPECT_TRUE(handle.isvalid());
    handle.cancel();
    int snapshot = counter.load();
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    loop.stop();

    // 取消后任务不应继续累加（允许 1 次余量来覆盖竞态）
    EXPECT_LE(counter.load() - snapshot, 1);
}

TEST(EventLoopTest, MultiThreadEventLoopExecutesEvents)
{
    cutl::multithread_eventloop loop(64, 8, 2);
    std::atomic<int> count{0};
    for (int i = 0; i < 10; ++i)
    {
        loop.post_event([&count]() { count.fetch_add(1); });
    }

    // multithread_eventloop::start() 是阻塞的，需要在独立线程里调用。
    std::thread runner([&loop]() { loop.start(); });

    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (count.load() < 10 && std::chrono::steady_clock::now() < deadline)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    loop.stop();
    if (runner.joinable())
    {
        runner.join();
    }
    EXPECT_EQ(count.load(), 10);
}
