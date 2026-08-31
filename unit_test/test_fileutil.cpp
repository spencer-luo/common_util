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
#ifndef _WIN32
#include <unistd.h>
#else
#include <process.h>
#endif

namespace
{

// 用 testcase 名 + pid 拼出唯一目录，确保并行 ctest 时不互相干扰。
std::string MakeUniqueTestRoot(const std::string& test_name)
{
#ifndef _WIN32
    auto pid = static_cast<long>(::getpid());
#else
    auto pid = static_cast<long>(::_getpid());
#endif
    return std::string("./_unit_test_fileutil_tmp_") + test_name + "_" + std::to_string(pid);
}

class FileUtilTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        const auto* info = ::testing::UnitTest::GetInstance()->current_test_info();
        std::string name = info ? info->name() : std::string("anonymous");
        root_ = cutl::path(MakeUniqueTestRoot(name));
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

// ---------------------------------------------------------------------------
// fsync / fsyncdir
//
// 真正的掉电持久性无法在单元测试里验证，所以这里断言的是可观测的契约：
// 返回值是否正确、用户态缓冲是否被一并写出、非法入参是否被拒绝。
// ---------------------------------------------------------------------------

// 成功时必须返回 true。曾经的实现写成 file_sync(handle) == 0，
// 而 file_sync() 成功时返回 true，于是返回值恰好反了：成功报错、失败报成功。
TEST_F(FileUtilTest, FsyncHandleReturnsTrueOnSuccess)
{
    auto file = root_.join("sync.txt");
    auto* fd = std::fopen(file.str().c_str(), "w");
    ASSERT_NE(fd, nullptr);
    cutl::file_guard guard(fd);

    std::fputs("hello", fd);
    EXPECT_TRUE(cutl::fsync(fd));
}

// fsync 之前必须先 fflush，否则还留在 FILE 用户态缓冲里的数据不会被落盘。
TEST_F(FileUtilTest, FsyncHandleFlushesUserspaceBuffer)
{
    auto file = root_.join("buffered.txt");
    auto* fd = std::fopen(file.str().c_str(), "w");
    ASSERT_NE(fd, nullptr);
    cutl::file_guard guard(fd);

    // 只写入 FILE 缓冲，不主动 fflush
    std::fputs("buffered content", fd);
    ASSERT_TRUE(cutl::fsync(fd));

    // 此时通过另一个句柄读取应当已经能看到全部内容
    EXPECT_EQ(cutl::readtext(file), "buffered content");
}

TEST_F(FileUtilTest, FsyncHandleRejectsNullptr)
{
    EXPECT_FALSE(cutl::fsync(static_cast<FILE*>(nullptr)));
}

// fsync(路径) 在文件内容之外，还会把父目录的目录项落盘。
TEST_F(FileUtilTest, FsyncPathSyncsFileAndDirectoryEntry)
{
    auto file = root_.join("durable.txt");
    ASSERT_TRUE(cutl::writetext(file, "durable"));

    EXPECT_TRUE(cutl::fsync(file));
    // 落盘不应改变文件内容
    EXPECT_EQ(cutl::readtext(file), "durable");
}

TEST_F(FileUtilTest, FsyncPathRejectsMissingFile)
{
    EXPECT_FALSE(cutl::fsync(root_.join("__no_such_file__")));
}

TEST_F(FileUtilTest, FsyncDirOnDirectory)
{
    EXPECT_TRUE(cutl::fsyncdir(root_));

    auto sub = root_.join("subdir");
    ASSERT_TRUE(cutl::createdir(sub));
    EXPECT_TRUE(cutl::fsyncdir(sub));
}

// 传文件或不存在的路径都必须被拒绝，否则会静默地"同步了别的东西"。
TEST_F(FileUtilTest, FsyncDirRejectsNonDirectory)
{
    auto file = root_.join("plain.txt");
    ASSERT_TRUE(cutl::writetext(file, "x"));
    EXPECT_FALSE(cutl::fsyncdir(file));

    EXPECT_FALSE(cutl::fsyncdir(root_.join("__no_such_dir__")));
}

// createfile/writetext/copyfile 在新建目录项后会顺带落盘父目录。
// 掉电持久性无法在单元测试里验证，因此这里覆盖的是这段新增逻辑涉及的各条分支，
// 确认它们没有把原本可用的写入路径改坏。

// 首次写入会新建文件(需要落盘父目录)，再次写入是覆盖(不需要)，两条分支都必须正常。
TEST_F(FileUtilTest, WriteTextCreateThenOverwrite)
{
    auto file = root_.join("rewrite.txt");
    ASSERT_FALSE(file.exists());

    EXPECT_TRUE(cutl::writetext(file, "first"));
    EXPECT_EQ(cutl::readtext(file), "first");

    // 此时文件已存在，走的是覆盖分支
    EXPECT_TRUE(cutl::writetext(file, "second"));
    EXPECT_EQ(cutl::readtext(file), "second");

    // 覆盖成更短的内容，验证截断也正常
    EXPECT_TRUE(cutl::writetext(file, "x"));
    EXPECT_EQ(cutl::readtext(file), "x");
}

// 不带任何目录分隔符的相对路径，父目录退化为当前目录，落盘同样要成功。
TEST_F(FileUtilTest, WriteTextToBareFilenameInCwd)
{
#ifndef _WIN32
    auto name = "_ut_bare_" + std::to_string(static_cast<long>(::getpid())) + ".txt";
#else
    auto name = "_ut_bare_" + std::to_string(static_cast<long>(::_getpid())) + ".txt";
#endif
    auto file = cutl::path(name);
    ASSERT_TRUE(cutl::writetext(file, "bare"));
    EXPECT_EQ(cutl::readtext(file), "bare");
    EXPECT_TRUE(cutl::removefile(file));
}

// 目标已存在时 copyfile 会先删旧的再建新的，两次都动了父目录。
TEST_F(FileUtilTest, CopyFileOverwritesExistingDestination)
{
    auto src = root_.join("src.txt");
    auto dst = root_.join("dst.txt");
    ASSERT_TRUE(cutl::writetext(src, "new content"));
    ASSERT_TRUE(cutl::writetext(dst, "old content that is much longer"));

    EXPECT_TRUE(cutl::copyfile(src, dst));
    EXPECT_EQ(cutl::readtext(dst), "new content");
}

#ifndef _WIN32
// 复制符号链接走的是另一条分支：它没有独立的数据内容，
// 落盘父目录是唯一能让这个链接持久化的手段。
TEST_F(FileUtilTest, CopySymlinkSyncsDirectoryEntry)
{
    auto target = root_.join("sym_target.txt").abspath();
    ASSERT_TRUE(cutl::writetext(cutl::filepath(target), "target data"));

    auto link = root_.join("sym_link.txt");
    if (!cutl::createlink(cutl::filepath(target), link))
    {
        GTEST_SKIP() << "Symlink creation not permitted in this environment.";
    }

    auto copied = root_.join("sym_copy.txt");
    EXPECT_TRUE(cutl::copyfile(link, copied));
    EXPECT_TRUE(copied.issymlink());
    EXPECT_EQ(cutl::readtext(copied), "target data");
}
#endif

// 新建文件后先 fsync 文件、再 fsync 父目录，是保证"文件确实存在"的标准做法。
// 这里验证整套调用链能跑通且文件内容完整。
TEST_F(FileUtilTest, FsyncFileThenParentDirectory)
{
    auto file = root_.join("created.txt");
    auto* fd = std::fopen(file.str().c_str(), "w");
    ASSERT_NE(fd, nullptr);
    {
        cutl::file_guard guard(fd);
        std::fputs("content", fd);
        ASSERT_TRUE(cutl::fsync(fd));
    }
    ASSERT_TRUE(cutl::fsyncdir(root_));

    EXPECT_TRUE(file.exists());
    EXPECT_EQ(cutl::readtext(file), "content");
}
