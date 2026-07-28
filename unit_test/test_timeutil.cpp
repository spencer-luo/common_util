/**
 * @file test_timeutil.cpp
 * @brief Unit tests for cutl::timeutil (timestamp / clocktime / unit conversion).
 */

#include "common_util/timeutil.h"
#include <atomic>
#include <chrono>
#include <ctime>
#include <gtest/gtest.h>
#include <thread>
#include <vector>

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

// ---------------------------------------------------------------------------
// localtime_s / gmtime_s
// ---------------------------------------------------------------------------

namespace
{
    // struct tm 的逐字段比较，出错时能直接看出是哪个字段不对
    void ExpectTmEq(const struct tm& actual, const struct tm& expected, const char* what)
    {
        EXPECT_EQ(actual.tm_sec, expected.tm_sec) << what << ": tm_sec";
        EXPECT_EQ(actual.tm_min, expected.tm_min) << what << ": tm_min";
        EXPECT_EQ(actual.tm_hour, expected.tm_hour) << what << ": tm_hour";
        EXPECT_EQ(actual.tm_mday, expected.tm_mday) << what << ": tm_mday";
        EXPECT_EQ(actual.tm_mon, expected.tm_mon) << what << ": tm_mon";
        EXPECT_EQ(actual.tm_year, expected.tm_year) << what << ": tm_year";
        EXPECT_EQ(actual.tm_wday, expected.tm_wday) << what << ": tm_wday";
        EXPECT_EQ(actual.tm_yday, expected.tm_yday) << what << ": tm_yday";
        EXPECT_EQ(actual.tm_isdst, expected.tm_isdst) << what << ": tm_isdst";
    }
} // namespace

// gmtime_s 的结果与时区无关，可以直接断言绝对值。
// 注意 tm_year 是 "年份-1900"，tm_mon 从 0 开始，tm_wday 中周日为 0。
TEST(TimeUtilTest, GmtimeKnownTimestamps)
{
    // Unix 纪元原点：1970-01-01 00:00:00 UTC，星期四
    auto epoch = cutl::gmtime_s(static_cast<time_t>(0));
    EXPECT_EQ(epoch.tm_year, 70);
    EXPECT_EQ(epoch.tm_mon, 0);
    EXPECT_EQ(epoch.tm_mday, 1);
    EXPECT_EQ(epoch.tm_hour, 0);
    EXPECT_EQ(epoch.tm_min, 0);
    EXPECT_EQ(epoch.tm_sec, 0);
    EXPECT_EQ(epoch.tm_wday, 4);
    EXPECT_EQ(epoch.tm_yday, 0);
    // gmtime 语义上不存在夏令时
    EXPECT_EQ(epoch.tm_isdst, 0);

    // 2009-02-13 23:31:30 UTC，星期五
    auto t = cutl::gmtime_s(static_cast<time_t>(1234567890));
    EXPECT_EQ(t.tm_year, 109);
    EXPECT_EQ(t.tm_mon, 1);
    EXPECT_EQ(t.tm_mday, 13);
    EXPECT_EQ(t.tm_hour, 23);
    EXPECT_EQ(t.tm_min, 31);
    EXPECT_EQ(t.tm_sec, 30);
    EXPECT_EQ(t.tm_wday, 5);
    EXPECT_EQ(t.tm_yday, 43);
    EXPECT_EQ(t.tm_isdst, 0);
}

// 32 位 time_t 的溢出边界：2038-01-19 03:14:08 UTC。
// 在 time_t 为 64 位的平台上必须能正确换算，否则会回绕到 1901 年。
TEST(TimeUtilTest, GmtimeAfterYear2038)
{
    if (sizeof(time_t) < 8)
    {
        GTEST_SKIP() << "time_t is 32-bit on this platform, 2038 overflow is unavoidable.";
    }

    auto t = cutl::gmtime_s(static_cast<time_t>(2147483648LL));
    EXPECT_EQ(t.tm_year, 138) << "2038 boundary wrapped around, time_t overflow?";
    EXPECT_EQ(t.tm_mon, 0);
    EXPECT_EQ(t.tm_mday, 19);
    EXPECT_EQ(t.tm_hour, 3);
    EXPECT_EQ(t.tm_min, 14);
    EXPECT_EQ(t.tm_sec, 8);
    EXPECT_EQ(t.tm_wday, 2);
}

#ifndef _WIN32
// 纪元之前的时间。Windows 的 _gmtime64_s 不接受负的 time_t，故仅在 *nix 上验证。
TEST(TimeUtilTest, GmtimeBeforeEpoch)
{
    auto t = cutl::gmtime_s(static_cast<time_t>(-1));
    EXPECT_EQ(t.tm_year, 69);
    EXPECT_EQ(t.tm_mon, 11);
    EXPECT_EQ(t.tm_mday, 31);
    EXPECT_EQ(t.tm_hour, 23);
    EXPECT_EQ(t.tm_min, 59);
    EXPECT_EQ(t.tm_sec, 59);
    EXPECT_EQ(t.tm_wday, 3);
    EXPECT_EQ(t.tm_yday, 364);
}

// 与系统自带的可重入版本逐字段对照，确保包装层没有漏拷或改写字段。
TEST(TimeUtilTest, MatchesSystemReentrantVersions)
{
    const time_t samples[] = {
        0,
        1,
        1234567890,
        1700000000,
        static_cast<time_t>(cutl::timestamp(cutl::timeunit::s)),
    };

    for (auto s : samples)
    {
        struct tm sys_gm = {};
        ASSERT_NE(gmtime_r(&s, &sys_gm), nullptr);
        ExpectTmEq(cutl::gmtime_s(s), sys_gm, "gmtime_s");

        struct tm sys_local = {};
        ASSERT_NE(localtime_r(&s, &sys_local), nullptr);
        ExpectTmEq(cutl::localtime_s(s), sys_local, "localtime_s");
    }
}
#endif

