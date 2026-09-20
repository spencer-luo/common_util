/**
 * @file test_config.cpp
 * @brief Unit tests for cutl library_name / library_version / library_init.
 */

#include "common_util/config.h"
#include "common_util/version.h"
#include <gtest/gtest.h>
#include <regex>
#include <string>

TEST(ConfigTest, LibraryNameMatchesMacro)
{
    auto name = cutl::library_name();
    EXPECT_FALSE(name.empty());
    EXPECT_EQ(name, std::string(CUTL_NAME));
}

TEST(ConfigTest, LibraryVersionLooksLikeSemver)
{
    auto version = cutl::library_version();
    EXPECT_FALSE(version.empty());
    EXPECT_EQ(version, std::string(CUTL_VERSION));
    std::regex pattern(R"(^\d+\.\d+\.\d+$)");
    EXPECT_TRUE(std::regex_match(version, pattern)) << "version=" << version;
}

TEST(ConfigTest, LibraryInitCanBeCalledRepeatedly)
{
    // 输出函数只发布一次（test_main 已完成首次初始化），后续调用只更新级别、不替换回调。
    EXPECT_NO_THROW(cutl::library_init([](cutl::loglevel, const std::string&) {}));
    EXPECT_NO_THROW(cutl::library_init(nullptr, cutl::loglevel::info_level));
}
