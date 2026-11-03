#pragma once

#include "common.hpp"
#include "common_util/datetime.h"
#include "common_util/threadpool.h"
#include "common_util/threadutil.h"

#include <atomic>
#include <chrono>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

namespace
{

int g_threadpool_demo_failures = 0;

void DemoCheck(bool ok, const char* what)
{
    if (ok)
    {
        std::cout << "  ok  " << what << std::endl;
        return;
    }
    ++g_threadpool_demo_failures;
    std::cout << "  FAIL " << what << std::endl;
}

bool DemoWait(const std::function<bool()>& pred, std::chrono::milliseconds budget)
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

// threadpool(name) + start(n) + add_task + stop
void thread_pool_case_01()
{
    PrintSubTitle("thread_pool case 01: legacy pool");

    cutl::threadpool tp("demo_tp");
    tp.start(2);

    std::atomic<int> ran{ 0 };
    for (int i = 0; i < 4; ++i)
    {
        const bool accepted = tp.add_task(
          [&ran, i]()
          {
              auto tid = cutl::get_current_thread_tid();
              std::cout << cutl::datetime::now().format() << " (" << tid << ") task " << i
                        << std::endl;
              ran.fetch_add(1);
          });
        DemoCheck(accepted, "add_task accepted");
    }
    DemoCheck(DemoWait([&ran]() { return ran.load() == 4; }, std::chrono::seconds(2)),
              "four tasks finished");
    tp.stop();
    DemoCheck(tp.thread_count() == 0, "stop waits until workers exit");
}

// Bounded queue: add_task waits, the timeout overload gives up.
void thread_pool_case_02()
{
    PrintSubTitle("thread_pool case 02: queue and timeout");

    cutl::threadpool tp("demo_q", 1);
    tp.start(1);

    std::atomic<bool> release{ false };
    DemoCheck(tp.add_task(
                [&release]()
                {
                    while (!release.load())
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                }),
              "blocking task accepted");
    DemoCheck(tp.add_task([]() {}), "second task queued");
    DemoCheck(tp.queue_size() == 1, "queue holds the waiting task");
    DemoCheck(!tp.add_task([]() {}, std::chrono::milliseconds(50)), "third task times out");

    release.store(true);
    tp.stop();
}

// add_task_with_args_and_return, including a call after stop().
void thread_pool_case_03()
{
    PrintSubTitle("thread_pool case 03: task with return value");

    auto mtid = cutl::get_current_thread_tid();
    cutl::threadpool tp("demo_ret", 4);
    tp.start(1);

    auto sum = tp.add_task_with_args_and_return([](int a, int b) { return a + b; }, 3, 2);
    DemoCheck(sum.get() == 5, "3 + 2 == 5");

    auto hello = tp.add_task_with_args_and_return(
      [](const std::string& name) { return std::string("Hello ") + name; }, std::string("Spencer"));
    const std::string text = hello.get();
    std::cout << cutl::datetime::now().format() << " (" << mtid << ") " << text << std::endl;
    DemoCheck(text == "Hello Spencer", "greeting");

    tp.stop();
    auto after_stop = tp.add_task_with_args_and_return([](int n) { return n + 1; }, 41);
    DemoCheck(after_stop.get() == 42, "stopped pool runs the task in the caller");
}

// threadpool(name, config): execute / submit / counts / shutdown.
void thread_pool_case_04()
{
    PrintSubTitle("thread_pool case 04: configured pool");

    cutl::threadpool pool(
      "demo_cfg", cutl::threadpool_config{ 1, 2, 200, 4, cutl::reject_policy::discard_newest });

    auto square = pool.submit([](int n) { return n * n; }, 6);
    DemoCheck(square.get() == 36, "submit returns 36");

    std::atomic<int> ran{ 0 };
    DemoCheck(pool.execute([&ran]() { ran.fetch_add(1); }), "execute accepted");
    DemoCheck(DemoWait([&ran]() { return ran.load() == 1; }, std::chrono::seconds(1)),
              "execute ran");

    std::cout << "  workers=" << pool.thread_count() << " queued=" << pool.queue_size()
              << std::endl;
    DemoCheck(pool.shutdown(false), "first shutdown");
    DemoCheck(!pool.shutdown(false), "second shutdown is a no-op");
    DemoCheck(!pool.execute([]() {}), "execute after shutdown");

    auto still = pool.submit([]() { return 7; });
    DemoCheck(still.get() == 7, "submit after shutdown runs in the caller");
}

// The three reject policies, plus an unnamed threadpool(config).
void thread_pool_case_05()
{
    PrintSubTitle("thread_pool case 05: reject policies");

    std::promise<void> entered;
    std::promise<void> release;
    auto entered_future = entered.get_future();
    auto release_future = release.get_future();

    cutl::threadpool discard(
      cutl::threadpool_config{ 1, 1, 1000, 0, cutl::reject_policy::discard_newest });
    discard.execute(
      [&entered, &release_future]()
      {
          entered.set_value();
          release_future.wait();
      });
    entered_future.wait();
    DemoCheck(!discard.execute([]() {}), "discard_newest drops the new task");
    release.set_value();
    DemoCheck(discard.shutdown(), "discard pool shutdown");

    std::promise<void> hold_entered;
    std::promise<void> hold_release;
    cutl::threadpool callers(
      "demo_sync", cutl::threadpool_config{ 1, 1, 1000, 0, cutl::reject_policy::synchronous });
    callers.execute(
      [&hold_entered, &hold_release]()
      {
          hold_entered.set_value();
          hold_release.get_future().wait();
      });
    hold_entered.get_future().wait();
    const auto caller = std::this_thread::get_id();
    std::thread::id ran_on;
    DemoCheck(callers.execute([&ran_on]() { ran_on = std::this_thread::get_id(); }),
              "synchronous execute returns true");
    DemoCheck(ran_on == caller, "synchronous task ran on the caller");
    hold_release.set_value();
    callers.shutdown();

    std::promise<void> oldest_entered;
    std::promise<void> oldest_release;
    std::vector<int> order;
    cutl::threadpool oldest(
      "demo_old", cutl::threadpool_config{ 1, 1, 1000, 1, cutl::reject_policy::discard_oldest });
    oldest.execute(
      [&oldest_entered, &oldest_release, &order]()
      {
          oldest_entered.set_value();
          oldest_release.get_future().wait();
          order.push_back(0);
      });
    oldest_entered.get_future().wait();
    oldest.execute([&order]() { order.push_back(1); });
    oldest.execute([&order]() { order.push_back(2); });
    DemoCheck(oldest.queue_size() == 1, "discard_oldest keeps one queued task");
    oldest_release.set_value();
    oldest.shutdown();
    DemoCheck(order.size() == 2 && order[0] == 0 && order[1] == 2, "oldest queued task was dropped");
}

// shutdown(true) drops tasks that have not started.
void thread_pool_case_06()
{
    PrintSubTitle("thread_pool case 06: shutdown now");

    std::promise<void> entered;
    std::promise<void> release;
    std::atomic<int> ran{ 0 };
    cutl::threadpool pool(
      "demo_now", cutl::threadpool_config{ 1, 1, 1000, 4, cutl::reject_policy::discard_newest });
    pool.execute(
      [&entered, &release, &ran]()
      {
          ran.fetch_add(1);
          entered.set_value();
          release.get_future().wait();
      });
    entered.get_future().wait();
    pool.execute([&ran]() { ran.fetch_add(1); });
    DemoCheck(pool.queue_size() == 1, "one task is queued");

    std::thread release_when_cleared(
      [&pool, &release]()
      {
          while (pool.queue_size() != 0)
          {
              std::this_thread::yield();
          }
          release.set_value();
      });
    DemoCheck(pool.shutdown(true), "shutdown(true)");
    release_when_cleared.join();
    DemoCheck(ran.load() == 1, "queued task was dropped");
}

void TestThreadPool()
{
    PrintTitle("Thread Pool Usage Demo");
    g_threadpool_demo_failures = 0;

    thread_pool_case_01();
    thread_pool_case_02();
    thread_pool_case_03();
    thread_pool_case_04();
    thread_pool_case_05();
    thread_pool_case_06();

    if (g_threadpool_demo_failures == 0)
    {
        std::cout << "thread pool demo: all checks passed" << std::endl;
    }
    else
    {
        std::cout << "thread pool demo: " << g_threadpool_demo_failures << " checks failed"
                  << std::endl;
    }
}
