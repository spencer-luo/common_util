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

TEST(ConfigTest, LibraryInitWithCustomLogger)
{
    int call_count = 0;
    cutl::library_init([&call_count](cutl::loglevel, const std::string&) { ++call_count; });
    // library_init 内部会调用 CUTL_INFO 一次，因此回调至少被触发一次
    EXPECT_GE(call_count, 1);
    // 重新设置成空回调，避免影响后续用例
    cutl::library_init([](cutl::loglevel, const std::string&) {});
}