// localtime_s 的结果依赖运行环境的时区，所以断言的是与时区无关的不变量：
// 把本地时间再用 mktime 转回 time_t，必须得到原值。
TEST(TimeUtilTest, LocaltimeRoundTripsThroughMktime)
{
    const time_t samples[] = {
        86400, // 1970-01-02，避开纪元原点附近可能为负的本地时间
        1234567890,
        1700000000,
        static_cast<time_t>(cutl::timestamp(cutl::timeunit::s)),
    };

    for (auto s : samples)
    {
        struct tm local = cutl::localtime_s(s);
        // mktime 会修改传入的结构体，所以用副本
        struct tm copy = local;
        time_t back = mktime(&copy);
        EXPECT_EQ(back, s) << "round trip failed for time_t=" << static_cast<long long>(s);
    }
}

// 本地时间与 UTC 的差值应当等于时区偏移（含夏令时）。
// 允许半小时/45 分钟制时区，因此按分钟比较。
TEST(TimeUtilTest, LocaltimeAndGmtimeDifferByTimezoneOffset)
{
    const time_t s = 1700000000;
    struct tm local = cutl::localtime_s(s);
    struct tm utc = cutl::gmtime_s(s);

    // 换算成 "从各自年初起的分钟数" 再相减，避免跨日/跨年时的进位问题
    auto minutes_of_year = [](const struct tm& t) {
        return static_cast<long>(t.tm_yday) * 24 * 60 + t.tm_hour * 60 + t.tm_min;
    };
    long diff = minutes_of_year(local) - minutes_of_year(utc);
    // 跨年时 tm_yday 会从 365 跳回 0，做一次回绕修正
    if (local.tm_year != utc.tm_year)
    {
        diff += (local.tm_year > utc.tm_year ? 1 : -1) * 366 * 24 * 60;
    }

    EXPECT_GE(diff, -12 * 60);
    EXPECT_LE(diff, 14 * 60);
    // 秒数不受时区影响
    EXPECT_EQ(local.tm_sec, utc.tm_sec);
}

// 这两个函数带 _s 后缀、文档也承诺 "Thread safe"，因此并发行为必须验证：
// 若内部误用了不可重入的 localtime/gmtime（返回共享静态缓冲区），
// 多线程下就会读到别的线程写入的结果。
TEST(TimeUtilTest, ThreadSafeUnderConcurrency)
{
    constexpr int kThreads = 8;
    constexpr int kRounds = 2000;

    // 先在单线程下算出基准值
    std::vector<time_t> inputs;
    std::vector<struct tm> expected_gm;
    std::vector<struct tm> expected_local;
    for (int i = 0; i < kThreads; ++i)
    {
        // 各线程使用差异明显的时间戳（相隔约 100 天），一旦串台立刻能发现
        time_t s = static_cast<time_t>(1000000000LL + i * 8640000LL);
        inputs.push_back(s);
        expected_gm.push_back(cutl::gmtime_s(s));
        expected_local.push_back(cutl::localtime_s(s));
    }

    std::atomic<int> mismatches(0);
    std::vector<std::thread> workers;
    for (int i = 0; i < kThreads; ++i)
    {
        workers.emplace_back([&, i]() {
            for (int r = 0; r < kRounds; ++r)
            {
                auto gm = cutl::gmtime_s(inputs[i]);
                if (gm.tm_year != expected_gm[i].tm_year || gm.tm_mon != expected_gm[i].tm_mon ||
                    gm.tm_mday != expected_gm[i].tm_mday || gm.tm_hour != expected_gm[i].tm_hour ||
                    gm.tm_min != expected_gm[i].tm_min || gm.tm_sec != expected_gm[i].tm_sec)
                {
                    ++mismatches;
                }

                auto lo = cutl::localtime_s(inputs[i]);
                if (lo.tm_year != expected_local[i].tm_year || lo.tm_mon != expected_local[i].tm_mon ||
                    lo.tm_mday != expected_local[i].tm_mday || lo.tm_hour != expected_local[i].tm_hour ||
                    lo.tm_min != expected_local[i].tm_min || lo.tm_sec != expected_local[i].tm_sec)
                {
                    ++mismatches;
                }
            }
        });
    }
    for (auto& t : workers)
    {
        t.join();
    }

    EXPECT_EQ(mismatches.load(), 0) << "concurrent conversions returned wrong results, "
                                    << "the implementation is probably not reentrant";
}

// 返回值是按值传递的独立副本，连续两次调用不会互相覆盖。
TEST(TimeUtilTest, ResultsAreIndependentCopies)
{
    auto a = cutl::gmtime_s(static_cast<time_t>(0));
    auto b = cutl::gmtime_s(static_cast<time_t>(1234567890));
    EXPECT_EQ(a.tm_year, 70) << "the first result was overwritten by the second call";
    EXPECT_EQ(b.tm_year, 109);

    auto c = cutl::localtime_s(static_cast<time_t>(86400));
    auto d = cutl::localtime_s(static_cast<time_t>(1700000000));
    EXPECT_NE(c.tm_year * 400 + c.tm_yday, d.tm_year * 400 + d.tm_yday);
}
