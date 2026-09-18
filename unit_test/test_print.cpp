/**
 * @file test_print.cpp
 * @brief Unit tests for cutl::print_* helpers (capture stdout for content check).
 */

#include "common_util/print.h"
#include <array>
#include <functional>
#include <gtest/gtest.h>
#include <map>
#include <set>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace
{

// 把 std::cout 重定向到 stringstream 期间执行 fn，返回捕获到的字符串。
std::string CaptureStdout(const std::function<void()>& fn)
{
    std::stringstream buf;
    auto* old = std::cout.rdbuf(buf.rdbuf());
    fn();
    std::cout.rdbuf(old);
    return buf.str();
}

} // namespace

TEST(PrintTest, PrintArr)
{
    int arr[] = {1, 2, 3};
    auto out = CaptureStdout([&]() { cutl::print_arr(arr, 3); });
    EXPECT_NE(out.find("[1, 2, 3]"), std::string::npos);
}

TEST(PrintTest, PrintVec)
{
    std::vector<int> v = {1, 2, 3};
    auto out = CaptureStdout([&]() { cutl::print_vec(v); });
    EXPECT_NE(out.find("[1, 2, 3]"), std::string::npos);
}

TEST(PrintTest, PrintMap)
{
    std::map<int, int> m = {{1, 10}, {2, 20}};
    auto out = CaptureStdout([&]() { cutl::print_map(m); });
    EXPECT_NE(out.find("1: 10"), std::string::npos);
    EXPECT_NE(out.find("2: 20"), std::string::npos);
}

TEST(PrintTest, PrintSet)
{
    std::set<int> s = {1, 2, 3};
    auto out = CaptureStdout([&]() { cutl::print_set(s); });
    EXPECT_NE(out.find("{1, 2, 3}"), std::string::npos);
}

TEST(PrintTest, PrintClrDefaultAppendsNewline)
{
    auto out = CaptureStdout([&]() { cutl::print_clr("hello", cutl::cli_clr_red); });
    EXPECT_NE(out.find("hello"), std::string::npos);
    ASSERT_FALSE(out.empty());
    EXPECT_EQ(out.back(), '\n');
}

TEST(PrintTest, PrintClrEmptyEndswith)
{
    auto out = CaptureStdout(
        [&]() { cutl::print_clr("hello", cutl::cli_clr_red, cutl::cli_clr_black, ""); });
    EXPECT_NE(out.find("hello"), std::string::npos);
    EXPECT_EQ(out.find('\n'), std::string::npos);
}

TEST(PrintTest, PrintClrNullEndswith)
{
    auto out = CaptureStdout(
        [&]() { cutl::print_clr("hello", cutl::cli_clr_red, cutl::cli_clr_black, nullptr); });
    EXPECT_NE(out.find("hello"), std::string::npos);
    EXPECT_EQ(out.find('\n'), std::string::npos);
}

TEST(PrintTest, PrintClrCustomEndswith)
{
    auto out = CaptureStdout(
        [&]() { cutl::print_clr("hello", cutl::cli_clr_green, cutl::cli_clr_black, " | "); });
    EXPECT_NE(out.find("hello"), std::string::npos);
    EXPECT_NE(out.find(" | "), std::string::npos);
    EXPECT_EQ(out.find('\n'), std::string::npos);
}

TEST(PrintTest, PrintClrExplicitNewline)
{
    auto out = CaptureStdout(
        [&]() { cutl::print_clr("hello", cutl::cli_clr_green, cutl::cli_clr_black, "\n"); });
    EXPECT_NE(out.find("hello"), std::string::npos);
    ASSERT_FALSE(out.empty());
    EXPECT_EQ(out.back(), '\n');
}

TEST(PrintTest, LevelHelpersPrintMessage)
{
    auto out = CaptureStdout([&]() {
        cutl::print_debug("debug-msg");
        cutl::print_info("info-msg");
        cutl::print_warn("warn-msg");
        cutl::print_error("error-msg");
        cutl::print_success("ok-msg");
    });
    EXPECT_NE(out.find("debug-msg"), std::string::npos);
    EXPECT_NE(out.find("info-msg"), std::string::npos);
    EXPECT_NE(out.find("warn-msg"), std::string::npos);
    EXPECT_NE(out.find("error-msg"), std::string::npos);
    EXPECT_NE(out.find("ok-msg"), std::string::npos);
    ASSERT_FALSE(out.empty());
    EXPECT_EQ(out.back(), '\n');
}

