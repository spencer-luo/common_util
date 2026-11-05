#pragma once

#include "common.hpp"
#include "common_util/datetime.h"
#include "common_util/eventloop.h"
#include "common_util/threadutil.h"

#include <atomic>
#include <chrono>
#include <functional>
#include <iostream>
#include <string>
#include <thread>

namespace
{

int g_eventloop_demo_failures = 0;

void EvtCheck(bool ok, const char* what)
{
    if (ok)
    {
        std::cout << "  ok  " << what << std::endl;
        return;
    }
    ++g_eventloop_demo_failures;
    std::cout << "  FAIL " << what << std::endl;
}

bool EvtWait(const std::function<bool()>& pred, std::chrono::milliseconds budget)
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

// eventloop::start() blocks the caller until stop().
void test_eventloop()
{
    PrintSubTitle("test_eventloop");

    cutl::eventloop event_loop;
    std::atomic<int> tasks{ 0 };
    std::atomic<int> timers{ 0 };
    std::atomic<int> on_loop{ 0 };

    event_loop.post_event(
      [&event_loop, &tasks, &on_loop]()
      {
          if (event_loop.is_loop_thread())
          {
              on_loop.fetch_add(1);
          }
          std::cout << cutl::datetime::now().format() << " execute Task 01" << std::endl;
          tasks.fetch_add(1);
      });
    event_loop.post_event(
      [&tasks]()
      {
          std::cout << cutl::datetime::now().format() << " execute Task 02" << std::endl;
          tasks.fetch_add(1);
      });
    event_loop.post_timer_event(
      "timer_40ms",
      [&timers]()
      {
          const int n = timers.fetch_add(1) + 1;
          std::cout << cutl::datetime::now().format() << " timer " << n << std::endl;
      },
      std::chrono::milliseconds(40),
      2);

    std::thread runner([&event_loop]() { event_loop.start(); });
    EvtCheck(EvtWait([&tasks]() { return tasks.load() == 2; }, std::chrono::seconds(2)),
             "two tasks finished on the loop thread");
    EvtCheck(on_loop.load() == 1, "task saw the eventloop thread");
    EvtCheck(EvtWait([&timers]() { return timers.load() == 2; }, std::chrono::seconds(2)),
             "timer ran exactly twice");
    event_loop.stop();
    runner.join();
    EvtCheck(!event_loop.is_running(), "stop unblocked start()");
}

// singlethread_eventloop::start() returns after spawning the loop thread.
void test_singlethread_eventloop()
{
    PrintSubTitle("test_singlethread_eventloop");

    {
        cutl::singlethread_eventloop full("ev_full", 2, 1);
        EvtCheck(full.post_event([]() {}), "first task accepted");
        EvtCheck(full.post_event([]() {}), "second task accepted");
        EvtCheck(!full.post_event([]() {}), "third task rejected because the queue is full");
        auto kept = full.post_timer_event("keep", []() {}, std::chrono::hours(1), 1);
        auto dropped = full.post_timer_event("drop", []() {}, std::chrono::hours(1), 1);
        EvtCheck(kept.isvalid(), "timer accepted");
        EvtCheck(!dropped.isvalid(), "timer rejected because the timer queue is full");
    }

    cutl::singlethread_eventloop event_loop("single_thrd_ev");
    std::atomic<int> tasks{ 0 };
    std::atomic<int> timers{ 0 };
    std::atomic<int> on_loop{ 0 };

    auto timer = event_loop.post_timer_event(
      "timer_40ms",
      [&timers]()
      {
          const int n = timers.fetch_add(1) + 1;
          std::cout << cutl::datetime::now().format() << " execute per 40ms " << n << std::endl;
      },
      std::chrono::milliseconds(40));
    EvtCheck(timer.isvalid(), "repeating timer accepted");
    event_loop.post_event(cutl::EventloopTask());
    event_loop.post_event(
      [&event_loop, &tasks, &on_loop]()
      {
          if (event_loop.is_loop_thread())
          {
              on_loop.fetch_add(1);
          }
          std::cout << cutl::datetime::now().format() << " execute normal task" << std::endl;
          tasks.fetch_add(1);
      });

    event_loop.start();
    EvtCheck(EvtWait([&event_loop]() { return event_loop.is_running(); }, std::chrono::seconds(2)),
             "start() returned and the loop thread is running");
    EvtCheck(!event_loop.is_loop_thread(), "caller is not the loop thread");
    EvtCheck(EvtWait([&tasks]() { return tasks.load() == 1; }, std::chrono::seconds(2)),
             "null task was skipped and the next task ran");
    EvtCheck(on_loop.load() == 1, "task ran on the loop thread");
    EvtCheck(EvtWait([&timers]() { return timers.load() >= 2; }, std::chrono::seconds(2)),
             "repeating timer fired");
    timer.cancel();
    const int snapshot = timers.load();
    std::this_thread::sleep_for(std::chrono::milliseconds(120));
    event_loop.stop();
    EvtCheck(!event_loop.is_running(), "stop joined the loop thread");
    EvtCheck(timers.load() - snapshot <= 1, "cancel stopped further timer ticks");
}

// Tasks and timer callbacks run on the thread pool, not the loop thread.
void test_multithread_eventloop()
{
    PrintSubTitle("test_multithread_eventloop");

    cutl::multithread_eventloop event_loop(32, 8, 2);
    std::atomic<int> tasks{ 0 };
    std::atomic<int> timers{ 0 };
    std::atomic<int> off_loop{ 0 };
    std::atomic<int> inflight{ 0 };
    std::atomic<int> peak{ 0 };

    for (int i = 0; i < 2; ++i)
    {
        event_loop.post_event(
          [&event_loop, &tasks, &off_loop, &inflight, &peak, i]()
          {
              const int now = inflight.fetch_add(1) + 1;
              int seen = peak.load();
              while (now > seen && !peak.compare_exchange_weak(seen, now))
              {
              }
              if (!event_loop.is_loop_thread())
              {
                  off_loop.fetch_add(1);
              }
              const auto tid = cutl::get_current_thread_tid();
              std::cout << cutl::datetime::now().format() << " (" << tid << ") task " << i
                        << std::endl;
              std::this_thread::sleep_for(std::chrono::milliseconds(50));
              inflight.fetch_sub(1);
              tasks.fetch_add(1);
          });
    }
    event_loop.post_timer_event(
      "timer_40ms",
      [&timers]()
      {
          const auto tid = cutl::get_current_thread_tid();
          const int n = timers.fetch_add(1) + 1;
          std::cout << cutl::datetime::now().format() << " (" << tid << ") timer " << n
                    << std::endl;
      },
      std::chrono::milliseconds(40),
      3);

    std::thread runner([&event_loop]() { event_loop.start(); });
    EvtCheck(EvtWait([&tasks]() { return tasks.load() == 2; }, std::chrono::seconds(2)),
             "two tasks finished");
    EvtCheck(off_loop.load() == 2, "tasks ran on the thread pool, not the loop thread");
    EvtCheck(peak.load() >= 2, "two pool threads ran tasks at the same time");
    EvtCheck(EvtWait([&timers]() { return timers.load() >= 2; }, std::chrono::seconds(2)),
             "timer callbacks ran on the pool");

    event_loop.stop();
    runner.join();
    EvtCheck(!event_loop.is_running(), "stop joined the loop thread");
}

void TestEventLoop()
{
    PrintTitle("Test EventLoop");
    g_eventloop_demo_failures = 0;

    test_eventloop();
    test_singlethread_eventloop();
    test_multithread_eventloop();

    if (g_eventloop_demo_failures == 0)
    {
        std::cout << "event loop demo: all checks passed" << std::endl;
    }
    else
    {
        std::cout << "event loop demo: " << g_eventloop_demo_failures << " checks failed"
                  << std::endl;
    }
}
