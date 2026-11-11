/**
 * @file test_sysutil.cpp
 * @brief Unit tests for cutl::sysutil (platform / arch / endian / system / callcmd / getenv).
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
 
 namespace
 {
 
 // Windows 的 pipline_open 会再包一层 cmd.exe /c，这里只写各自 shell 的命令本体。
 std::string callcmd_of(const std::string& unix_cmd, const std::string& windows_cmd)
 {
 #ifndef _WIN32
     (void)windows_cmd;
     return unix_cmd;
 #else
     (void)unix_cmd;
     return windows_cmd;
 #endif
 }
 
 } // namespace
 
 TEST(SysUtilTest, CallcmdReturnsStrippedStdout)
 {
     std::string out = "stale";
     const bool ok = cutl::callcmd(callcmd_of("printf 'common_util_test\\n'", "echo common_util_test"), out);
     EXPECT_TRUE(ok);
     EXPECT_EQ(out, "common_util_test");
 }
 
 TEST(SysUtilTest, CallcmdStripsSurroundingWhitespace)
 {
     std::string out = "stale";
     const bool ok = cutl::callcmd(
       callcmd_of("printf ' \\t hello world \\n'", "echo hello world"), out);
     EXPECT_TRUE(ok);
     EXPECT_EQ(out, "hello world");
 }
 
 TEST(SysUtilTest, CallcmdKeepsInternalNewlines)
 {
     std::string out = "stale";
     const bool ok = cutl::callcmd(
       callcmd_of("printf 'alpha\\n\\nbeta\\n'", "echo alpha& echo.& echo beta"), out);
     EXPECT_TRUE(ok);
     EXPECT_EQ(out, "alpha\n\nbeta");
 }
 
 TEST(SysUtilTest, CallcmdReadsPastSingleBuffer)
 {
     std::string out = "stale";
     const std::string payload(1200, 'b');
     const std::string cmd = callcmd_of("printf '%s\\nend_marker\\n' '" + payload + "'",
                                        "echo " + payload + "& echo end_marker");
     const bool ok = cutl::callcmd(cmd, out);
     EXPECT_TRUE(ok);
     EXPECT_EQ(out, payload + "\nend_marker");
 }
 
 TEST(SysUtilTest, CallcmdSucceedsWhenStdoutPresentDespiteExitCode)
 {
     std::string out = "stale";
     const bool ok = cutl::callcmd(
       callcmd_of("printf 'ota_version\\n'; exit 2", "echo ota_version& exit /b 2"), out);
     EXPECT_TRUE(ok);
     EXPECT_EQ(out, "ota_version");
 }
 
 TEST(SysUtilTest, CallcmdIgnoresStderrWhenStdoutIsPresent)
 {
     std::string out = "stale";
     const bool ok = cutl::callcmd(
       callcmd_of("printf 'keep_stdout\\n'; echo drop_stderr >&2",
                  "echo keep_stdout& echo drop_stderr 1>&2"),
       out);
     EXPECT_TRUE(ok);
     EXPECT_EQ(out, "keep_stdout");
 }
 
 TEST(SysUtilTest, CallcmdClearsResultWhenStdoutIsEmpty)
 {
     std::string out = "stale";
     const bool ok = cutl::callcmd(
       callcmd_of("echo pipe_err_marker >&2; exit 1", "echo pipe_err_marker 1>&2& exit /b 1"), out);
     EXPECT_FALSE(ok);
     EXPECT_TRUE(out.empty());
 }
 
 TEST(SysUtilTest, CallcmdClearsResultWhenStdoutIsWhitespace)
 {
     std::string out = "stale";
     const bool ok = cutl::callcmd(callcmd_of("printf ' \\t\\n'", "echo."), out);
     EXPECT_FALSE(ok);
     EXPECT_TRUE(out.empty());
 }
 
 TEST(SysUtilTest, CallcmdFailsWhenCommandProducesNoStdout)
 {
     std::string out = "stale";
     const bool missing = cutl::callcmd("cutl_no_such_command_zz", out);
     EXPECT_FALSE(missing);
     EXPECT_TRUE(out.empty());
 
     out = "stale";
     const bool empty = cutl::callcmd("", out);
     EXPECT_FALSE(empty);
     EXPECT_TRUE(out.empty());
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
 