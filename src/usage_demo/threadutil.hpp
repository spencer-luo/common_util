#pragma once

#include "common.hpp"
#include "threadutil.h"
#include <thread>
#include <iostream>

void TestThreadUtil()
{
    PrintTitle("Thread Util Demo");

    auto th = std::thread([](){
        cutl::set_current_thread_name("sub_thread");
        std::cout << "Sub thread running..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        auto name = cutl::get_current_thread_name();
        auto tid = cutl::get_current_thread_tid();
        std::cout << "Sub thread id: " << tid << ", name: " << name << std::endl;
        std::cout << "Sub thread stopped." << std::endl;
    });
    th.join();

    cutl::set_current_thread_name("main_thread");
    auto name = cutl::get_current_thread_name();
    auto tid = cutl::get_current_thread_tid();
    std::cout << "Main thread id: " << tid << ", name: " << name << std::endl;
}