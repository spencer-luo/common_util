/**
 * @file test_sysutil.cpp
 * @brief Unit tests for cutl::sysutil (platform / arch / endian / system / getenv).
 */

#include "common_util/sysutil.h"
#include <cstdlib>
#include <gtest/gtest.h>
#include <string>

TEST(SysUtilTest, PlatformInformation)
{
    auto type = cutl::platform_type();
    auto name = cutl::platform_name(type);
    EXPECT_FALSE(name.empty());

    EXPECT_EQ(cutl::platform_name(cutl::os_platform::os_windows), "Windows");
    EXPECT_EQ(cutl::platform_name(cutl::os_platform::os_macos), "macOS");
    EXPECT_EQ(cutl::platform_name(cutl::os_platform::os_linux), "Linux");
    EXPECT_EQ(cutl::platform_name(cutl::os_platform::os_unknown), "Unknown");
}

TEST(SysUtilTest, ProgramBitAndStl)
{
    auto bit = cutl::program_bit();
    EXPECT_TRUE(bit == 32 || bit == 64);
    auto stl = cutl::cpp_stl_version();
    EXPECT_FALSE(stl.empty());
}

TEST(SysUtilTest, EndianAndByteSwap)
{
    auto e = cutl::endian_type();
    EXPECT_TRUE(e == cutl::endian::little || e == cutl::endian::big);

    EXPECT_EQ(cutl::byteswap(static_cast<uint16_t>(0x1234)), 0x3412);
    EXPECT_EQ(cutl::byteswap(static_cast<uint32_t>(0x12345678)), 0x78563412u);
    EXPECT_EQ(cutl::byteswap(static_cast<uint64_t>(0x0102030405060708ULL)),
              0x0807060504030201ULL);

    uint8_t buf[4] = {0x01, 0x02, 0x03, 0x04};
    cutl::byteswap(buf, 4);
    EXPECT_EQ(buf[0], 0x04);
    EXPECT_EQ(buf[3], 0x01);
}

TEST(SysUtilTest, SystemCommandSuccessAndFailure)
{
#ifndef _WIN32
    // 简单的 echo 命令应当成功返回
    EXPECT_TRUE(cutl::system("echo hello > /dev/null"));
    EXPECT_EQ(cutl::system2("echo hello > /dev/null"), 0);
#else
    EXPECT_TRUE(cutl::system("echo hello > NUL"));
#endif
}

TEST(SysUtilTest, CallcmdReturnsOutput)
{
    std::string out;
#ifndef _WIN32
    bool ok = cutl::callcmd("echo common_util_test", out);
    EXPECT_TRUE(ok);
    EXPECT_NE(out.find("common_util_test"), std::string::npos);
#else
    bool ok = cutl::callcmd("cmd /c echo common_util_test", out);
    EXPECT_TRUE(ok);
    EXPECT_NE(out.find("common_util_test"), std::string::npos);
#endif
}

TEST(SysUtilTest, GetEnvDefaults)
{
    auto v = cutl::getenv("CUTL_NO_SUCH_VAR_HOPEFULLY", "fallback");
    EXPECT_EQ(v, "fallback");

#ifndef _WIN32
    // PATH 在所有 *nix 平台都设置
    auto path = cutl::getenv("PATH", "");
    EXPECT_FALSE(path.empty());
#endif
}

TEST(SysUtilTest, CwdAndHomedir)
{
    auto cwd = cutl::getcwd();
    auto home = cutl::homedir();
    EXPECT_FALSE(cwd.empty());
    EXPECT_FALSE(home.empty());
}

TEST(SysUtilTest, ArchitectureNonEmpty)
{
    auto arch = cutl::architecture();
    EXPECT_FALSE(arch.empty());
}
