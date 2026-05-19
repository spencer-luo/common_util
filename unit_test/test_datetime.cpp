/**
 * @file test_datetime.cpp
 * @brief Unit tests for cutl::datetime.
 */

#include "common_util/datetime.h"
#include <gtest/gtest.h>
#include <regex>

TEST(DatetimeTest, ConstructFromMs)
{
    cutl::datetime dt(1714654150785ULL); // 任意 ms 时间戳
    EXPECT_EQ(dt.timestamp(), 1714654150785ULL);
}

TEST(DatetimeTest, NowAndFormat)
{
    auto now = cutl::datetime::now();
    EXPECT_GT(now.timestamp(), 1700000000000ULL);

    // 默认格式 YYYY-MM-DD HH:MM:SS.sss
    auto str = now.format();
    std::regex pattern(R"(^\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\.\d{3}$)");
    EXPECT_TRUE(std::regex_match(str, pattern)) << "format str=" << str;

    auto str_no_ms = now.format(cutl::datetime_format::datetime_format_a, true, false);
    std::regex pattern2(R"(^\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}$)");
    EXPECT_TRUE(std::regex_match(str_no_ms, pattern2)) << "format str=" << str_no_ms;

    // utctime 应当是 23 个字符且包含 '-'
    auto utc = now.utctime();
    EXPECT_FALSE(utc.empty());
    EXPECT_NE(utc.find('-'), std::string::npos);
}

TEST(DatetimeTest, ParseValidString)
{
    auto dt = cutl::datetime::get("2024-03-02 14:18:44");
    EXPECT_NE(dt.timestamp(), 0u);

    auto dt_ms = cutl::datetime::get("2024-03-02 14:18:44.023");
    EXPECT_EQ(dt_ms.timestamp() % 1000ULL, 23u);

    auto dt_b = cutl::datetime::get("2024.03.02 14:18:44");
    EXPECT_EQ(dt.timestamp(), dt_b.timestamp());
    auto dt_c = cutl::datetime::get("2024/03/02 14:18:44");
    EXPECT_EQ(dt.timestamp(), dt_c.timestamp());
    auto dt_d = cutl::datetime::get("20240302 14:18:44");
    EXPECT_EQ(dt.timestamp(), dt_d.timestamp());
}

TEST(DatetimeTest, ParseInvalidStringReturnsZero)
{
    // 格式错误
    EXPECT_EQ(cutl::datetime::get("2024-0322 14:18:44").timestamp(), 0u);
    // 月份越界
    EXPECT_EQ(cutl::datetime::get("2024-13-02 14:18:44").timestamp(), 0u);
    // 闰年错误
    EXPECT_EQ(cutl::datetime::get("2023-02-29 14:18:44").timestamp(), 0u);
    // 日期越界
    EXPECT_EQ(cutl::datetime::get("2024-03-42 14:18:44").timestamp(), 0u);
}

TEST(DatetimeTest, AssignmentAndOperators)
{
    auto a = cutl::datetime::get("2024-03-02 10:00:00");
    auto b = a;
    EXPECT_EQ(a.timestamp(), b.timestamp());

    auto c = a + cutl::datetime::min;
    EXPECT_EQ(c.timestamp(), a.timestamp() + cutl::datetime::min);

    auto d = a - cutl::datetime::min;
    EXPECT_EQ(d.timestamp(), a.timestamp() - cutl::datetime::min);

    a += cutl::datetime::hour;
    EXPECT_EQ(a.timestamp(), b.timestamp() + cutl::datetime::hour);
    a -= cutl::datetime::hour;
    EXPECT_EQ(a.timestamp(), b.timestamp());
}

TEST(DatetimeTest, DurationBetweenTwoDatetimes)
{
    auto t1 = cutl::datetime::get("2024-03-01 10:00:00");
    auto t2 = cutl::datetime::get("2024-03-01 11:00:00");
    auto duration = t2 - t1;
    EXPECT_EQ(duration, static_cast<int64_t>(cutl::datetime::hour));

    auto neg_duration = t1 - t2;
    EXPECT_EQ(neg_duration, -static_cast<int64_t>(cutl::datetime::hour));
}

TEST(DatetimeTest, Constants)
{
    EXPECT_EQ(cutl::datetime::second, 1000);
    EXPECT_EQ(cutl::datetime::min, 60 * 1000);
    EXPECT_EQ(cutl::datetime::hour, 60 * 60 * 1000);
    EXPECT_EQ(cutl::datetime::day, 24 * 60 * 60 * 1000);
}

TEST(DatetimeTest, FormatWithCustomString)
{
    auto dt = cutl::datetime::get("2024-03-02 14:18:44");
    // 自定义格式：%Y/%m/%d
    EXPECT_EQ(dt.format("%Y/%m/%d", true, false), "2024/03/02");
    // 仅小时
    EXPECT_EQ(dt.format("%H", true, false), "14");
    // %f 是毫秒占位符
    auto dt_ms = cutl::datetime::get("2024-03-02 14:18:44.025");
    auto with_ms = dt_ms.format("%H:%M:%S.%f", true, true);
    // 应当包含 .025
    EXPECT_NE(with_ms.find(".025"), std::string::npos) << with_ms;
}

TEST(DatetimeTest, OutputStreamOperator)
{
    auto dt = cutl::datetime::get("2024-03-02 14:18:44");
    std::ostringstream oss;
    oss << dt;
    auto s = oss.str();
    // 默认输出格式包含日期数字
    EXPECT_NE(s.find("2024"), std::string::npos);
    EXPECT_NE(s.find("03"), std::string::npos);
    EXPECT_NE(s.find("14:18:44"), std::string::npos) << s;
}
