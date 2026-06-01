/**
 * @file test_print.cpp
 * @brief Unit tests for cutl::print_* helpers (capture stdout for content check).
 */

#include "common_util/print.h"
#include <gtest/gtest.h>
#include <map>
#include <set>
#include <sstream>
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

TEST(PrintTest, PrintColoredText)
{
    auto out = CaptureStdout([&]() { cutl::print_clr("hello", cutl::cli_clr_red); });
    EXPECT_NE(out.find("hello"), std::string::npos);
}

TEST(PrintTest, LevelHelpersPrintMessage)
{
    auto out = CaptureStdout([&]() {
        cutl::print_info("info-msg");
        cutl::print_warn("warn-msg");
        cutl::print_success("ok-msg");
    });
    EXPECT_NE(out.find("info-msg"), std::string::npos);
    EXPECT_NE(out.find("warn-msg"), std::string::npos);
    EXPECT_NE(out.find("ok-msg"), std::string::npos);
}
