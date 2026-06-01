/**
 * @file test_threadutil.cpp
 * @brief Unit tests for cutl::threadutil (thread name / tid).
 */

#include "common_util/threadutil.h"
#include <atomic>
#include <gtest/gtest.h>
#include <thread>

TEST(ThreadUtilTest, GetTidIsConsistentInThread)
{
    auto tid1 = cutl::get_current_thread_tid();
    auto tid2 = cutl::get_current_thread_tid();
    EXPECT_EQ(tid1, tid2);
    EXPECT_NE(tid1, 0);
}

TEST(ThreadUtilTest, DifferentThreadHasDifferentTid)
{
    auto main_tid = cutl::get_current_thread_tid();
    std::atomic<int32_t> sub_tid{0};
    std::thread t([&sub_tid]() { sub_tid = cutl::get_current_thread_tid(); });
    t.join();
    EXPECT_NE(main_tid, sub_tid.load());
}

TEST(ThreadUtilTest, SetAndGetThreadName)
{
    std::thread t([]() {
        cutl::set_current_thread_name("ut_thread");
        auto name = cutl::get_current_thread_name();
        EXPECT_EQ(name, "ut_thread");
    });
    t.join();
}
