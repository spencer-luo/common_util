/**
 * @file test_filepath.cpp
 * @brief Unit tests for cutl::filepath.
 */

#include "common_util/filepath.h"
#include "common_util/fileutil.h"
#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <vector>
#ifndef _WIN32
#include <unistd.h>
#endif

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

// 期望行为（当前实现尚未满足，故显式禁用）：
// replace_extension() 应当只替换文件名部分的后缀。它目前在整条路径上查找最后一个点，
// 当文件名没有后缀而某级目录名里带点时，会把目录名当成后缀切掉：
//   输入 /home/a.b/README + ".md"
//   期望 /home/a.b/README.md
//   实际 /home/a.md            <- 目录名被吃掉
// 修复方式是改为在 basename() 上定位后缀，再与 dirname() 拼接，与 extension()/stem() 一致。
TEST(FilePathTest, DISABLED_ReplaceExtensionShouldNotTouchDirectoryName)
{
    EXPECT_EQ(cutl::path("/home/a.b/README").replace_extension(".md"), "/home/a.b/README.md");
    EXPECT_EQ(cutl::path("/a/b.c/d").replace_extension(".txt"), "/a/b.c/d.txt");
}

TEST(FilePathTest, Stem)
{
    EXPECT_EQ(cutl::path("/a/b/c.txt").stem(), "c");
    // 没有后缀时，整个文件名就是主干
    EXPECT_EQ(cutl::path("/a/b/c").stem(), "c");
    EXPECT_EQ(cutl::path("/a/b/README").stem(), "README");
    // 多重后缀由 max_dot_number 控制，与 extension() 的参数含义一致
    EXPECT_EQ(cutl::path("/a/b/c.tar.gz").stem(1), "c.tar");
    EXPECT_EQ(cutl::path("/a/b/c.tar.gz").stem(2), "c");
    EXPECT_EQ(cutl::path("/a/b/website.min.js.gz").stem(3), "website");
    // 不带目录的裸文件名
    EXPECT_EQ(cutl::path("c.txt").stem(), "c");
    // 目录名里的点不能被误判为文件后缀
    EXPECT_EQ(cutl::path("/home/a.b/README").stem(), "README");
    EXPECT_EQ(cutl::path("/home/a.b/c.txt").stem(), "c");
    // 末尾分隔符会在构造时被去掉，因此取到的是最后一级目录名
    EXPECT_EQ(cutl::path("/a/b/").stem(), "b");
}

// 以点开头且没有其他点的文件（如 .bashrc），本库的 extension() 会把整个名字视为后缀，
// 因此 stem() 返回空串。这与 std::filesystem::path::stem() 不同，
// 后者会返回 ".bashrc"。此处把本库的取值固定下来，避免无意改动。
TEST(FilePathTest, StemOfDotFile)
{
    EXPECT_EQ(cutl::path("/tmp/.bashrc").extension(), ".bashrc");
    EXPECT_EQ(cutl::path("/tmp/.bashrc").stem(), "");
    // 点开头但另有后缀时，前导点属于主干
    EXPECT_EQ(cutl::path("/tmp/.config.json").stem(), ".config");
    EXPECT_EQ(cutl::path("/tmp/.config.json").extension(), ".json");
}

// stem() 与 extension() 必须严格互补：两者拼接后等于 basename()。
// 这条不变式保证了无论后缀怎么判定，都不会丢字符或多字符。
TEST(FilePathTest, StemPlusExtensionEqualsBasename)
{
    const std::vector<std::string> paths = {
        "/a/b/c.txt",   "/a/b/c",          "/a/b/c.tar.gz", "/a/b/website.min.js.gz",
        "/tmp/.bashrc", "/tmp/.config.json", "c.txt",       "/home/a.b/README",
        "noext",        "/a/b/",
    };

    for (const auto& p : paths)
    {
        auto fp = cutl::path(p);
        for (uint8_t n = 1; n <= 3; ++n)
        {
            EXPECT_EQ(fp.stem(n) + fp.extension(n), fp.basename())
              << "path=" << p << ", max_dot_number=" << static_cast<int>(n);
        }
    }
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
// abspath() 的行为分两种情况，测试也据此分开：
//
// 1. 路径完整存在 —— realpath(3) 成功，结果是完全确定的：符号链接被解析，
//    "//"、"."、".." 都被规范化。这部分由下面的 AbspathOnExistingPath* 用例
//    严格断言。
//
// 2. 路径（的一部分）不存在 —— realpath(3) 返回 nullptr。按 POSIX，此时输出
//    缓冲区的内容是 **未定义** 的；glibc 的实现会在缓冲区里留下"已解析到的最深
//    前缀"。当前 absolute_path() 直接返回该缓冲区，所以返回值 **不是** 完整的
//    输入路径。这是一个已知局限，见 AbspathOnMissingPathIsUnspecified 以及
//    被显式禁用的 DISABLED_AbspathOnMissingPathShouldKeepFullPath。
class FilePathAbspathTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        root_ = "./_ut_abspath_tmp_" + std::to_string(static_cast<long>(::getpid()));
        cleanup();
        ASSERT_TRUE(cutl::createdir(cutl::filepath(root_ + "/a/b"), true));
        // 以规范化后的 root 作为期望值的基准，避免 /tmp 本身是符号链接
        // （如 macOS 的 /tmp -> /private/tmp）导致的平台差异。
        root_abs_ = cutl::path(root_).abspath();
        ASSERT_FALSE(root_abs_.empty());
        ASSERT_EQ(root_abs_.front(), '/');
    }

    void TearDown() override { cleanup(); }

    void cleanup()
    {
        auto dir = cutl::filepath(root_);
        if (dir.exists())
        {
            cutl::removedir(dir, true);
        }
    }

    std::string root_;     // 相对路径形式，如 "./_ut_abspath_tmp_123"
    std::string root_abs_; // 规范化后的绝对路径
};

