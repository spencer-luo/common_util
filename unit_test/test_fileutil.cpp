/**
 * @file test_fileutil.cpp
 * @brief Unit tests for cutl::fileutil (create / remove / read / write / list / find / copy).
 *
 * 测试通过 testing::Environment 在 SetUp 时统一创建临时目录、TearDown 时清理，
 * 保证每个用例内部不残留磁盘状态。
 */

#include "common_util/fileutil.h"
#include "common_util/filepath.h"
#include "common_util/sysutil.h"
#include "common_util/timeutil.h"
#include <chrono>
#include <gtest/gtest.h>
#include <string>
#include <thread>

namespace
{

// 与 ctest 工作目录区分开，避免污染源码目录。
std::string MakeTestRoot()
{
    return "./_unit_test_fileutil_tmp";
}

class FileUtilTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        root_ = cutl::path(MakeTestRoot());
        // 每个用例都从一个干净的目录开始
        cutl::removedir(root_, true);
        ASSERT_TRUE(cutl::createdir(root_));
    }

    void TearDown() override { cutl::removedir(root_, true); }

    cutl::filepath root_{"."};
};

} // namespace

TEST_F(FileUtilTest, CreateAndRemoveFile)
{
    auto file = root_.join("a.txt");
    EXPECT_TRUE(cutl::createfile(file));
    EXPECT_TRUE(file.exists());
    EXPECT_TRUE(file.isfile());
    EXPECT_TRUE(cutl::removefile(file));
    EXPECT_FALSE(file.exists());
}

TEST_F(FileUtilTest, CreateAndRemoveDirRecursive)
{
    auto deep = root_.join("level1/level2/level3");
    EXPECT_TRUE(cutl::createdir(deep, true));
    EXPECT_TRUE(deep.exists());
    EXPECT_TRUE(deep.isdir());

    auto level1 = root_.join("level1");
    EXPECT_TRUE(cutl::removedir(level1, true));
    EXPECT_FALSE(level1.exists());
}

TEST_F(FileUtilTest, ReadWriteText)
{
    auto file = root_.join("hello.txt");
    const std::string content = "Hello, common_util!\nLine2";
    EXPECT_TRUE(cutl::writetext(file, content));
    EXPECT_TRUE(file.exists());

    auto loaded = cutl::readtext(file);
    EXPECT_EQ(loaded, content);

    // 文件大小与内容长度一致
    EXPECT_EQ(cutl::filesize(file), content.size());
}

TEST_F(FileUtilTest, FileTypeFlag)
{
    // 当前实现 ft_file => "-", ft_directory => "d", ft_symlink => "l", ft_unknown => "u"
    EXPECT_EQ(cutl::filetype_flag(cutl::filetype::ft_file), "-");
    EXPECT_EQ(cutl::filetype_flag(cutl::filetype::ft_directory), "d");
    EXPECT_EQ(cutl::filetype_flag(cutl::filetype::ft_symlink), "l");
    EXPECT_EQ(cutl::filetype_flag(cutl::filetype::ft_unknown), "u");
}

TEST_F(FileUtilTest, ListFilesAndFind)
{
    auto sub = root_.join("sub");
    ASSERT_TRUE(cutl::createdir(sub));
    ASSERT_TRUE(cutl::createfile(root_.join("file1.txt")));
    ASSERT_TRUE(cutl::createfile(root_.join("file2.data")));
    ASSERT_TRUE(cutl::createfile(sub.join("file3.txt")));

    // 非递归只看到一级
    auto top_level = cutl::list_files(root_, cutl::filetype::ft_all, false);
    EXPECT_GE(top_level.size(), 3u); // sub + file1.txt + file2.data

    // 递归包含子目录文件
    auto all = cutl::list_files(root_, cutl::filetype::ft_all, true);
    EXPECT_GE(all.size(), 4u);

    // 仅文件
    auto only_files = cutl::list_files(root_, cutl::filetype::ft_file, true);
    EXPECT_EQ(only_files.size(), 3u);

    // 按子串查找
    auto found = cutl::find_files(root_, "file", true);
    EXPECT_EQ(found.size(), 3u);

    // 按扩展名查找
    auto by_ext = cutl::find_files_by_extension(root_, ".txt", true);
    EXPECT_EQ(by_ext.size(), 2u);
    auto by_data = cutl::find_files_by_extension(root_, ".data", true);
    EXPECT_EQ(by_data.size(), 1u);
}

