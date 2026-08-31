/**
 * @file test_strutil.cpp
 * @brief Unit tests for cutl::strutil (case conversion, strip, split/join, desensitizing, etc.)
 */

#include "common_util/strutil.h"
#include <gtest/gtest.h>
#include <string>
#include <utility>
#include <vector>

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

    // 单字符也走同一个字符串分隔符接口
    auto dotted = cutl::split("a.b.c", ".");
    ASSERT_EQ(dotted.size(), 3u);
    EXPECT_EQ(dotted[0], "a");
    EXPECT_EQ(dotted[1], "b");
    EXPECT_EQ(dotted[2], "c");
}

// 多字符分隔符必须作为一个整体匹配。
// 旧实现每次只前进 1 个字符，导致分隔符的第 2 个及后续字符被算进下一段，
// 例如 split("a::b", "::") 会得到 {"a", ":b"}。
TEST(StrUtilTest, SplitByMultiCharSeparator)
{
    auto parts = cutl::split("a::b::c", "::");
    ASSERT_EQ(parts.size(), 3u);
    EXPECT_EQ(parts[0], "a");
    EXPECT_EQ(parts[1], "b");
    EXPECT_EQ(parts[2], "c");

    auto tags = cutl::split("a<>b", "<>");
    ASSERT_EQ(tags.size(), 2u);
    EXPECT_EQ(tags[0], "a");
    EXPECT_EQ(tags[1], "b");

    // 逗号加空格这种常见分隔符
    auto nums = cutl::split("1, 2, 3", ", ");
    ASSERT_EQ(nums.size(), 3u);
    EXPECT_EQ(nums[0], "1");
    EXPECT_EQ(nums[1], "2");
    EXPECT_EQ(nums[2], "3");

    // 连续出现的多字符分隔符之间应当切出一个空串
    auto consecutive = cutl::split("a::::b", "::");
    ASSERT_EQ(consecutive.size(), 3u);
    EXPECT_EQ(consecutive[0], "a");
    EXPECT_EQ(consecutive[1], "");
    EXPECT_EQ(consecutive[2], "b");

    // 分隔符自身发生重叠时，从左到右不回退匹配，与 Python "aaa".split("aa") 一致
    auto overlap = cutl::split("aaa", "aa");
    ASSERT_EQ(overlap.size(), 2u);
    EXPECT_EQ(overlap[0], "");
    EXPECT_EQ(overlap[1], "a");

    // 分隔符比原串更长，或者没有出现，都应原样返回一整段
    auto longer = cutl::split("ab", "abcd");
    ASSERT_EQ(longer.size(), 1u);
    EXPECT_EQ(longer[0], "ab");

    auto absent = cutl::split("abc", "::");
    ASSERT_EQ(absent.size(), 1u);
    EXPECT_EQ(absent[0], "abc");
}

// 首尾的分隔符会在对应侧产生空串，与 Python str.split() 行为一致。
TEST(StrUtilTest, SplitKeepsEmptyPartsAtBothEnds)
{
    auto trailing = cutl::split("a,b,", ",");
    ASSERT_EQ(trailing.size(), 3u);
    EXPECT_EQ(trailing[2], "");

    auto leading = cutl::split(",a", ",");
    ASSERT_EQ(leading.size(), 2u);
    EXPECT_EQ(leading[0], "");
    EXPECT_EQ(leading[1], "a");

    // 全是分隔符时，n 个分隔符切出 n+1 个空串
    auto all = cutl::split("aaa", "a");
    ASSERT_EQ(all.size(), 4u);
    for (const auto& s : all)
    {
        EXPECT_EQ(s, "");
    }

    auto both = cutl::split("::a::", "::");
    ASSERT_EQ(both.size(), 3u);
    EXPECT_EQ(both[0], "");
    EXPECT_EQ(both[1], "a");
    EXPECT_EQ(both[2], "");
}

// 空分隔符无法切分，返回原串本身，且不能死循环。
TEST(StrUtilTest, SplitByEmptySeparator)
{
    auto parts = cutl::split("abc", "");
    ASSERT_EQ(parts.size(), 1u);
    EXPECT_EQ(parts[0], "abc");

    EXPECT_TRUE(cutl::split("", "").empty());
}

// split 与 join 互为逆操作：只要原串里的分隔符都被切开，再拼回去必须还原。
TEST(StrUtilTest, SplitJoinRoundTrip)
{
    const std::vector<std::pair<std::string, std::string>> cases = {
        { "a,b,c", "," },   { "a::b::c", "::" },  { "1, 2, 3", ", " },
        { "a", "::" },      { "a::b::", "::" },   { "::a::b", "::" },
        { "a<>b<><>c", "<>" },
    };

    for (const auto& item : cases)
    {
        const auto& str = item.first;
        const auto& sep = item.second;
        EXPECT_EQ(cutl::join(cutl::split(str, sep), sep), str)
          << "str=" << str << ", separator=" << sep;
    }
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
