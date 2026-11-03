#include "common.hpp"
#include "common_util/datetime.h"
#include "common_util/eventloop.h"
#include "common_util/threadutil.h"

#include <atomic>
#include <functional>
#include <thread>

void test_eventloop()
{
    PrintSubTitle("test_eventloop");

    cutl::eventloop eventLoop;
    // timer task
    int repet_count = 0;
    eventLoop.post_timer_event(
      "timer_1s",
      [&repet_count]()
      {
          repet_count++;
          std::cout << cutl::datetime::now().format() << " execute per 1s " << repet_count
                    << " times." << std::endl;
      },
      std::chrono::seconds(1),
      3);
    // normal task
    eventLoop.post_event(
      []() { std::cout << cutl::datetime::now().format() << " execute Task 01" << std::endl; });
    eventLoop.post_event(
      []() { std::cout << cutl::datetime::now().format() << " execute Task 02" << std::endl; });

    // 主线程处理事件循环
    eventLoop.start();

    std::cout << "main thread exit" << std::endl;
}

void test_singlethread_eventloop()
{
    PrintSubTitle("test_singlethread_eventloop");

    // , 2, 1
    cutl::singlethread_eventloop eventLoop("single_thrd_ev");

    // timer task 01
    int repet_count = 0;
    eventLoop.post_timer_event(
      "timer_1s",
      [&repet_count]()
      {
          repet_count++;
          std::cout << cutl::datetime::now().format() << " execute per 1s " << repet_count
                    << " times." << std::endl;
      },
      std::chrono::seconds(1),
      3);
    // timer task 02
    auto timer_2s = eventLoop.post_timer_event(
      "timer_1.5s",
      []() { std::cout << cutl::datetime::now().format() << " execute per 1.5s" << std::endl; },
      std::chrono::milliseconds(1500));
    // normal task 01
    eventLoop.post_event(
      []()
      { std::cout << cutl::datetime::now().format() << " execute normal task 01" << std::endl; });

    eventLoop.start();
    std::cout << "main thread id:" << std::this_thread::get_id()
              << ", main thread is eventloop thread:" << eventLoop.is_loop_thread() << std::endl;

    // normal task 02
    std::this_thread::sleep_for(std::chrono::milliseconds(1200));
    eventLoop.post_event(
      []()
      { std::cout << cutl::datetime::now().format() << " execute normal task 02" << std::endl; });
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    eventLoop.post_event(
      []()
      { std::cout << cutl::datetime::now().format() << " execute normal task 03" << std::endl; });
    eventLoop.post_event(
      []()
      { std::cout << cutl::datetime::now().format() << " execute normal task 04" << std::endl; });

    std::this_thread::sleep_for(std::chrono::seconds(5));
    timer_2s.cancel();

    eventLoop.stop();
    std::cout << "main thread exit" << std::endl;
}

void test_multithread_eventloop()
{
    PrintSubTitle("test_multithread_eventloop");

    int failures = 0;
    auto check = [&failures](bool ok, const char* what)
    {
        if (ok)
        {
            std::cout << "  ok  " << what << std::endl;
            return;
        }
        ++failures;
        std::cout << "  FAIL " << what << std::endl;
    };
    auto wait_until = [](const std::function<bool()>& pred, std::chrono::milliseconds budget)
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
    };

    // start() blocks, so the loop runs on its own thread. Tasks run on the pool.
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
    check(wait_until([&tasks]() { return tasks.load() == 2; }, std::chrono::seconds(2)),
          "two tasks finished");
    check(off_loop.load() == 2, "tasks ran on the thread pool, not the loop thread");
    check(peak.load() >= 2, "two pool threads ran tasks at the same time");
    check(wait_until([&timers]() { return timers.load() >= 2; }, std::chrono::seconds(2)),
          "timer callbacks ran on the pool");

    event_loop.stop();
    runner.join();
    check(!event_loop.is_running(), "stop joined the loop thread");

    if (failures == 0)
    {
        std::cout << "multithread eventloop demo: all checks passed" << std::endl;
    }
    else
    {
        std::cout << "multithread eventloop demo: " << failures << " checks failed" << std::endl;
    }
}

void TestEventLoop()
{
    PrintTitle("Test EventLoop");

    // test_eventloop();
    test_singlethread_eventloop();
    test_multithread_eventloop();
}