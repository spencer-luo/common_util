#include "common.hpp"
#include "datetime.h"
#include "eventloop.h"

void test_case_01()
{
    PrintSubTitle("test_case_01");

    cutl::eventloop eventLoop;
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
    auto timer_2s = eventLoop.post_timer_event(
      "timer_1.5s",
      []() { std::cout << cutl::datetime::now().format() << " execute per 1.5s" << std::endl; },
      std::chrono::milliseconds(1500));
    eventLoop.post_event(
      []() { std::cout << cutl::datetime::now().format() << " execute Task" << std::endl; });
    std::thread t2(
      [&timer_2s]()
      {
          std::this_thread::sleep_for(std::chrono::seconds(5));
          timer_2s.cancel();
      });

    eventLoop.start();

    std::cout << "main thread exit" << std::endl;
}

void test_case_02()
{
    PrintSubTitle("test_case_02");

    cutl::eventloop eventLoop;
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
    auto timer_2s = eventLoop.post_timer_event(
      "timer_1.5s",
      []() { std::cout << cutl::datetime::now().format() << " execute per 1.5s" << std::endl; },
      std::chrono::milliseconds(1500));
    eventLoop.post_event(
      []() { std::cout << cutl::datetime::now().format() << " execute Task" << std::endl; });

    std::thread loop([&eventLoop]() { eventLoop.start(); });
    std::cout << "main thread id:" << std::this_thread::get_id()
              << ", eventloop thread id:" << loop.get_id() << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(6));
    timer_2s.cancel();

    loop.join();
    std::cout << "main thread exit" << std::endl;
}

void TestEventLoop()
{
    PrintTitle("Test EventLoop");

    // test_case_01();
    test_case_02();
}