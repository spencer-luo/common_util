/**
 * @file test_verutil.cpp
 * @brief Unit tests for cutl::verutil (is_version, get_version, compare_version).
 */

#include "common_util/verutil.h"
#include <gtest/gtest.h>

TEST(VerUtilTest, IsVersion)
{
    // 当前实现要求至少包含 major.minor.patch（最后一段 1-3 位数字）
    EXPECT_TRUE(cutl::is_version("1.2.3"));
    EXPECT_TRUE(cutl::is_version("12.34.567"));
    EXPECT_TRUE(cutl::is_version("3.28.3"));
    EXPECT_FALSE(cutl::is_version("v3.2.2"));
    EXPECT_FALSE(cutl::is_version("3"));
    EXPECT_FALSE(cutl::is_version("abc.def.ghi"));
    EXPECT_FALSE(cutl::is_version(""));
}

TEST(VerUtilTest, GetVersion)
{
    EXPECT_EQ(cutl::get_version("cmake version 3.28.3"), "3.28.3");
    EXPECT_EQ(cutl::get_version("Release v1.0.10 build 2024"), "1.0.10");
    EXPECT_EQ(cutl::get_version("no version here"), "");
}

TEST(VerUtilTest, CompareVersion)
{
    EXPECT_EQ(cutl::compare_version("1.0.0", "1.0.0"), 0);
    EXPECT_EQ(cutl::compare_version("1.0.1", "1.0.0"), 1);
    EXPECT_EQ(cutl::compare_version("1.0.0", "1.0.1"), -1);
    EXPECT_EQ(cutl::compare_version("3.28.3", "3.2.2"), 1);
    EXPECT_EQ(cutl::compare_version("3.28.3", "3.30.0"), -1);
    // 缺省段视为 0：1.0 == 1.0.0
    EXPECT_EQ(cutl::compare_version("1.0", "1.0.0"), 0);
    EXPECT_EQ(cutl::compare_version("1.0", "1.0.1"), -1);
}
