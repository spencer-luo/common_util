#include "common.hpp"
#include "datetime.h"
#include "eventloop.h"

void test_case_01()
{
    PrintSubTitle("test_case_01");

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

void test_case_02()
{
    PrintSubTitle("test_case_02");

    cutl::eventloop eventLoop(2, 1);
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

    std::thread loop([&eventLoop]() { eventLoop.start(); });
    std::cout << "main thread id:" << std::this_thread::get_id()
              << ", eventloop thread id:" << loop.get_id() << std::endl;

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

    loop.join();
    std::cout << "main thread exit" << std::endl;
}

void test_case_03()
{
    PrintSubTitle("test_case_03");

    cutl::multithread_eventloop eventLoop;
    // timer task
    int repet_count = 0;
    eventLoop.post_timer_event(
      "timer_1s",
      [&repet_count]()
      {
          repet_count++;
          auto tid = cutl::get_current_thread_tid();
          std::cout << cutl::datetime::now().format() << " (" << tid << ") timer_1s start ... "
                    << std::endl;
          if (repet_count % 2 == 0)
          {
              std::this_thread::sleep_for(std::chrono::seconds(2));
          }
          std::cout << cutl::datetime::now().format() << " execute per 1s " << repet_count
                    << " times." << std::endl;
          std::cout << cutl::datetime::now().format() << " (" << tid << ") timer_1s end. "
                    << std::endl;
      },
      std::chrono::seconds(1),
      3);
    // normal task
    eventLoop.post_event(
      []()
      {
          auto tid = cutl::get_current_thread_tid();
          std::cout << cutl::datetime::now().format() << " (" << tid << ") Task 01 start ..."
                    << std::endl;
          std::this_thread::sleep_for(std::chrono::seconds(2));
          std::cout << cutl::datetime::now().format() << " execute Task 01" << std::endl;
          std::cout << cutl::datetime::now().format() << " (" << tid << ") Task 01 end."
                    << std::endl;
      });
    eventLoop.post_event(
      []() { std::cout << cutl::datetime::now().format() << " execute Task 02" << std::endl; });

    // 主线程处理事件循环
    eventLoop.start();

    std::cout << "main thread exit" << std::endl;
}

void TestEventLoop()
{
    PrintTitle("Test EventLoop");

    // test_case_01();
    // test_case_02();
    test_case_03();
}