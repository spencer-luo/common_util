#pragma once

#include "common.hpp"
#include "common_util/datetime.h"
#include "common_util/threadpool.h"
#include "common_util/threadutil.h"

void thread_pool_case_01()
{
    PrintSubTitle("thread_pool case 01");

    cutl::threadpool tp("ThreadPool");
    tp.start(2);
    tp.add_task(
      []()
      {
          auto tid = cutl::get_current_thread_tid();
          std::cout << cutl::datetime::now().format() << " (" << tid << ") Task 1 start ... "
                    << std::endl;
          std::this_thread::sleep_for(std::chrono::seconds(2));
          std::cout << cutl::datetime::now().format() << " (" << tid << ") Task 1 end "
                    << std::endl;
      });
    tp.add_task(
      []()
      {
          auto tid = cutl::get_current_thread_tid();
          std::cout << cutl::datetime::now().format() << " (" << tid << ") Task 2 start ... "
                    << std::endl;
          std::this_thread::sleep_for(std::chrono::seconds(2));
          std::cout << cutl::datetime::now().format() << " (" << tid << ") Task 2 end "
                    << std::endl;
      });
    tp.add_task(
      []()
      {
          auto tid = cutl::get_current_thread_tid();
          std::cout << cutl::datetime::now().format() << " (" << tid << ") Task 3 start ... "
                    << std::endl;
          std::this_thread::sleep_for(std::chrono::seconds(2));
          std::cout << cutl::datetime::now().format() << " (" << tid << ") Task 3 end "
                    << std::endl;
      });

    std::this_thread::sleep_for(std::chrono::seconds(5));
    tp.stop();
    auto mtid = cutl::get_current_thread_tid();
    std::cout << cutl::datetime::now().format() << " (" << mtid << ") Main thread end."
              << std::endl;
}

void thread_pool_case_02()
{
    PrintSubTitle("thread_pool case 02");

    cutl::threadpool tp("ThreadPol", 2);
    tp.start(1);
    tp.add_task(
      []()
      {
          auto tid = cutl::get_current_thread_tid();
          std::cout << cutl::datetime::now().format() << " (" << tid << ") Task 1 start ... "
                    << std::endl;
          std::this_thread::sleep_for(std::chrono::seconds(3));
          std::cout << cutl::datetime::now().format() << " (" << tid << ") Task 1 end "
                    << std::endl;
      });
    tp.add_task(
      []()
      {
          auto tid = cutl::get_current_thread_tid();
          std::cout << cutl::datetime::now().format() << " (" << tid << ") Task 2 start ... "
                    << std::endl;
          std::this_thread::sleep_for(std::chrono::seconds(2));
          std::cout << cutl::datetime::now().format() << " (" << tid << ") Task 2 end "
                    << std::endl;
      });
    tp.add_task(
      []()
      {
          auto tid = cutl::get_current_thread_tid();
          std::cout << cutl::datetime::now().format() << " (" << tid << ") Task 3 start ... "
                    << std::endl;
          std::this_thread::sleep_for(std::chrono::seconds(2));
          std::cout << cutl::datetime::now().format() << " (" << tid << ") Task 3 end "
                    << std::endl;
      },
      std::chrono::seconds(1));
    tp.add_task(
      []()
      {
          auto tid = cutl::get_current_thread_tid();
          std::cout << cutl::datetime::now().format() << " (" << tid << ") Task 4 start ... "
                    << std::endl;
          std::this_thread::sleep_for(std::chrono::seconds(2));
          std::cout << cutl::datetime::now().format() << " (" << tid << ") Task 4 end "
                    << std::endl;
      },
      std::chrono::seconds(1));

    std::this_thread::sleep_for(std::chrono::seconds(8));
    tp.stop();
    auto mtid = cutl::get_current_thread_tid();
    std::cout << cutl::datetime::now().format() << " (" << mtid << ") Main thread end."
              << std::endl;
}

void thread_pool_case_03()
{
    PrintSubTitle("thread_pool case 03");

    // 主线程ID
    auto mtid = cutl::get_current_thread_tid();

    // 创建并启动线程池
    cutl::threadpool tp("ThreadPol", 1);
    tp.start(1);

    // 添加 Task 1
    // std::cout << cutl::datetime::now().format() << " (" << mtid << ") add Task 1" << std::endl;
    auto add = [](int a, int b)
    {
        auto tid = cutl::get_current_thread_tid();
        std::cout << cutl::datetime::now().format() << " (" << tid << ") Task 1 start ... "
                  << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << a << " + " << b << " = " << a + b << std::endl;
        std::cout << cutl::datetime::now().format() << " (" << tid << ") Task 1 end " << std::endl;
    };
    try
    {
        tp.add_task_with_args_and_return(add, 3, 2);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Add task 1 failed: " << e.what() << '\n';
    }

    // TODO: 关闭线程池再添加任务，模拟出错case
    // tp.stop();

    // 添加 Task 2
    // std::cout << cutl::datetime::now().format() << " (" << mtid << ") add Task 2" << std::endl;
    auto sayHello = [](const std::string& name)
    {
        auto tid = cutl::get_current_thread_tid();
        std::cout << cutl::datetime::now().format() << " (" << tid << ") Task 2 start ... "
                  << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << cutl::datetime::now().format() << " (" << tid << ") Task 2 end " << std::endl;
        return "Hello " + name;
    };
    try
    {
        auto res_future2 = tp.add_task_with_args_and_return(sayHello, "Spencer");
        // 等待并打印返回值
        auto result2 = res_future2.get();
        std::cout << cutl::datetime::now().format() << " (" << mtid
                  << ") Task 2 result: " << result2 << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Add task 2 failed: " << e.what() << '\n';
    }

    // 结束线程池
    std::this_thread::sleep_for(std::chrono::seconds(4));
    tp.stop();
    std::cout << cutl::datetime::now().format() << " (" << mtid << ") Main thread end."
              << std::endl;
}

void TestThreadPool()
{
    PrintTitle("Thread Pool Usage Demo");

    // thread_pool_case_01();
    // thread_pool_case_02();
    thread_pool_case_03();
}