/**
 * @file test_filepath.cpp
 * @brief Unit tests for cutl::filepath.
 */

#include "common_util/filepath.h"
#include <gtest/gtest.h>

TEST(FilePathTest, ConstructAndStr)
{
    cutl::filepath p("/tmp/test.txt");
    EXPECT_EQ(p.str(), "/tmp/test.txt");
    // path() 工厂函数等价于构造函数
    auto p2 = cutl::path("/tmp/test.txt");
    EXPECT_EQ(p2.str(), "/tmp/test.txt");
}

TEST(FilePathTest, TrailingSeparatorRemoved)
{
    auto p = cutl::path("/tmp/test/");
    EXPECT_EQ(p.str(), "/tmp/test");
}

TEST(FilePathTest, JoinUsesSeparator)
{
    auto base = cutl::path("/tmp/foo");
    auto joined = base.join("bar.txt");
    char sep = cutl::filepath::separator();
    std::string expected = std::string("/tmp/foo") + sep + "bar.txt";
    EXPECT_EQ(joined.str(), expected);
}

TEST(FilePathTest, DirnameBasename)
{
    auto p = cutl::path("/tmp/foo/bar.txt");
    EXPECT_EQ(p.dirname(), "/tmp/foo");
    EXPECT_EQ(p.basename(), "bar.txt");

    auto p2 = cutl::path("bar.txt");
    EXPECT_EQ(p2.dirname(), "");
    EXPECT_EQ(p2.basename(), "bar.txt");
}

TEST(FilePathTest, Extension)
{
    EXPECT_EQ(cutl::path("/a/b/c.txt").extension(), ".txt");
    EXPECT_EQ(cutl::path("/a/b/c").extension(), "");
    EXPECT_EQ(cutl::path("/a/b/c.tar.gz").extension(1), ".gz");
    EXPECT_EQ(cutl::path("/a/b/c.tar.gz").extension(2), ".tar.gz");
    EXPECT_EQ(cutl::path("/a/b/website.min.js.gz").extension(3), ".min.js.gz");
}

TEST(FilePathTest, ReplaceExtension)
{
    auto p = cutl::path("/tmp/data.tar.gz");
    EXPECT_EQ(p.replace_extension(".zip", 2), "/tmp/data.zip");
    EXPECT_EQ(p.replace_extension(".bak", 1), "/tmp/data.tar.bak");
}

TEST(FilePathTest, ExistsAndType)
{
    // /tmp 目录绝大多数 *nix 平台都存在
#ifndef _WIN32
    auto tmp = cutl::path("/tmp");
    EXPECT_TRUE(tmp.exists());
    EXPECT_TRUE(tmp.isdir());
    EXPECT_FALSE(tmp.isfile());
#endif
    auto bogus = cutl::path("/__no_such_path_for_test__/__no__");
    EXPECT_FALSE(bogus.exists());
}

TEST(FilePathTest, OutputStreamOperator)
{
    auto p = cutl::path("/a/b");
    std::ostringstream oss;
    oss << p;
    EXPECT_EQ(oss.str(), "/a/b");
}
