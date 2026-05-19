/**
 * @file test_strfmt.cpp
 * @brief Unit tests for cutl::strfmt (alignment, number formatting, hex/bin, container fmt).
 */

#include "common_util/strfmt.h"
#include <array>
#include <gtest/gtest.h>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

TEST(StrFmtTest, AlignStr)
{
    EXPECT_EQ(cutl::align_str_left("abc", 6, '-'), "abc---");
    EXPECT_EQ(cutl::align_str_right("abc", 6, '-'), "---abc");
    // 字符串长度等于宽度时，原样返回
    EXPECT_EQ(cutl::align_str_left("hello", 5, '-'), "hello");
    EXPECT_EQ(cutl::align_str_right("hello", 5, '-'), "hello");
}

TEST(StrFmtTest, FmtUintAndDouble)
{
    EXPECT_EQ(cutl::fmt_uint(12), "12");
    EXPECT_EQ(cutl::fmt_uint(12, 5), "00012");
    EXPECT_EQ(cutl::fmt_uint(12, 4, 'x'), "xx12");
    EXPECT_EQ(cutl::fmt_double(3.141592653, 4), "3.1416");
    EXPECT_EQ(cutl::fmt_double(3.0, 2), "3.00");
}

TEST(StrFmtTest, FmtFileSize)
{
    // 0 字节
    EXPECT_NE(cutl::fmt_filesize(0).find("B"), std::string::npos);
    // 1KB+ 单位走 K 分支
    auto kb = cutl::fmt_filesize(2048, true, 1);
    EXPECT_NE(kb.find('K'), std::string::npos);
    // 非简化模式带完整单位
    auto kb_full = cutl::fmt_filesize(2048, false, 1);
    EXPECT_NE(kb_full.find("KB"), std::string::npos);
    // MB 范围
    auto mb = cutl::fmt_filesize(5 * 1024 * 1024ULL, true, 1);
    EXPECT_NE(mb.find('M'), std::string::npos);
}

TEST(StrFmtTest, FmtTimeDuration)
{
    // 65 秒 -> 1m:05s
    auto s = cutl::fmt_timeduration_s(65);
    EXPECT_NE(s.find("01m:05s"), std::string::npos);
    // 1.123 秒（毫秒接口）
    auto ms = cutl::fmt_timeduration_ms(1123);
    EXPECT_NE(ms.find("01s"), std::string::npos);
    EXPECT_NE(ms.find("123ms"), std::string::npos);
    // 1.000123 秒（微秒接口）
    auto us = cutl::fmt_timeduration_us(1000123);
    EXPECT_NE(us.find("01s"), std::string::npos);
    EXPECT_NE(us.find("us"), std::string::npos);
}

TEST(StrFmtTest, FmtTimeZone)
{
    EXPECT_EQ(cutl::fmt_timezone_offset(0), "UTC+00:00");
    EXPECT_EQ(cutl::fmt_timezone_offset(8), "UTC+08:00");
    EXPECT_EQ(cutl::fmt_timezone_offset(-5), "UTC-05:00");
    // 仅断言系统时区字符串以 UTC 开头
    auto sys_tz = cutl::fmt_system_timezone();
    EXPECT_EQ(sys_tz.substr(0, 3), "UTC");
}

TEST(StrFmtTest, ToHex)
{
    EXPECT_EQ(cutl::to_hex(static_cast<uint8_t>(0x0f)), "0F");
    EXPECT_EQ(cutl::to_hex(static_cast<uint8_t>(0x0f), false), "0f");
    EXPECT_EQ(cutl::to_hex(static_cast<uint16_t>(0x00FC)), "00FC");
    EXPECT_EQ(cutl::to_hex(static_cast<uint32_t>(0x1B02AEFC)), "1B02AEFC");
    EXPECT_EQ(cutl::to_hex(static_cast<uint64_t>(0xABCDEF0123456789ULL)), "ABCDEF0123456789");

    uint8_t bytes[] = {0x01, 0x02, 0xAB};
    auto text = cutl::to_hex(bytes, 3, true, ' ');
    EXPECT_EQ(text, "01 02 AB ");
    auto no_sep = cutl::to_hex(bytes, 3, true, 0);
    EXPECT_EQ(no_sep, "0102AB");

    // 高位字节也应正确（覆盖之前 signed char 溢出的回归路径）。
    uint8_t high_bytes[] = {0x80, 0xFF, 0xCD};
    EXPECT_EQ(cutl::to_hex(high_bytes, 3, true, 0), "80FFCD");
    EXPECT_EQ(cutl::to_hex(high_bytes, 3, false, 0), "80ffcd");
}

TEST(StrFmtTest, ToBin)
{
    EXPECT_EQ(cutl::to_bin(static_cast<uint8_t>(0x0f)), "0000,1111");
    EXPECT_EQ(cutl::to_bin(static_cast<uint16_t>(0x00FC)), "0000,0000 1111,1100");

    // 32 位 / 64 位的二进制格式：每 4 位一组以','分隔，每 8 位一组以分隔符隔开（默认空格）
    auto u32_bin = cutl::to_bin(static_cast<uint32_t>(0x000000FCu));
    EXPECT_NE(u32_bin.find("1111,1100"), std::string::npos);
    auto u64_bin = cutl::to_bin(static_cast<uint64_t>(0x00000000000000FCULL));
    EXPECT_NE(u64_bin.find("1111,1100"), std::string::npos);
    // 自定义分隔符
    auto u32_bin_dash = cutl::to_bin(static_cast<uint32_t>(0x000000FFu), '-');
    EXPECT_NE(u32_bin_dash.find('-'), std::string::npos);
}

