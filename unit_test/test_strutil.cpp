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

// keep 为 0 时该侧取 len/4。长度不足 4 时两侧都是 0，结果与原串等长的 '*'。
TEST(StrUtilTest, DesensitizingAutoKeep)
{
    EXPECT_EQ(cutl::desensitizing(""), "");
    EXPECT_EQ(cutl::desensitizing("a"), "*");
    EXPECT_EQ(cutl::desensitizing("ab"), "**");
    EXPECT_EQ(cutl::desensitizing("abc"), "***");
    EXPECT_EQ(cutl::desensitizing("abcd"), "a**d");
    EXPECT_EQ(cutl::desensitizing("abcde"), "a***e");
    EXPECT_EQ(cutl::desensitizing("123456"), "1****6");
    EXPECT_EQ(cutl::desensitizing("2515774"), "2*****4");
    EXPECT_EQ(cutl::desensitizing("12345678"), "12****78");
    EXPECT_EQ(cutl::desensitizing("123456789"), "12*****89");
}

// 隐藏个数等于 6 时仍按实际个数填 '*'；大于 6 时中间固定为 "**..**"。
TEST(StrUtilTest, DesensitizingHiddenBoundary)
{
    EXPECT_EQ(cutl::desensitizing("1234567890"), "12******90");
    EXPECT_EQ(cutl::desensitizing("12345678901"), "12**..**01");
    EXPECT_EQ(cutl::desensitizing("123456789012"), "123******012");
    EXPECT_EQ(cutl::desensitizing("1234567890123"), "123**..**123");
    EXPECT_EQ(cutl::desensitizing("18500425678"), "18**..**78");

    EXPECT_EQ(cutl::desensitizing("1234567890", 3, 1), "123******0");
    EXPECT_EQ(cutl::desensitizing("12345678901", 2, 2), "12**..**01");
    EXPECT_EQ(cutl::desensitizing("12345678901", 1, 1), "1**..**1");
}

TEST(StrUtilTest, DesensitizingExplicitKeep)
{
    EXPECT_EQ(cutl::desensitizing("18500425678", 3, 4), "185****5678");
    EXPECT_EQ(cutl::desensitizing("18500425678", 3, 2), "185******78");
    EXPECT_EQ(cutl::desensitizing("18500425678", 2, 2), "18**..**78");
    EXPECT_EQ(cutl::desensitizing("abcde", 2, 2), "ab*de");
    EXPECT_EQ(cutl::desensitizing("abcdefghij", 4, 0), "abcd****ij");
}

// 为 0 的一侧单独按 len/4 计算，另一侧使用传入值。
TEST(StrUtilTest, DesensitizingOneSideAuto)
{
    EXPECT_EQ(cutl::desensitizing("12345678", 1, 0), "1*****78");
    EXPECT_EQ(cutl::desensitizing("12345678", 0, 1), "12*****8");
    EXPECT_EQ(cutl::desensitizing("abc", 1, 0), "a**");
    EXPECT_EQ(cutl::desensitizing("abc", 0, 1), "**c");
    EXPECT_EQ(cutl::desensitizing("hello", 3, 0), "hel*o");
    // 显式传入的 0 与缺省参数相同，不能表示“这一侧一个都不留”。
    EXPECT_EQ(cutl::desensitizing("12345678", 0, 2), "12****78");
}

// 两侧保留长度之和大于等于原串时，整串替换成等长的 '*'。空串仍是空串。
TEST(StrUtilTest, DesensitizingKeepCoversString)
{
    EXPECT_EQ(cutl::desensitizing("", 3, 4), "");
    EXPECT_EQ(cutl::desensitizing("abcd", 2, 2), "****");
    EXPECT_EQ(cutl::desensitizing("abcd", 3, 3), "****");
    EXPECT_EQ(cutl::desensitizing("abcd", 4, 0), "****");
    EXPECT_EQ(cutl::desensitizing("abcd", 0, 4), "****");
    EXPECT_EQ(cutl::desensitizing("hello", 5, 1), "*****");
    EXPECT_EQ(cutl::desensitizing("hello", 10, 0), "*****");
    EXPECT_EQ(cutl::desensitizing("ab", 1, 2), "**");
    EXPECT_EQ(cutl::desensitizing("a", 255, 255), "*");
}

// keep 用 uint8_t 传入，相加和与长度比较时不能按 8 位回绕。
TEST(StrUtilTest, DesensitizingLargeKeep)
{
    const std::string covered(400, 'a');
    EXPECT_EQ(cutl::desensitizing(covered, 255, 255), std::string(400, '*'));

    const std::string long_text(300, 'b');
    std::string abbreviated(255, 'b');
    abbreviated += "**..**";
    abbreviated += 'b';
    EXPECT_EQ(cutl::desensitizing(long_text, 255, 1), abbreviated);

    const std::string longer(600, 'c');
    std::string both(255, 'c');
    both += "**..**";
    both.append(255, 'c');
    EXPECT_EQ(cutl::desensitizing(longer, 255, 255), both);
}

// 按字节处理：内嵌 '\0' 和多字节字符都计入长度，不按码点切分。
TEST(StrUtilTest, DesensitizingBytes)
{
    const std::string with_nul("a\0bcdefghi", 10);
    std::string expected("a\0", 2);
    expected += "******";
    expected += "hi";
    EXPECT_EQ(cutl::desensitizing(with_nul), expected);

    const std::string one = "\xe4\xb8\xad";
    EXPECT_EQ(one.size(), 3u);
    EXPECT_EQ(cutl::desensitizing(one), "***");

    const std::string text = "\xe4\xbd\xa0\xe5\xa5\xbd\xe4\xb8\x96\xe7\x95\x8c";
    EXPECT_EQ(text.size(), 12u);
    EXPECT_EQ(cutl::desensitizing(text), std::string("\xe4\xbd\xa0") + "******" + "\xe7\x95\x8c");
}

TEST(StrUtilTest, DesensitizingLeavesInputUnchanged)
{
    std::string phone = "18500425678";
    EXPECT_EQ(cutl::desensitizing(phone, 3, 4), "185****5678");
    EXPECT_EQ(phone, "18500425678");
}

TEST(StrUtilTest, DesensitizingAutoLengthSweep)
{
    for (size_t len = 0; len <= 64; ++len)
    {
        const std::string input(len, 'x');
        const std::string out = cutl::desensitizing(input);
        const size_t left = len / 4;
        const size_t right = len / 4;
        if (len == 0 || left + right >= len)
        {
            EXPECT_EQ(out, std::string(len, '*')) << "len=" << len;
            continue;
        }
        EXPECT_EQ(out.substr(0, left), std::string(left, 'x')) << "len=" << len;
        EXPECT_EQ(out.substr(out.size() - right), std::string(right, 'x')) << "len=" << len;
        const size_t hidden = len - left - right;
        if (hidden > 6)
        {
            EXPECT_EQ(out.size(), left + 6 + right) << "len=" << len;
            EXPECT_EQ(out.substr(left, 6), "**..**") << "len=" << len;
        }
        else
        {
            EXPECT_EQ(out.size(), len) << "len=" << len;
            EXPECT_EQ(out.substr(left, hidden), std::string(hidden, '*')) << "len=" << len;
        }
    }
}

TEST(StrUtilTest, WideStringRoundTrip)
{
    // 仅做 ASCII 范围的字符往返，避免 locale 差异影响测试稳定性。
    std::string s = "Hello, common_util!";
    auto ws = cutl::s2ws(s);
    auto back = cutl::ws2s(ws);
    EXPECT_EQ(back, s);
}