TEST_F(FilePathAbspathTest, AbspathOnExistingPathIsCanonical)
{
    // 基本情况：已存在的目录
    EXPECT_EQ(cutl::path(root_ + "/a/b").abspath(), root_abs_ + "/a/b");

    // 重复斜杠被折叠
    EXPECT_EQ(cutl::path(root_ + "//a///b").abspath(), root_abs_ + "/a/b");

    // "." 被消化
    EXPECT_EQ(cutl::path(root_ + "/a/./b").abspath(), root_abs_ + "/a/b");

    // ".." 被消化
    EXPECT_EQ(cutl::path(root_ + "/a/b/..").abspath(), root_abs_ + "/a");
    EXPECT_EQ(cutl::path(root_ + "/a/../a/b").abspath(), root_abs_ + "/a/b");

    // 相对路径转成绝对路径
    auto rel = cutl::path(root_ + "/a").abspath();
    EXPECT_EQ(rel.front(), '/');
    EXPECT_EQ(rel, root_abs_ + "/a");

    // 越过根目录的 ".." 被丢弃（"/" 的父目录仍是 "/"）
    EXPECT_EQ(cutl::path("/../../etc").abspath(), "/etc");
}

TEST_F(FilePathAbspathTest, AbspathOnExistingPathResolvesSymlink)
{
    const auto target = cutl::path(root_ + "/a/b").abspath();
    const auto link = cutl::filepath(root_ + "/link");
    if (!cutl::createlink(cutl::filepath(target), link))
    {
        GTEST_SKIP() << "Symlink creation not permitted in this environment.";
    }

    // 通过链接访问时，abspath() 应当返回链接指向的真实路径
    EXPECT_EQ(cutl::path(root_ + "/link").abspath(), target);

    // 关键语义：".." 是在符号链接解析之后才应用的，
    // 因此 link/.. 等于 target 的父目录，而不是 link 所在的目录。
    EXPECT_EQ(cutl::path(root_ + "/link/..").abspath(), root_abs_ + "/a");
}

// 已知局限：路径不存在时 realpath(3) 失败，输出缓冲区内容按 POSIX 未定义，
// 当前实现会把它原样返回。这里只断言可移植的不变量：不抛异常、不崩溃，
// 且返回值要么为空、要么是一个绝对路径。
// 不断言具体取值，避免把 glibc 的实现细节固化成测试期望。
TEST_F(FilePathAbspathTest, AbspathOnMissingPathIsUnspecified)
{
    const std::vector<std::string> missing = {
        root_ + "/__no_such__/deep/file.txt",
        root_ + "/a/__no_such__//x/../y",
        "/__cutl_no_such_root__/a/./b/../c",
        "__cutl_no_such_relative__/a/b",
    };
    for (const auto& in : missing)
    {
        std::string abs;
        ASSERT_NO_THROW({ abs = cutl::path(in).abspath(); }) << "input=" << in;
        if (!abs.empty())
        {
            EXPECT_EQ(abs.front(), '/') << "input=" << in << ", abs=" << abs;
        }
    }
}

// 期望行为（当前实现尚未满足，故显式禁用）：
// 路径不存在时也应返回完整的绝对路径，而不是被截断的前缀。
// 这几条断言直接来自实际线上日志：
//   输入 /home/x/ws/ota_engine//stark/module/ota_engine/bin/ota_engine
//   期望 /home/x/ws/ota_engine/stark/module/ota_engine/bin/ota_engine
//   实际 /home/x/ws/ota_engine                       <- 被截断
// 若将来把 absolute_path() 改为 weakly_canonical 语义（用 realpath 解析最长的
// 存在前缀，剩余部分再按词法拼接），去掉 DISABLED_ 前缀即可启用本用例。
TEST_F(FilePathAbspathTest, DISABLED_AbspathOnMissingPathShouldKeepFullPath)
{
    // 尾部文件名必须保留，重复斜杠应被折叠
    EXPECT_EQ(cutl::path(root_ + "/a//__no_such__/bin/app").abspath(),
              root_abs_ + "/a/__no_such__/bin/app");

    // 中间含 ".." 且尾部不存在
    EXPECT_EQ(cutl::path(root_ + "/a/b/../__no_such__/conf/app.yaml").abspath(),
              root_abs_ + "/a/__no_such__/conf/app.yaml");

    // 完全不存在的绝对路径也应完成词法规范化
    EXPECT_EQ(cutl::path("/__cutl_no_such_root__/a/./b/../c").abspath(),
              "/__cutl_no_such_root__/a/c");

    // 不存在的相对路径应补上 cwd 前缀且保留完整尾部
    auto abs = cutl::path("__cutl_no_such_relative__/a/b").abspath();
    EXPECT_EQ(abs.find("//"), std::string::npos) << "abs=" << abs;
    EXPECT_NE(abs.find("__cutl_no_such_relative__/a/b"), std::string::npos) << "abs=" << abs;
}
#endif
