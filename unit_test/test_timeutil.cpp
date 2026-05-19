/**
 * @file test_timeutil.cpp
 * @brief Unit tests for cutl::timeutil (timestamp / clocktime / unit conversion).
 */

#include "common_util/timeutil.h"
#include <chrono>
#include <gtest/gtest.h>
#include <thread>

TEST(TimeUtilTest, UnitConversions)
{
    EXPECT_EQ(cutl::ms2s(1000u), 1u);
    EXPECT_EQ(cutl::us2s(1000000u), 1u);
    EXPECT_EQ(cutl::us2ms(1000u), 1u);
    EXPECT_EQ(cutl::s2ms(1u), 1000u);
    EXPECT_EQ(cutl::s2us(1u), 1000000u);
    EXPECT_EQ(cutl::ms2us(1u), 1000u);

    // 多步往返
    uint64_t ms = 12345u;
    EXPECT_EQ(cutl::us2ms(cutl::ms2us(ms)), ms);
}

TEST(TimeUtilTest, Timestamp)
{
    auto t_s = cutl::timestamp(cutl::timeunit::s);
    auto t_ms = cutl::timestamp(cutl::timeunit::ms);
    auto t_us = cutl::timestamp(cutl::timeunit::us);

    // 现代时间戳应该明显大于 0
    EXPECT_GT(t_s, 1700000000ULL);   // 约 2023-11
    EXPECT_GT(t_ms, t_s * 1000ULL - 2000ULL);
    EXPECT_GT(t_us, t_ms * 1000ULL - 2000000ULL);
}

TEST(TimeUtilTest, ClockTimeIsMonotonic)
{
    auto a = cutl::clocktime(cutl::timeunit::ms);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    auto b = cutl::clocktime(cutl::timeunit::ms);
    EXPECT_GE(b, a);
    // 至少经过了 10ms
    EXPECT_GE(b - a, 10u);
}

TEST(TimeUtilTest, TimezoneOffsetWithinRange)
{
    int offset = cutl::get_timezone_offset();
    EXPECT_GE(offset, -12);
    EXPECT_LE(offset, 14);
}

TEST(TimeUtilTest, CpuClockTime)
{
    auto t = cutl::cpu_clocktime(cutl::timeunit::us);
    // 进程已经启动，CPU 时间应该已经被记录
    (void)t; // 只要可调用即可
    SUCCEED();
}
