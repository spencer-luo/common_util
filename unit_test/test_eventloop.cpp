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
#include <functional>
#include <gtest/gtest.h>
#include <stdexcept>
#include <thread>
#include <vector>

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

bool wait_until(const std::function<bool()>& pred, int timeout_ms = 2000)
{
    auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);
    while (!pred())
    {
        if (std::chrono::steady_clock::now() >= deadline)
        {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    return true;
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

TEST(EventLoopTest, IsLoopThreadDistinguishesCallSite)
{
    cutl::singlethread_eventloop loop("ut_isloopthread");
    std::atomic<bool> on_loop_thread{false};
    loop.post_event([&loop, &on_loop_thread]() {
        on_loop_thread = loop.is_loop_thread();
    });

    loop.start();
    wait_for_running(loop);
    // 主线程显然不是事件循环线程
    EXPECT_FALSE(loop.is_loop_thread());

    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(1);
    while (!on_loop_thread.load() && std::chrono::steady_clock::now() < deadline)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    loop.stop();
    EXPECT_TRUE(on_loop_thread.load());
}

TEST(EventLoopTest, TimerTaskHandlerMoveSemantics)
{
    cutl::singlethread_eventloop loop("ut_handler_move");
    std::atomic<int> counter{0};

    auto h1 = loop.post_timer_event(
      "tick",
      [&counter]() { counter.fetch_add(1); },
      std::chrono::milliseconds(40));
    EXPECT_TRUE(h1.isvalid());

    // 移动构造：原 handler 被搬走后语义上应失效，新 handler 仍能取消
    cutl::timer_task_handler h2(std::move(h1));
    EXPECT_TRUE(h2.isvalid());

    loop.start();
    wait_for_running(loop);
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    h2.cancel();
    int snapshot = counter.load();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    loop.stop();
    EXPECT_LE(counter.load() - snapshot, 1);

    // 移动赋值：把另一个有效 handler 移交过来
    cutl::singlethread_eventloop loop2("ut_handler_move2");
    auto a = loop2.post_timer_event(
      "a", []() {}, std::chrono::milliseconds(50));
    auto b = loop2.post_timer_event(
      "b", []() {}, std::chrono::milliseconds(50));
    EXPECT_TRUE(a.isvalid());
    EXPECT_TRUE(b.isvalid());
    a = std::move(b);
    EXPECT_TRUE(a.isvalid());
    loop2.start();
    wait_for_running(loop2);
    a.cancel();
    loop2.stop();
}

TEST(EventLoopTest, MultiThreadTasksRunOnThePool)
{
    cutl::multithread_eventloop loop(32, 8, 2);
    std::atomic<int> ran{ 0 };
    std::atomic<int> on_loop{ 0 };
    std::atomic<int> inflight{ 0 };
    std::atomic<int> peak{ 0 };

    for (int i = 0; i < 2; ++i)
    {
        loop.post_event(
          [&loop, &ran, &on_loop, &inflight, &peak]()
          {
              if (loop.is_loop_thread())
              {
                  on_loop.fetch_add(1);
              }
              const int now = inflight.fetch_add(1) + 1;
              int seen = peak.load();
              while (now > seen && !peak.compare_exchange_weak(seen, now))
              {
              }
              std::this_thread::sleep_for(std::chrono::milliseconds(80));
              inflight.fetch_sub(1);
              ran.fetch_add(1);
          });
    }

    std::thread runner([&loop]() { loop.start(); });
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (ran.load() < 2 && std::chrono::steady_clock::now() < deadline)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    loop.stop();
    runner.join();

    EXPECT_EQ(ran.load(), 2);
    EXPECT_EQ(on_loop.load(), 0);
    EXPECT_GE(peak.load(), 2);
}

TEST(EventLoopTest, MultiThreadTimerAndCancel)
{
    cutl::multithread_eventloop loop(32, 8, 2);
    std::atomic<int> ticks{ 0 };
    auto handle = loop.post_timer_event(
      "tick", [&ticks]() { ticks.fetch_add(1); }, std::chrono::milliseconds(30), -1);

    std::thread runner([&loop]() { loop.start(); });
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (ticks.load() < 2 && std::chrono::steady_clock::now() < deadline)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    EXPECT_TRUE(handle.isvalid());
    handle.cancel();
    const int snapshot = ticks.load();
    std::this_thread::sleep_for(std::chrono::milliseconds(120));
    loop.stop();
    runner.join();

    EXPECT_GE(ticks.load(), 2);
    EXPECT_LE(ticks.load() - snapshot, 1);
}

TEST(EventLoopTest, MultiThreadDefaultWorkerCountAndPostAfterStart)
{
    cutl::multithread_eventloop loop;
    std::atomic<int> ran{ 0 };
    std::thread runner([&loop]() { loop.start(); });
    wait_for_running(loop);

    EXPECT_TRUE(loop.post_event([&ran]() { ran.fetch_add(1); }));
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (ran.load() < 1 && std::chrono::steady_clock::now() < deadline)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    loop.stop();
    runner.join();
    EXPECT_FALSE(loop.is_running());
    EXPECT_EQ(ran.load(), 1);
}

// 在任务执行过程中再投递，唤醒发生在进入 wait 之前。不能拖到默认的 500ms 超时。
TEST(EventLoopTest, NestedPostIsNotDelayedUntilTimeout)
{
    cutl::singlethread_eventloop loop("ut_wakeup");
    std::atomic<int> stage{ 0 };
    std::chrono::steady_clock::time_point posted_at;
    std::chrono::steady_clock::time_point ran_at;

    loop.post_event(
      [&loop, &stage, &posted_at, &ran_at]()
      {
          loop.post_event(
            [&stage, &ran_at]()
            {
                ran_at = std::chrono::steady_clock::now();
                stage.store(2);
            });
          posted_at = std::chrono::steady_clock::now();
          stage.store(1);
      });

    loop.start();
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (stage.load() < 2 && std::chrono::steady_clock::now() < deadline)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    loop.stop();

    EXPECT_EQ(stage.load(), 2);
    EXPECT_LT(ran_at - posted_at, std::chrono::milliseconds(300));
}

TEST(EventLoopTest, CallbackExceptionDoesNotStopTheLoop)
{
    cutl::singlethread_eventloop loop("ut_throw");
    std::atomic<int> ran{ 0 };
    loop.post_event([]() { throw std::runtime_error("task"); });
    loop.post_timer_event(
      "boom",
      []() { throw std::runtime_error("timer"); },
      std::chrono::milliseconds(20),
      1);
    loop.post_event([&ran]() { ran.fetch_add(1); });

    loop.start();
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (ran.load() < 1 && std::chrono::steady_clock::now() < deadline)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    // 留出定时器触发的时间，确认它抛出的异常也不会让循环退出。
    std::this_thread::sleep_for(std::chrono::milliseconds(80));
    loop.stop();
    EXPECT_EQ(ran.load(), 1);
    EXPECT_FALSE(loop.is_running());
}

TEST(EventLoopTest, TimerQueueRejectsPastCapacity)
{
    cutl::singlethread_eventloop loop("ut_timer_cap", 20, 5);
    std::atomic<int> accepted{ 0 };
    std::vector<std::thread> posters;
    for (int i = 0; i < 16; ++i)
    {
        posters.emplace_back(
          [&loop, &accepted]()
          {
              auto handler = loop.post_timer_event(
                "cap", []() {}, std::chrono::hours(1), 1);
              if (handler.isvalid())
              {
                  accepted.fetch_add(1);
              }
          });
    }
    for (auto& poster : posters)
    {
        poster.join();
    }
    EXPECT_EQ(accepted.load(), 5);
}

TEST(EventLoopTest, LargeRepeatCountStaysValid)
{
    cutl::eventloop loop;
    // 低 32 位是 0。若内部把 int64_t 截成 int，这个次数会变成 0，任务直接失效。
    auto handler =
      loop.post_timer_event("big", []() {}, std::chrono::hours(1), static_cast<int64_t>(1) << 32);
    EXPECT_TRUE(handler.isvalid());
}

TEST(EventLoopTest, SingleThreadDestructorJoinsLoop)
{
    {
        cutl::singlethread_eventloop loop("ut_dtor_join");
        loop.start();
        wait_for_running(loop);
        ASSERT_TRUE(loop.is_running());
    }
}

TEST(EventLoopTest, DestructorUnblocksBlockedStart)
{
    std::thread runner;
    {
        cutl::eventloop loop;
        runner = std::thread([&loop]() { loop.start(); });
        wait_for_running(loop);
        ASSERT_TRUE(loop.is_running());
    }
    runner.join();
}

TEST(EventLoopTest, MultiThreadDestructorUnblocksStart)
{
    std::thread runner;
    {
        cutl::multithread_eventloop loop(16, 4, 1);
        runner = std::thread([&loop]() { loop.start(); });
        wait_for_running(loop);
        ASSERT_TRUE(loop.is_running());
    }
    runner.join();
}

TEST(EventLoopTest, BlockingLoopRunsTasksOnItsThread)
{
    cutl::eventloop loop;
    std::atomic<int> ran{ 0 };
    std::atomic<int> on_loop{ 0 };
    loop.post_event(
      [&loop, &ran, &on_loop]()
      {
          if (loop.is_loop_thread())
          {
              on_loop.fetch_add(1);
          }
          ran.fetch_add(1);
      });
    loop.post_timer_event(
      "twice", [&ran]() { ran.fetch_add(1); }, std::chrono::milliseconds(20), 2);

    std::thread runner([&loop]() { loop.start(); });
    EXPECT_TRUE(wait_until([&ran]() { return ran.load() >= 3; }));
    loop.stop();
    runner.join();

    EXPECT_FALSE(loop.is_running());
    EXPECT_EQ(on_loop.load(), 1);
    EXPECT_EQ(ran.load(), 3);
}

TEST(EventLoopTest, TaskQueueRejectsWhenFull)
{
    cutl::singlethread_eventloop loop("ut_task_full", 2, 1);
    EXPECT_TRUE(loop.post_event([]() {}));
    EXPECT_TRUE(loop.post_event([]() {}));
    EXPECT_FALSE(loop.post_event([]() {}));

    auto accepted = loop.post_timer_event("keep", []() {}, std::chrono::hours(1), 1);
    auto rejected = loop.post_timer_event("drop", []() {}, std::chrono::hours(1), 1);
    EXPECT_TRUE(accepted.isvalid());
    EXPECT_FALSE(rejected.isvalid());
}

TEST(EventLoopTest, NullTaskIsSkipped)
{
    cutl::singlethread_eventloop loop("ut_null_task");
    std::atomic<int> ran{ 0 };
    EXPECT_TRUE(loop.post_event(cutl::EventloopTask()));
    EXPECT_TRUE(loop.post_event([&ran]() { ran.fetch_add(1); }));
    loop.post_timer_event("empty", cutl::EventloopTask(), std::chrono::milliseconds(20), 1);

    loop.start();
    EXPECT_TRUE(wait_until([&ran]() { return ran.load() == 1; }));
    std::this_thread::sleep_for(std::chrono::milliseconds(60));
    loop.stop();
    EXPECT_FALSE(loop.is_running());
    EXPECT_EQ(ran.load(), 1);
}

TEST(EventLoopTest, TimerRepeatCountIsExact)
{
    cutl::singlethread_eventloop loop("ut_repeat");
    std::atomic<int> ticks{ 0 };
    loop.post_timer_event(
      "thrice", [&ticks]() { ticks.fetch_add(1); }, std::chrono::milliseconds(30), 3);

    loop.start();
    EXPECT_TRUE(wait_until([&ticks]() { return ticks.load() >= 3; }));
    std::this_thread::sleep_for(std::chrono::milliseconds(120));
    loop.stop();
    EXPECT_EQ(ticks.load(), 3);
}

// 到期任务还没放回队列时，它仍占着一个名额。
TEST(EventLoopTest, InFlightTimerOccupiesCapacity)
{
    cutl::singlethread_eventloop loop("ut_inflight", 8, 2);
    std::atomic<int> accepted{ 0 };
    std::atomic<int> rejected{ 0 };
    loop.post_timer_event(
      "hold",
      [&loop, &accepted, &rejected]()
      {
          for (int i = 0; i < 3; ++i)
          {
              auto handler = loop.post_timer_event("extra", []() {}, std::chrono::hours(1), 1);
              if (handler.isvalid())
              {
                  accepted.fetch_add(1);
              }
              else
              {
                  rejected.fetch_add(1);
              }
          }
      },
      std::chrono::milliseconds(20),
      1);

    loop.start();
    EXPECT_TRUE(wait_until([&accepted, &rejected]() {
        return accepted.load() + rejected.load() == 3;
    }));
    loop.stop();
    EXPECT_EQ(accepted.load(), 1);
    EXPECT_EQ(rejected.load(), 2);
}

TEST(EventLoopTest, RestartAfterStop)
{
    cutl::singlethread_eventloop loop("ut_restart");
    std::atomic<int> ran{ 0 };

    loop.start();
    wait_for_running(loop);
    loop.post_event([&ran]() { ran.store(1); });
    EXPECT_TRUE(wait_until([&ran]() { return ran.load() == 1; }));
    loop.stop();
    EXPECT_FALSE(loop.is_running());

    loop.post_event([&ran]() { ran.store(2); });
    loop.start();
    EXPECT_TRUE(wait_until([&ran]() { return ran.load() == 2; }));
    loop.stop();
    EXPECT_EQ(ran.load(), 2);
}

TEST(EventLoopTest, SecondStartWhileRunningIsIgnored)
{
    cutl::singlethread_eventloop loop("ut_start_twice");
    loop.start();
    wait_for_running(loop);
    loop.start();
    EXPECT_TRUE(loop.is_running());
    loop.stop();
    EXPECT_FALSE(loop.is_running());
}

TEST(EventLoopTest, StopFromLoopThreadDoesNotDeadlock)
{
    cutl::singlethread_eventloop loop("ut_self_stop");
    std::atomic<bool> called{ false };
    loop.post_event(
      [&loop, &called]()
      {
          called.store(true);
          loop.stop();
      });
    loop.start();
    EXPECT_TRUE(wait_until([&called]() { return called.load(); }));
    EXPECT_TRUE(wait_until([&loop]() { return !loop.is_running(); }));
}

TEST(EventLoopTest, DestructorDropsQueuedTasks)
{
    std::atomic<int> ran{ 0 };
    {
        cutl::singlethread_eventloop loop("ut_drop");
        loop.post_event([&ran]() { ran.fetch_add(1); });
        loop.post_timer_event("later", [&ran]() { ran.fetch_add(1); }, std::chrono::hours(1), 1);
    }
    EXPECT_EQ(ran.load(), 0);
}