TEST(PrintTest, LevelHelpersDefaultAppendNewline)
{
    const auto check_newline = [](const std::string& out) {
        EXPECT_NE(out.find("msg"), std::string::npos);
        ASSERT_FALSE(out.empty());
        EXPECT_EQ(out.back(), '\n');
    };
    check_newline(CaptureStdout([&]() { cutl::print_debug("msg"); }));
    check_newline(CaptureStdout([&]() { cutl::print_info("msg"); }));
    check_newline(CaptureStdout([&]() { cutl::print_warn("msg"); }));
    check_newline(CaptureStdout([&]() { cutl::print_error("msg"); }));
    check_newline(CaptureStdout([&]() { cutl::print_success("msg"); }));
}

TEST(PrintTest, LevelHelpersEmptyEndswith)
{
    const auto check_no_newline = [](const std::string& out) {
        EXPECT_NE(out.find("msg"), std::string::npos);
        EXPECT_EQ(out.find('\n'), std::string::npos);
    };
    check_no_newline(CaptureStdout([&]() { cutl::print_debug("msg", ""); }));
    check_no_newline(CaptureStdout([&]() { cutl::print_info("msg", ""); }));
    check_no_newline(CaptureStdout([&]() { cutl::print_warn("msg", ""); }));
    check_no_newline(CaptureStdout([&]() { cutl::print_error("msg", ""); }));
    check_no_newline(CaptureStdout([&]() { cutl::print_success("msg", ""); }));
}

TEST(PrintTest, LevelHelpersCustomEndswith)
{
    auto out = CaptureStdout([&]() {
        cutl::print_info("foo", "");
        cutl::print_info("bar", " ");
        cutl::print_info("baz", "\n");
    });
    auto foo = out.find("foo");
    auto bar = out.find("bar");
    auto baz = out.find("baz");
    ASSERT_NE(foo, std::string::npos);
    ASSERT_NE(bar, std::string::npos);
    ASSERT_NE(baz, std::string::npos);
    EXPECT_LT(foo, bar);
    EXPECT_LT(bar, baz);
    auto space = out.find(' ', bar);
    ASSERT_NE(space, std::string::npos);
    EXPECT_LT(space, baz);
    EXPECT_EQ(out.find('\n', foo), out.rfind('\n'));
    ASSERT_FALSE(out.empty());
    EXPECT_EQ(out.back(), '\n');
}

TEST(PrintTest, PrintArrStdArray)
{
    std::array<int, 3> arr{{1, 2, 3}};
    auto out = CaptureStdout([&]() { cutl::print_arr(arr); });
    EXPECT_NE(out.find("[1, 2, 3]"), std::string::npos);
}

TEST(PrintTest, PrintUnorderedMap)
{
    std::unordered_map<int, int> m = {{1, 10}, {2, 20}};
    auto out = CaptureStdout([&]() { cutl::print_unordered_map(m); });
    EXPECT_NE(out.find("1: 10"), std::string::npos);
    EXPECT_NE(out.find("2: 20"), std::string::npos);
}

TEST(PrintTest, PrintUnorderedSet)
{
    std::unordered_set<int> s = {1, 2, 3};
    auto out = CaptureStdout([&]() { cutl::print_unordered_set(s); });
    // 顺序不固定，但每个元素和大括号都要出现
    EXPECT_NE(out.find('{'), std::string::npos);
    EXPECT_NE(out.find('}'), std::string::npos);
    EXPECT_NE(out.find('1'), std::string::npos);
    EXPECT_NE(out.find('2'), std::string::npos);
    EXPECT_NE(out.find('3'), std::string::npos);
}

TEST(PrintTest, PrintMatrix)
{
    int matrix[2 * 2] = {1, 2, 3, 4};
    auto out = CaptureStdout([&]() {
        cutl::print_matrix(matrix, 2, 2, "M", 0, 0, 0, 2, 2);
    });
    EXPECT_NE(out.find("M"), std::string::npos);
    EXPECT_NE(out.find("1"), std::string::npos);
    EXPECT_NE(out.find("4"), std::string::npos);
}
