/**
 * @file test_dlloader.cpp
 * @brief Unit tests for cutl::dlloader.
 *
 * 库已修复加载失败时的 dlclose(nullptr) 问题，这里同时覆盖加载成功与失败两条路径。
 */

#include "common_util/dlloader.h"
#include "common_util/sysutil.h"
#include <gtest/gtest.h>

#ifndef _WIN32
#include <sys/stat.h>
#endif

namespace
{

bool file_exists(const std::string& path)
{
#ifndef _WIN32
    struct stat st{};
    return ::stat(path.c_str(), &st) == 0;
#else
    return false;
#endif
}

} // namespace

#ifndef _WIN32
TEST(DlLoaderTest, LoadSystemLibraryAndLookupSymbol)
{
    // 常见 glibc 路径，存在性因发行版而异，先尝试几个再决定是否跳过。
    static const char* kCandidates[] = {
        "/lib/x86_64-linux-gnu/libm.so.6",
        "/usr/lib/x86_64-linux-gnu/libm.so.6",
        "/lib/aarch64-linux-gnu/libm.so.6",
        "/usr/lib/libm.so.6",
    };
    std::string lib_path;
    for (auto* p : kCandidates)
    {
        if (file_exists(p))
        {
            lib_path = p;
            break;
        }
    }
    if (lib_path.empty())
    {
        GTEST_SKIP() << "No suitable system shared library found for dlloader test.";
    }

    cutl::dlloader loader(lib_path);
    auto sym = loader.get_symbol("cos");
    EXPECT_NE(sym, nullptr);
    // 查找一个不存在的符号应返回 nullptr 而非抛异常或崩溃。
    auto missing = loader.get_symbol("__definitely_not_a_symbol__");
    EXPECT_EQ(missing, nullptr);
}

TEST(DlLoaderTest, LoadInvalidPathDoesNotCrashOnDestruct)
{
    // 库修复后：加载失败应安全析构（不会对 nullptr 调用 dlclose）。
    cutl::dlloader loader("/__no_such_dynamic_library__/libfake.so");
    auto sym = loader.get_symbol("anything");
    EXPECT_EQ(sym, nullptr);
}
#endif
