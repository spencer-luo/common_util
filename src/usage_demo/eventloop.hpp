#include "common.hpp"
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
          std::cout << "execute per 1s " << repet_count << " times." << std::endl;
      },
      std::chrono::seconds(1),
      3);
    auto timer_2s = eventLoop.post_timer_event(
      "timer_2s",
      []() { std::cout << "execute per 2s" << std::endl; },
      std::chrono::milliseconds(1500));
    eventLoop.post_event([]() { std::cout << "execute Task" << std::endl; });
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
}

void TestEventLoop()
{
    PrintTitle("Test EventLoop");

    test_case_01();
    // test_case_02();
}