TEST_F(FileUtilTest, CopyFileAndDir)
{
    auto src = root_.join("src.txt");
    auto dst = root_.join("dst.txt");
    ASSERT_TRUE(cutl::writetext(src, "abc"));
    EXPECT_TRUE(cutl::copyfile(src, dst));
    EXPECT_TRUE(dst.exists());
    EXPECT_EQ(cutl::readtext(dst), "abc");

    // 复制目录
    auto src_dir = root_.join("dir1");
    ASSERT_TRUE(cutl::createdir(src_dir));
    ASSERT_TRUE(cutl::writetext(src_dir.join("a.txt"), "1"));
    auto dst_dir = root_.join("dir1_copy");
    EXPECT_TRUE(cutl::copydir(src_dir, dst_dir));
    EXPECT_TRUE(dst_dir.exists());
    EXPECT_EQ(cutl::readtext(dst_dir.join("a.txt")), "1");
}

TEST_F(FileUtilTest, RenameFile)
{
    auto src = root_.join("old.txt");
    auto dst = root_.join("new.txt");
    ASSERT_TRUE(cutl::writetext(src, "x"));
    EXPECT_TRUE(cutl::renamefile(src, dst));
    EXPECT_FALSE(src.exists());
    EXPECT_TRUE(dst.exists());
    EXPECT_EQ(cutl::readtext(dst), "x");
}

TEST_F(FileUtilTest, LastModifiedTimeIsRecent)
{
    auto file = root_.join("mtime.txt");
    ASSERT_TRUE(cutl::writetext(file, "data"));
    auto now = cutl::timestamp(cutl::timeunit::s);
    auto mtime = cutl::last_modified_time(file);
    // 时钟差异小于 60 秒
    EXPECT_LE(mtime, now + 5);
    EXPECT_GE(mtime + 60, now);
}

TEST_F(FileUtilTest, DirSizeAccumulates)
{
    auto a = root_.join("a.bin");
    auto b = root_.join("b.bin");
    ASSERT_TRUE(cutl::writetext(a, std::string(100, 'a')));
    ASSERT_TRUE(cutl::writetext(b, std::string(50, 'b')));
    auto total = cutl::dirsize(root_);
    EXPECT_GE(total, 150u);
}

TEST_F(FileUtilTest, FlagFileLifecycle)
{
    auto flag_path = root_.join("__flag__");
    {
        cutl::flagfile guard(flag_path);
        EXPECT_TRUE(guard.exists());
    }
    // 离开作用域后 flag 文件应被自动删除
    EXPECT_FALSE(flag_path.exists());

    // 显式 remove
    cutl::flagfile guard2(flag_path);
    EXPECT_TRUE(guard2.exists());
    EXPECT_TRUE(guard2.remove());
    EXPECT_FALSE(flag_path.exists());
}

TEST_F(FileUtilTest, FileGuardClosesOnScopeExit)
{
    auto file = root_.join("guard.txt");
    auto fd = std::fopen(file.str().c_str(), "w");
    ASSERT_NE(fd, nullptr);
    {
        cutl::file_guard guard(fd);
        EXPECT_EQ(guard.getfd(), fd);
        std::fputs("hello", fd);
    }
    // file_guard 析构后再次打开读取应该可以获得内容
    EXPECT_EQ(cutl::readtext(file), "hello");
}

#ifndef _WIN32
TEST_F(FileUtilTest, SymlinkAndRealpath)
{
    auto target = root_.join("target.txt").abspath();
    ASSERT_TRUE(cutl::writetext(cutl::filepath(target), "link target"));

    auto link = root_.join("link.txt");
    if (!cutl::createlink(cutl::filepath(target), link))
    {
        GTEST_SKIP() << "Symlink creation not permitted in this environment.";
    }
    EXPECT_TRUE(link.issymlink());
    EXPECT_EQ(link.realpath(), target);
    // 通过链接读取应得到目标文件内容
    EXPECT_EQ(cutl::readtext(link), "link target");
}
#endif
