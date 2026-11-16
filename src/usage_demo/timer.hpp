#pragma once

#include "common.hpp"
#include "common_util/timer.h"

#include <atomic>
#include <chrono>
#include <functional>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace
{

int g_timer_demo_failures = 0;

void TimerCheck(bool ok, const char* what)
{
    if (ok)
    {
        std::cout << "  ok  " << what << std::endl;
        return;
    }
    ++g_timer_demo_failures;
    std::cout << "  FAIL " << what << std::endl;
}

bool TimerWait(const std::function<bool()>& pred, std::chrono::milliseconds budget)
{
    const auto deadline = std::chrono::steady_clock::now() + budget;
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

void DemoAdd(std::atomic<int>* counter, int delta)
{
    counter->fetch_add(delta);
}

void test_set_timeout()
{
    PrintSubTitle("set_timeout");

    std::atomic<int> calls{ 0 };
    std::atomic<int> summed{ 0 };
    std::string name = "timer";
    cutl::set_timeout(DemoAdd, 40, &summed, 4);
    cutl::set_timeout(
      [&calls, name]() {
          calls.fetch_add(1);
          std::cout << "set_timeout lambda, name=" << name << std::endl;
      },
      40);
    cutl::set_timeout(
      [&calls]() {
          calls.fetch_add(1);
          throw std::runtime_error("demo timeout");
      },
      40);

    TimerCheck(TimerWait([&]() { return calls.load() == 2 && summed.load() == 4; },
                         std::chrono::milliseconds(400)),
               "one-shot calls finished and arguments were forwarded");
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    TimerCheck(calls.load() == 2, "a one-shot callback does not run twice");
}

void test_periodic_timer()
{
    PrintSubTitle("periodic timer");

    std::atomic<int> calls{ 0 };
    std::atomic<int> extra_workers{ 0 };
    std::mutex id_mutex;
    std::thread::id worker;
    cutl::timer t(
      "demo_period",
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
                  extra_workers.fetch_add(1);
              }
          }
          const int n = calls.fetch_add(1) + 1;
          std::cout << "periodic tick " << n << std::endl;
      },
      std::chrono::milliseconds(40));
    t.start();
    t.start();
    TimerCheck(t.is_running(), "start() makes the timer running");
    TimerCheck(TimerWait([&calls]() { return calls.load() >= 3; }, std::chrono::milliseconds(400)),
               "periodic timer fires at least 3 times");
    const int running_calls = calls.load();
    t.stop();
    TimerCheck(!t.is_running(), "stop() clears the running flag");
    std::this_thread::sleep_for(std::chrono::milliseconds(80));
    TimerCheck(calls.load() == running_calls, "no further ticks after stop()");
    TimerCheck(extra_workers.load() == 0, "a second start() did not create another worker");
}

void test_overrun()
{
    PrintSubTitle("overrun");

    std::mutex stamps_mutex;
    std::vector<std::chrono::steady_clock::time_point> starts;
    cutl::timer t(
      "demo_overrun",
      [&]() {
          {
              std::lock_guard<std::mutex> lock(stamps_mutex);
              starts.push_back(std::chrono::steady_clock::now());
          }
          std::cout << "overrun tick " << starts.size() << std::endl;
          std::this_thread::sleep_for(std::chrono::milliseconds(80));
      },
      std::chrono::milliseconds(30));
    t.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(320));
    t.stop();

    bool gaps_ok = false;
    {
        std::lock_guard<std::mutex> lock(stamps_mutex);
        gaps_ok = starts.size() >= 2;
        for (size_t i = 1; i < starts.size(); ++i)
        {
            const auto gap =
              std::chrono::duration_cast<std::chrono::milliseconds>(starts[i] - starts[i - 1]);
            if (gap.count() < 100)
            {
                gaps_ok = false;
            }
        }
    }
    TimerCheck(gaps_ok, "an overrun waits one more period instead of running again immediately");
}

void test_stop_and_restart()
{
    PrintSubTitle("stop and restart");

    std::atomic<int> calls{ 0 };
    cutl::timer t("demo_restart", [&calls]() { calls.fetch_add(1); },
                  std::chrono::milliseconds(30));
    t.start();
    TimerCheck(TimerWait([&calls]() { return calls.load() >= 2; }, std::chrono::milliseconds(300)),
               "ticks run before stop()");
    t.stop();
    const int after_stop = calls.load();
    std::this_thread::sleep_for(std::chrono::milliseconds(70));
    TimerCheck(calls.load() == after_stop, "stop() keeps the callback from running");
    t.start();
    TimerCheck(TimerWait([&calls, after_stop]() { return calls.load() > after_stop; },
                         std::chrono::milliseconds(300)),
               "start() after stop() runs the callback again");
    t.stop();
}

void test_stop_from_callback()
{
    PrintSubTitle("stop from callback");

    std::atomic<int> calls{ 0 };
    cutl::timer* self = nullptr;
    {
        cutl::timer t(
          "demo_selfstop",
          [&calls, &self]() {
              calls.fetch_add(1);
              self->stop(false);
          },
          std::chrono::milliseconds(30));
        self = &t;
        t.start();
        TimerCheck(TimerWait([&t]() { return !t.is_running(); }, std::chrono::milliseconds(300)),
                   "stop(false) inside the callback requests exit");
        std::this_thread::sleep_for(std::chrono::milliseconds(60));
        TimerCheck(calls.load() == 1, "the callback does not run again after stopping itself");
    }
    TimerCheck(calls.load() == 1, "destroying the timer joins the worker left by stop(false)");
}

void test_invalid_timer()
{
    PrintSubTitle("invalid timer");

    cutl::timer zero("demo_zero", []() {}, std::chrono::microseconds(0));
    zero.start();
    TimerCheck(!zero.is_running(), "a zero period does not start");

    cutl::timer empty("demo_empty", cutl::timer::Task{}, std::chrono::milliseconds(40));
    empty.start();
    TimerCheck(!empty.is_running(), "an empty task does not start");
}

} // namespace

void TestTimer()
{
    PrintTitle("timer");
    g_timer_demo_failures = 0;

    test_set_timeout();
    test_periodic_timer();
    test_overrun();
    test_stop_and_restart();
    test_stop_from_callback();
    test_invalid_timer();

    if (g_timer_demo_failures == 0)
    {
        std::cout << "timer demo: all checks passed" << std::endl;
    }
    else
    {
        std::cout << "timer demo: " << g_timer_demo_failures << " checks failed" << std::endl;
    }
}
