/**
 * @file test_timecount.cpp
 * @brief Unit tests for cutl::steady_timecounter / cutl::cpu_timecounter.
 */

#include "common_util/timecount.h"
#include <chrono>
#include <gtest/gtest.h>
#include <thread>

TEST(TimecountTest, SteadyTimeCounterRunsWithoutCrash)
{
    {
        cutl::steady_timecounter counter("ut_steady_timecounter");
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    SUCCEED();
}

TEST(TimecountTest, AliasIsSteadyTimeCounter)
{
    {
        cutl::timecount counter("ut_timecount_alias");
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    SUCCEED();
}

TEST(TimecountTest, CpuTimeCounterRunsWithoutCrash)
{
    {
        cutl::cpu_timecounter counter("ut_cpu_timecounter");
        // 一些计算工作让 CPU 时间增加
        volatile double v = 0.0;
        for (int i = 0; i < 10000; ++i)
        {
            v += static_cast<double>(i) * 0.5;
        }
        (void)v;
    }
    SUCCEED();
}
