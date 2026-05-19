/**
 * @file test_strutil.cpp
 * @brief Unit tests for cutl::strutil (case conversion, strip, split/join, desensitizing, etc.)
 */

#include "common_util/strutil.h"
#include <gtest/gtest.h>

TEST(StrUtilTest, ToUpperLower)
{
    EXPECT_EQ(cutl::to_upper("Hello, World!"), "HELLO, WORLD!");
    EXPECT_EQ(cutl::to_upper(""), "");
    EXPECT_EQ(cutl::to_lower("HELLO, World!"), "hello, world!");
    EXPECT_EQ(cutl::to_lower(""), "");
    EXPECT_EQ(cutl::to_upper("123abcXYZ"), "123ABCXYZ");
    EXPECT_EQ(cutl::to_lower("123abcXYZ"), "123abcxyz");
}

TEST(StrUtilTest, StripVariants)
{
    const std::string text = "  \t hello world \n ";
    EXPECT_EQ(cutl::lstrip(text), "hello world \n ");
    EXPECT_EQ(cutl::rstrip(text), "  \t hello world");
    EXPECT_EQ(cutl::strip(text), "hello world");
    EXPECT_EQ(cutl::strip(""), "");
    EXPECT_EQ(cutl::strip("no_space"), "no_space");
}

TEST(StrUtilTest, StartsWithEndsWith)
{
    EXPECT_TRUE(cutl::starts_with("Hello, world!", "Hello"));
    EXPECT_FALSE(cutl::starts_with("Hello, world!", "hello"));
    EXPECT_TRUE(cutl::starts_with("Hello, world!", "hello", true));
    EXPECT_FALSE(cutl::starts_with("hi", "hello"));

    EXPECT_TRUE(cutl::ends_with("GOODBYE, WORLD!", "WORLD!"));
    EXPECT_FALSE(cutl::ends_with("GOODBYE, WORLD!", "world!"));
    EXPECT_TRUE(cutl::ends_with("GOODBYE, WORLD!", "world!", true));
    EXPECT_FALSE(cutl::ends_with("hi", "hello"));
}

TEST(StrUtilTest, SplitJoin)
{
    auto parts = cutl::split("a,b,c,d", ",");
    ASSERT_EQ(parts.size(), 4u);
    EXPECT_EQ(parts[0], "a");
    EXPECT_EQ(parts[3], "d");

    EXPECT_EQ(cutl::join(parts, "-"), "a-b-c-d");
    EXPECT_EQ(cutl::join({"x", "y"}, ""), "xy");

    auto empty = cutl::split("", ",");
    EXPECT_TRUE(empty.empty());
}

TEST(StrUtilTest, Desensitizing)
{
    EXPECT_EQ(cutl::desensitizing(""), "");
    EXPECT_EQ(cutl::desensitizing("a"), "*");
    EXPECT_EQ(cutl::desensitizing("ab"), "a*");
    // 长度 <= 6 时保留前 2 位，其余替换为 '*'
    EXPECT_EQ(cutl::desensitizing("123456").substr(0, 2), "12");
    EXPECT_EQ(cutl::desensitizing("123456").size(), 6u);

    // 11 位手机号：长度 < 16，前 3 位 + '*' + 后 3 位
    auto phone = cutl::desensitizing("18500425678");
    EXPECT_EQ(phone.substr(0, 3), "185");
    EXPECT_EQ(phone.substr(phone.size() - 3), "678");
    EXPECT_NE(phone.find('*'), std::string::npos);
}

TEST(StrUtilTest, WideStringRoundTrip)
{
    // 仅做 ASCII 范围的字符往返，避免 locale 差异影响测试稳定性。
    std::string s = "Hello, common_util!";
    auto ws = cutl::s2ws(s);
    auto back = cutl::ws2s(ws);
    EXPECT_EQ(back, s);
}
