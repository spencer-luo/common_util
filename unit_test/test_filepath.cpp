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

TEST(FilePathTest, CopyConstructAndAssign)
{
    cutl::filepath original("/tmp/copy_src.txt");
    // 拷贝构造
    cutl::filepath copy(original);
    EXPECT_EQ(copy.str(), original.str());

    // 拷贝赋值（自赋值也不应破坏内部状态）
    cutl::filepath assigned("/other/path");
    assigned = original;
    EXPECT_EQ(assigned.str(), original.str());
    assigned = assigned;
    EXPECT_EQ(assigned.str(), original.str());
}

TEST(FilePathTest, TypeReturnsExpectedFiletype)
{
    // 对一个不存在的路径，type() 应当返回 ft_unknown
    auto bogus = cutl::path("/__cutl_no_such_path__/__no__");
    EXPECT_EQ(bogus.type(), cutl::filetype::ft_unknown);

#ifndef _WIN32
    // /tmp 在 *nix 平台应当是目录
    auto tmp = cutl::path("/tmp");
    EXPECT_EQ(tmp.type(), cutl::filetype::ft_directory);
#endif
}

TEST(FilePathTest, PermissionsOnTmp)
{
    // 不存在的路径，权限查询都应为 false
    auto bogus = cutl::path("/__cutl_no_such_path__/__no__");
    EXPECT_FALSE(bogus.readable());
    EXPECT_FALSE(bogus.writable());
    EXPECT_FALSE(bogus.executable());

#ifndef _WIN32
    // /tmp 在 *nix 平台应当可读、可写、可执行（进入目录）
    auto tmp = cutl::path("/tmp");
    EXPECT_TRUE(tmp.readable());
    EXPECT_TRUE(tmp.writable());
    EXPECT_TRUE(tmp.executable());
#endif
}

#ifndef _WIN32
// 回归测试：abspath() 在路径不存在时不再返回 realpath 残留的"半截前缀"，
// 而是退化到纯词法规范化。
TEST(FilePathTest, AbspathOnMissingPathFallsBackToLexical)
{
    // 输入路径里包含 "//"、不存在的中间段，以及 "."、".." 等需要规范化的元素。
    const std::string in =
      "/home/__cutl_no_such_user__/robot_workspace/ota_engine//stark/module/"
      "ota_engine/bin/ota_engine";
    auto abs = cutl::path(in).abspath();

    // 不应被截断为 ".../stark"（旧 bug 的现象）
    EXPECT_NE(abs, "/home/__cutl_no_such_user__/robot_workspace/ota_engine/stark");
    // 重复斜杠应被折叠
    EXPECT_EQ(abs.find("//"), std::string::npos) << "abs=" << abs;
    // 末尾的真实文件名必须保留
    EXPECT_NE(abs.find("/bin/ota_engine"), std::string::npos) << "abs=" << abs;
}

TEST(FilePathTest, AbspathLexicalNormalizesDotAndDotDot)
{
    // /a/./b/../c -> /a/c（中间段都不存在的情况下也能正确规范化）
    auto abs1 = cutl::path("/__cutl_no_such_root__/a/./b/../c").abspath();
    EXPECT_EQ(abs1, "/__cutl_no_such_root__/a/c");

    // 越过根的 .. 应被丢弃
    auto abs2 = cutl::path("/../../etc").abspath();
    EXPECT_EQ(abs2, "/etc");

    // 重复斜杠折叠
    auto abs3 = cutl::path("/__cutl_no_such_root__//x///y").abspath();
    EXPECT_EQ(abs3, "/__cutl_no_such_root__/x/y");
}

// 回归：来自实际日志的两条用例。共同特点是中间存在 ".."，旧实现会把
// realpath buffer 里残留的"最长存在前缀"返回（如 .../conf 而不是 .../conf/app.yaml）。
TEST(FilePathTest, AbspathParentRefWithMissingTailKeepsFilename)
{
    auto a = cutl::path("/home/__cutl_no_user__/ws/ota_engine/../conf/app.yaml").abspath();
    EXPECT_EQ(a, "/home/__cutl_no_user__/ws/conf/app.yaml");

    auto b = cutl::path("/home/__cutl_no_user__/ws/ota_engine/../conf/busi.yaml").abspath();
    EXPECT_EQ(b, "/home/__cutl_no_user__/ws/conf/busi.yaml");
}

TEST(FilePathTest, AbspathRelativeMissingPathBecomesAbsolute)
{
    // 相对路径且不存在：应当带上 cwd 前缀，并且不会再有 "//"
    auto abs = cutl::path("__cutl_no_such_relative__/a/b").abspath();
    EXPECT_FALSE(abs.empty());
    EXPECT_EQ(abs.front(), '/');
    EXPECT_EQ(abs.find("//"), std::string::npos) << "abs=" << abs;
    EXPECT_NE(abs.find("__cutl_no_such_relative__/a/b"), std::string::npos)
      << "abs=" << abs;
}
#endif
