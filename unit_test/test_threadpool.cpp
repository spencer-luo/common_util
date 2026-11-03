/**
 * @file test_threadpool.cpp
 * @brief Unit tests for cutl::threadpool.
 */

#include "common_util/threadpool.h"
#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <gtest/gtest.h>
#include <stdexcept>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>

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

namespace
{

bool wait_until(const std::function<bool()>& pred, std::chrono::milliseconds budget)
{
    const auto deadline = std::chrono::steady_clock::now() + budget;
    while (!pred())
    {
        if (std::chrono::steady_clock::now() >= deadline)
        {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return true;
}

} // namespace

TEST(ThreadPoolTest, SubmitAndExecute)
{
    cutl::threadpool pool("ut_sub",
                          cutl::threadpool_config{ 2, 4, 1000, 16, cutl::reject_policy::discard_newest });
    std::atomic<int> sum(0);
    std::vector<std::future<int>> futures;
    for (int i = 1; i <= 8; ++i)
    {
        futures.push_back(pool.submit([i]() { return i * i; }));
        pool.execute([&sum, i]() { sum.fetch_add(i, std::memory_order_relaxed); });
    }
    int squares = 0;
    for (size_t i = 0; i < futures.size(); ++i)
    {
        squares += futures[i].get();
    }
    EXPECT_EQ(squares, 204);
    EXPECT_TRUE(pool.shutdown(false));
    EXPECT_EQ(sum.load(std::memory_order_relaxed), 36);
    EXPECT_FALSE(pool.shutdown(false));
    EXPECT_FALSE(pool.execute([]() {}));
}

TEST(ThreadPoolTest, TaskExceptionDoesNotKillWorker)
{
    cutl::threadpool pool("ut_ex",
                          cutl::threadpool_config{ 1, 1, 1000, 4, cutl::reject_policy::discard_newest });
    auto failed = pool.submit([]() -> int { throw std::runtime_error("boom"); });
    bool saw = false;
    try
    {
        failed.get();
    }
    catch (const std::runtime_error&)
    {
        saw = true;
    }
    EXPECT_TRUE(saw);
    EXPECT_EQ(pool.submit([]() { return 7; }).get(), 7);
    EXPECT_TRUE(pool.shutdown(false));
}

TEST(ThreadPoolTest, DiscardNewest)
{
    std::promise<void> entered2;
    std::promise<void> release2;
    auto entered2_future = entered2.get_future();
    auto release2_future = release2.get_future();
    std::atomic<int> discarded_ran(0);
    std::atomic<int> accepted(0);

    cutl::threadpool discard_pool(
      "ut_dc", cutl::threadpool_config{ 1, 1, 1000, 1, cutl::reject_policy::discard_newest });
    discard_pool.execute(
      [&entered2, &release2_future, &accepted]()
      {
          entered2.set_value();
          release2_future.wait();
          accepted.fetch_add(1, std::memory_order_relaxed);
      });
    entered2_future.wait();
    discard_pool.execute([&accepted]() { accepted.fetch_add(1, std::memory_order_relaxed); });
    discard_pool.execute(
      [&discarded_ran]() { discarded_ran.fetch_add(1, std::memory_order_relaxed); });
    release2.set_value();
    EXPECT_TRUE(discard_pool.shutdown(false));
    EXPECT_EQ(accepted.load(std::memory_order_relaxed), 2);
    EXPECT_EQ(discarded_ran.load(std::memory_order_relaxed), 0);
}

TEST(ThreadPoolTest, DiscardOldestAndSynchronous)
{
    std::promise<void> entered;
    std::promise<void> release;
    auto entered_future = entered.get_future();
    auto release_future = release.get_future();
    std::vector<int> order;

    cutl::threadpool oldest(
      "ut_do", cutl::threadpool_config{ 1, 1, 1000, 2, cutl::reject_policy::discard_oldest });
    oldest.execute(
      [&entered, &release_future, &order]()
      {
          entered.set_value();
          release_future.wait();
          order.push_back(0);
      });
    entered_future.wait();
    oldest.execute([&order]() { order.push_back(1); });
    oldest.execute([&order]() { order.push_back(2); });
    oldest.execute([&order]() { order.push_back(3); });
    EXPECT_EQ(oldest.queue_size(), 2u);
    release.set_value();
    EXPECT_TRUE(oldest.shutdown(false));
    ASSERT_EQ(order.size(), 3u);
    EXPECT_EQ(order[0], 0);
    EXPECT_EQ(order[1], 2);
    EXPECT_EQ(order[2], 3);

    std::promise<void> entered3;
    std::promise<void> release3;
    auto entered3_future = entered3.get_future();
    auto release3_future = release3.get_future();
    cutl::threadpool callers(
      "ut_cr", cutl::threadpool_config{ 1, 1, 1000, 1, cutl::reject_policy::synchronous });
    callers.execute(
      [&entered3, &release3_future]()
      {
          entered3.set_value();
          release3_future.wait();
      });
    entered3_future.wait();
    callers.execute([]() {});
    std::thread::id ran_on;
    callers.execute(
      [&ran_on, &callers]()
      {
          ran_on = std::this_thread::get_id();
          (void)callers.queue_size();
      });
    EXPECT_EQ(ran_on, std::this_thread::get_id());
    release3.set_value();
    EXPECT_TRUE(callers.shutdown(false));
}

TEST(ThreadPoolTest, ShutdownModes)
{
    std::promise<void> entered;
    std::promise<void> release;
    auto entered_future = entered.get_future();
    auto release_future = release.get_future();
    std::atomic<int> ran(0);

    cutl::threadpool graceful(
      "ut_sg", cutl::threadpool_config{ 1, 1, 1000, 8, cutl::reject_policy::discard_newest });
    graceful.execute(
      [&entered, &release_future, &ran]()
      {
          entered.set_value();
          release_future.wait();
          ran.fetch_add(1, std::memory_order_relaxed);
      });
    entered_future.wait();
    graceful.execute([&ran]() { ran.fetch_add(1, std::memory_order_relaxed); });
    graceful.execute([&ran]() { ran.fetch_add(1, std::memory_order_relaxed); });
    std::thread opener([&release]() { release.set_value(); });
    EXPECT_TRUE(graceful.shutdown(false));
    opener.join();
    EXPECT_EQ(ran.load(std::memory_order_relaxed), 3);

    std::promise<void> entered_now;
    std::promise<void> release_now;
    auto entered_now_future = entered_now.get_future();
    std::atomic<int> ran_now(0);
    cutl::threadpool immediate(
      "ut_sn", cutl::threadpool_config{ 1, 1, 1000, 8, cutl::reject_policy::discard_newest });
    immediate.execute(
      [&entered_now, &release_now, &ran_now]()
      {
          ran_now.fetch_add(1, std::memory_order_relaxed);
          entered_now.set_value();
          release_now.get_future().wait();
      });
    entered_now_future.wait();
    immediate.execute([&ran_now]() { ran_now.fetch_add(1, std::memory_order_relaxed); });
    immediate.execute([&ran_now]() { ran_now.fetch_add(1, std::memory_order_relaxed); });
    EXPECT_EQ(immediate.queue_size(), 2u);
    std::thread release_when_cleared(
      [&immediate, &release_now]()
      {
          while (immediate.queue_size() != 0)
          {
              std::this_thread::yield();
          }
          release_now.set_value();
      });
    EXPECT_TRUE(immediate.shutdown(true));
    release_when_cleared.join();
    EXPECT_EQ(ran_now.load(std::memory_order_relaxed), 1);
}

TEST(ThreadPoolTest, NestedExecute)
{
    cutl::threadpool pool("ut_ne",
                          cutl::threadpool_config{ 1, 2, 1000, 8, cutl::reject_policy::discard_newest });
    std::atomic<int> ran(0);
    std::promise<void> nested_queued;
    pool.execute(
      [&pool, &ran, &nested_queued]()
      {
          pool.execute([&ran]() { ran.fetch_add(1, std::memory_order_relaxed); });
          ran.fetch_add(1, std::memory_order_relaxed);
          nested_queued.set_value();
      });
    nested_queued.get_future().wait();
    EXPECT_TRUE(pool.shutdown(false));
    EXPECT_EQ(ran.load(std::memory_order_relaxed), 2);
}

TEST(ThreadPoolTest, KeepAliveAndCoreZero)
{
    std::promise<void> release;
    auto release_future = release.get_future().share();
    cutl::threadpool pool("ut_ka",
                          cutl::threadpool_config{ 1, 2, 200, 0, cutl::reject_policy::discard_newest });

    std::promise<void> first_entered;
    std::promise<void> second_entered;
    pool.execute(
      [&first_entered, release_future]()
      {
          first_entered.set_value();
          release_future.wait();
      });
    pool.execute(
      [&second_entered, release_future]()
      {
          second_entered.set_value();
          release_future.wait();
      });
    EXPECT_EQ(pool.thread_count(), 2u);
    std::atomic<int> rejected_ran(0);
    pool.execute([&rejected_ran]() { rejected_ran.fetch_add(1, std::memory_order_relaxed); });
    EXPECT_EQ(rejected_ran.load(std::memory_order_relaxed), 0);
    first_entered.get_future().wait();
    second_entered.get_future().wait();
    release.set_value();

    EXPECT_TRUE(wait_until([&pool]() { return pool.thread_count() == 1; }, std::chrono::seconds(3)));
    std::this_thread::sleep_for(std::chrono::milliseconds(400));
    EXPECT_EQ(pool.thread_count(), 1u);

    std::atomic<int> extra(0);
    pool.execute([&extra]() { extra.fetch_add(1, std::memory_order_relaxed); });
    EXPECT_TRUE(pool.shutdown(false));
    EXPECT_EQ(extra.load(std::memory_order_relaxed), 1);

    cutl::threadpool cached("ut_c0",
                            cutl::threadpool_config{ 0, 4, 100, 32, cutl::reject_policy::discard_newest });
    std::atomic<int> ran(0);
    for (int i = 0; i < 20; ++i)
    {
        cached.execute([&ran]() { ran.fetch_add(1, std::memory_order_relaxed); });
    }
    EXPECT_TRUE(cached.shutdown(false));
    EXPECT_EQ(ran.load(std::memory_order_relaxed), 20);
    EXPECT_TRUE(
      wait_until([&cached]() { return cached.thread_count() == 0; }, std::chrono::seconds(3)));
}

TEST(ThreadPoolTest, IdleExitDoesNotLoseTasks)
{
    for (int round = 0; round < 40; ++round)
    {
        cutl::threadpool pool(
          "ut_ie", cutl::threadpool_config{ 0, 8, 1, 1000, cutl::reject_policy::discard_newest });
        std::atomic<int> ran(0);
        for (int i = 0; i < 200; ++i)
        {
            pool.execute([&ran]() { ran.fetch_add(1, std::memory_order_relaxed); });
        }
        EXPECT_TRUE(pool.shutdown(false));
        EXPECT_EQ(ran.load(std::memory_order_relaxed), 200);
    }
}

TEST(ThreadPoolTest, InvalidConfig)
{
    cutl::threadpool pool(
      "ut_inv", cutl::threadpool_config{ 4, 2, 1, 1, cutl::reject_policy::discard_newest });
    EXPECT_FALSE(pool.execute([]() {}));
}

TEST(ThreadPoolTest, AddTaskBeforeStartReturnsFalse)
{
    cutl::threadpool tp("ut_ns", 8);
    EXPECT_EQ(tp.thread_count(), 0u);
    EXPECT_EQ(tp.queue_size(), 0u);
    EXPECT_FALSE(tp.add_task([]() {}));
    EXPECT_FALSE(tp.add_task([]() {}, std::chrono::milliseconds(10)));
    EXPECT_FALSE(tp.execute([]() {}));
}

TEST(ThreadPoolTest, NotCopyable)
{
    EXPECT_FALSE(std::is_copy_constructible<cutl::threadpool>::value);
    EXPECT_FALSE(std::is_copy_assignable<cutl::threadpool>::value);
}

TEST(ThreadPoolTest, EmptyTaskIsRejected)
{
    cutl::threadpool tp("ut_empty", 4);
    tp.start(1);
    cutl::threadpool::Task empty;
    EXPECT_FALSE(tp.add_task(empty));
    EXPECT_FALSE(tp.add_task(empty, std::chrono::milliseconds(10)));
    EXPECT_FALSE(tp.execute(empty));
    tp.stop();
}

TEST(ThreadPoolTest, StartTwiceKeepsTheFirstWorkerCount)
{
    cutl::threadpool tp("ut_twice", 8);
    tp.start(2);
    EXPECT_EQ(tp.thread_count(), 2u);
    tp.start(4);
    EXPECT_EQ(tp.thread_count(), 2u);
    tp.stop();
    EXPECT_EQ(tp.thread_count(), 0u);
}

TEST(ThreadPoolTest, StartZeroSpawnsHardwareCoreThreads)
{
    cutl::threadpool tp("ut_hw");
    tp.start(0);
    unsigned int cores = std::thread::hardware_concurrency();
    if (cores == 0)
    {
        cores = 1;
    }
    EXPECT_EQ(tp.thread_count(), static_cast<size_t>(cores));
    tp.stop();
}

TEST(ThreadPoolTest, RestartAfterStop)
{
    cutl::threadpool tp("ut_re", 8);
    tp.start(1);
    tp.stop();
    EXPECT_FALSE(tp.add_task([]() {}));

    tp.start(1);
    std::atomic<int> ran{ 0 };
    EXPECT_TRUE(tp.add_task([&ran]() { ran.fetch_add(1); }));
    EXPECT_TRUE(wait_until([&ran]() { return ran.load() == 1; }, std::chrono::seconds(1)));
    tp.stop();
    EXPECT_EQ(ran.load(), 1);
}

TEST(ThreadPoolTest, StopDropsQueuedTasksWithoutWaiting)
{
    cutl::threadpool tp("ut_dropq", 8);
    tp.start(1);

    std::promise<void> entered;
    std::atomic<bool> release{ false };
    std::atomic<int> ran{ 0 };
    EXPECT_TRUE(tp.add_task(
      [&entered, &release]()
      {
          entered.set_value();
          while (!release.load())
          {
              std::this_thread::sleep_for(std::chrono::milliseconds(10));
          }
      }));
    entered.get_future().wait();
    EXPECT_TRUE(tp.add_task([&ran]() { ran.fetch_add(1); }));
    EXPECT_TRUE(tp.add_task([&ran]() { ran.fetch_add(1); }));
    EXPECT_EQ(tp.queue_size(), 2u);

    tp.stop(true);
    EXPECT_EQ(tp.queue_size(), 0u);
    EXPECT_FALSE(tp.add_task([]() {}));
    release.store(true);
    EXPECT_TRUE(wait_until([&tp]() { return tp.thread_count() == 0; }, std::chrono::seconds(1)));
    EXPECT_EQ(ran.load(), 0);
}

TEST(ThreadPoolTest, AddTaskWithReturnRunsInCallerWhenStopped)
{
    cutl::threadpool tp("ut_caller_add", 4);
    tp.start(1);
    tp.stop();

    const auto caller = std::this_thread::get_id();
    std::thread::id ran_on;
    auto value = tp.add_task_with_args_and_return(
      [&ran_on](int n)
      {
          ran_on = std::this_thread::get_id();
          return n + 1;
      },
      41);
    EXPECT_EQ(value.get(), 42);
    EXPECT_EQ(ran_on, caller);
}

TEST(ThreadPoolTest, ReturnedTaskExceptionIsVisibleOnTheFuture)
{
    cutl::threadpool tp("ut_ret_ex", 4);
    tp.start(1);
    auto failed = tp.add_task_with_args_and_return([]() -> int { throw std::runtime_error("nope"); });
    EXPECT_THROW(failed.get(), std::runtime_error);
    EXPECT_EQ(tp.add_task_with_args_and_return([]() { return 1; }).get(), 1);
    tp.stop();
}

TEST(ThreadPoolTest, UnnamedConfigConstructorAcceptsTasks)
{
    cutl::threadpool pool(
      cutl::threadpool_config{ 1, 2, 1000, 8, cutl::reject_policy::synchronous });
    EXPECT_EQ(pool.submit([](int a, int b) { return a * b; }, 6, 7).get(), 42);
    EXPECT_TRUE(pool.shutdown());
}

TEST(ThreadPoolTest, RunningConfigPoolIgnoresStart)
{
    cutl::threadpool pool(
      "ut_cfg_start", cutl::threadpool_config{ 1, 3, 1000, 8, cutl::reject_policy::discard_newest });
    EXPECT_EQ(pool.thread_count(), 0u);
    pool.start(3);
    EXPECT_EQ(pool.thread_count(), 0u);
    EXPECT_TRUE(pool.execute([]() {}));
    EXPECT_TRUE(pool.shutdown());
}

TEST(ThreadPoolTest, SubmitRunsInCallerWhenExecuteRejects)
{
    std::promise<void> entered;
    std::promise<void> release;
    auto entered_future = entered.get_future();
    auto release_future = release.get_future();

    cutl::threadpool pool(
      "ut_sub_rej", cutl::threadpool_config{ 1, 1, 1000, 0, cutl::reject_policy::discard_newest });
    EXPECT_TRUE(pool.execute(
      [&entered, &release_future]()
      {
          entered.set_value();
          release_future.wait();
      }));
    entered_future.wait();
    EXPECT_FALSE(pool.execute([]() {}));

    const auto caller = std::this_thread::get_id();
    std::thread::id ran_on;
    auto value = pool.submit(
      [&ran_on]()
      {
          ran_on = std::this_thread::get_id();
          return 9;
      });
    EXPECT_EQ(value.get(), 9);
    EXPECT_EQ(ran_on, caller);
    release.set_value();
    EXPECT_TRUE(pool.shutdown());
}

TEST(ThreadPoolTest, SynchronousRejectReturnsTrueAndRunsInCaller)
{
    std::promise<void> entered;
    std::promise<void> release;
    auto entered_future = entered.get_future();
    auto release_future = release.get_future();

    cutl::threadpool pool(
      "ut_sync_ok", cutl::threadpool_config{ 1, 1, 1000, 0, cutl::reject_policy::synchronous });
    EXPECT_TRUE(pool.execute(
      [&entered, &release_future]()
      {
          entered.set_value();
          release_future.wait();
      }));
    entered_future.wait();

    const auto caller = std::this_thread::get_id();
    std::thread::id ran_on;
    EXPECT_TRUE(pool.execute([&ran_on]() { ran_on = std::this_thread::get_id(); }));
    EXPECT_EQ(ran_on, caller);
    release.set_value();
    EXPECT_TRUE(pool.shutdown());
}

TEST(ThreadPoolTest, DiscardOldestWithNothingQueuedReturnsFalse)
{
    std::promise<void> entered;
    std::promise<void> release;
    auto entered_future = entered.get_future();
    auto release_future = release.get_future();

    cutl::threadpool pool(
      "ut_do_empty", cutl::threadpool_config{ 1, 1, 1000, 0, cutl::reject_policy::discard_oldest });
    EXPECT_TRUE(pool.execute(
      [&entered, &release_future]()
      {
          entered.set_value();
          release_future.wait();
      }));
    entered_future.wait();
    EXPECT_EQ(pool.queue_size(), 0u);
    EXPECT_FALSE(pool.execute([]() {}));
    release.set_value();
    EXPECT_TRUE(pool.shutdown());
}

TEST(ThreadPoolTest, ZeroMaxThreadsStaysStopped)
{
    cutl::threadpool pool(
      "ut_max0", cutl::threadpool_config{ 0, 0, 1, 1, cutl::reject_policy::discard_newest });
    EXPECT_FALSE(pool.execute([]() {}));
    EXPECT_FALSE(pool.add_task([]() {}));
    EXPECT_FALSE(pool.shutdown());
}

TEST(ThreadPoolTest, StopWhenAlreadyStoppedIsHarmless)
{
    cutl::threadpool tp("ut_stop2", 4);
    tp.stop();
    tp.start(1);
    tp.stop();
    tp.stop(true);
    EXPECT_FALSE(tp.add_task([]() {}));
}