TEST(StrFmtTest, ToHexPointer)
{
    int x = 0;
    void* p = &x;
    auto s_lower = cutl::to_hex(p);
    auto s_upper = cutl::to_hex(p, true);
    EXPECT_FALSE(s_lower.empty());
    EXPECT_FALSE(s_upper.empty());
    // 上行/下行表示同一指针应包含相同位数（不算大小写）
    EXPECT_EQ(s_lower.size(), s_upper.size());
    // 自动补 "0" 前缀的形式
    auto s_filled = cutl::to_hex(p, false, true);
    EXPECT_FALSE(s_filled.empty());
}

TEST(StrFmtTest, FmtTimestampVariants)
{
    // 一个固定的 UTC 时间戳：2024-05-01 00:00:00 UTC = 1714521600
    constexpr uint64_t kEpochSec = 1714521600ULL;

    // UTC 模式下日期应当稳定为 2024-05-01
    auto utc_str = cutl::fmt_timestamp_s(kEpochSec, /*local=*/false);
    EXPECT_NE(utc_str.find("2024-05-01"), std::string::npos) << utc_str;

    // 自定义格式：仅打印年份 4 位
    auto only_year = cutl::fmt_timestamp(kEpochSec, /*local=*/false, "%Y");
    EXPECT_EQ(only_year, "2024");
    auto ymd = cutl::fmt_timestamp(kEpochSec, /*local=*/false, "%Y/%m/%d");
    EXPECT_EQ(ymd, "2024/05/01");

    // ms / us 接口：UTC 下日期与 _s 一致
    auto ms_str = cutl::fmt_timestamp_ms(kEpochSec * 1000ULL, /*local=*/false);
    EXPECT_NE(ms_str.find("2024-05-01"), std::string::npos) << ms_str;
    auto us_str = cutl::fmt_timestamp_us(kEpochSec * 1000000ULL, /*local=*/false);
    EXPECT_NE(us_str.find("2024-05-01"), std::string::npos) << us_str;
}

TEST(StrFmtTest, FmtUnorderedMapAndSet)
{
    std::unordered_map<int, int> m = {{1, 10}, {2, 20}};
    auto s = cutl::fmt_unordered_map(m);
    EXPECT_NE(s.find("1: 10"), std::string::npos);
    EXPECT_NE(s.find("2: 20"), std::string::npos);
    EXPECT_EQ(cutl::fmt_unordered_map(std::unordered_map<int, int>{}), "{}");

    // 多行格式
    auto s_multi = cutl::fmt_unordered_map(m, true);
    EXPECT_NE(s_multi.find("\n"), std::string::npos);

    std::unordered_set<int> us = {1, 2, 3};
    auto us_str = cutl::fmt_unordered_set(us);
    // 顺序不固定，但 3 个数字与边框都应出现
    EXPECT_NE(us_str.find('{'), std::string::npos);
    EXPECT_NE(us_str.find('}'), std::string::npos);
    EXPECT_NE(us_str.find('1'), std::string::npos);
    EXPECT_NE(us_str.find('2'), std::string::npos);
    EXPECT_NE(us_str.find('3'), std::string::npos);
    EXPECT_EQ(cutl::fmt_unordered_set(std::unordered_set<int>{}), "{}");
}

TEST(StrFmtTest, FmtArrVecMapSet)
{
    int arr[] = {1, 2, 3};
    EXPECT_EQ(cutl::fmt_arr(arr, 3), "[1, 2, 3]");
    EXPECT_EQ(cutl::fmt_arr<int>(nullptr, 0), "[]");

    std::array<int, 3> stdarr = {1, 2, 3};
    EXPECT_EQ(cutl::fmt_arr(stdarr), "[1, 2, 3]");

    std::vector<int> vec = {1, 2, 3};
    EXPECT_EQ(cutl::fmt_vec(vec), "[1, 2, 3]");
    EXPECT_EQ(cutl::fmt_vec(std::vector<int>{}), "[]");

    std::map<int, int> mp = {{1, 10}, {2, 20}};
    auto mp_str = cutl::fmt_map(mp);
    EXPECT_NE(mp_str.find("1: 10"), std::string::npos);
    EXPECT_NE(mp_str.find("2: 20"), std::string::npos);
    EXPECT_EQ(cutl::fmt_map(std::map<int, int>{}), "{}");

    std::set<int> s = {3, 1, 2};
    EXPECT_EQ(cutl::fmt_set(s), "{1, 2, 3}");
    EXPECT_EQ(cutl::fmt_set(std::set<int>{}), "{}");
}

TEST(StrFmtTest, FmtMatrixIntegers)
{
    int matrix[2 * 2] = {1, 2, 3, 4};
    auto text = cutl::fmt_matrix(matrix, 2, 2, "M", 0, 0, 0, 2, 2);
    EXPECT_NE(text.find("1"), std::string::npos);
    EXPECT_NE(text.find("4"), std::string::npos);
    EXPECT_NE(text.find("M"), std::string::npos);
}
