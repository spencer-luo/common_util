#pragma once

#include "common.hpp"
#include "common_util/sysutil.h"
#include <iostream>
#include <string>

void TestPlatformName()
{
    PrintSubTitle("TestPlatformName");

    auto type = cutl::platform_type();
    std::cout << "OS platform: " << cutl::platform_name(type) << std::endl;
}

void TestArchitecture()
{
    PrintSubTitle("TestArchitecture");

    std::cout << "OS architecture: " << cutl::architecture() << std::endl;
}

void TestCppStlVersion()
{
    PrintSubTitle("TestCppStlVersion");

    std::cout << "C++ STL version: " << cutl::cpp_stl_version() << std::endl;
}

void TestProgramBit()
{
    PrintSubTitle("TestProgramBit");

    std::cout << "program bit: x" << cutl::program_bit() << std::endl;
}

void TestEndian()
{
    PrintSubTitle("TestEndian");

    if (cutl::endian_type() == cutl::endian::little)
    {
        std::cout << "OS endianness: little endian" << std::endl;
    }
    else
    {
        std::cout << "OS endianness: big endian" << std::endl;
    }

    uint16_t a = 0x1234;
    std::cout << "byteswap for a: " << cutl::to_hex(cutl::byteswap(a)) << std::endl;
    uint32_t b = 0x12345678;
    std::cout << "byteswap for b: " << cutl::to_hex(cutl::byteswap(b)) << std::endl;
    uint64_t c = 0x123456789ABCDEF0;
    std::cout << "byteswap for c: " << cutl::to_hex(cutl::byteswap(c)) << std::endl;
    uint8_t bytes[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10};
    cutl::byteswap(bytes, 16);
    std::cout << "byteswap for bytes: " << cutl::to_hex(bytes, 16) << std::endl;
}

void TestSystemCall()
{
    PrintSubTitle("TestSystemCall");

    // Test cases for SystemCall function
    std::vector<std::string> testCommands = {
        "echo 'Hello, World!'", // Successful command
        "false",                // Command that exits with code 1
        "nonexistentcommand",   // Command not found
        "sleep 1",              // Another successful command
    };

    for (const auto& cmd : testCommands)
    {
        std::cout << "\n--- Testing command: " << cmd << " ---" << std::endl;
        auto ret = cutl::system(cmd);
        std::cout << "Return value: " << ret << std::endl;
    }
}

void TestSystemCall_v2()
{
    PrintSubTitle("TestSystemCall_v2");

    // Test cases for SystemCall function
    std::vector<std::string> testCommands = {
        "echo 'Hello, World!'", // Successful command
        "false",                // Command that exits with code 1
        "nonexistentcommand",   // Command not found
        "sleep 1",              // Another successful command
    };

    for (const auto& cmd : testCommands)
    {
        std::cout << "\n--- Testing command: " << cmd << " ---" << std::endl;
        auto ret = cutl::system2(cmd);
        std::cout << "Return value: " << ret << std::endl;
    }
}

void TestCallcmd()
{
    PrintSubTitle("TestCallcmd");

    // 打印一条调用的返回值和去掉首尾空白后的 stdout。失败时 result 应为空。
    auto show = [](const std::string& title, const std::string& cmd) {
        std::string result = "<previous>";
        const bool ret = cutl::callcmd(cmd, result);
        std::cout << title << std::endl;
        std::cout << "cmd: " << cmd << std::endl;
        std::cout << "return: " << ret << std::endl;
        std::cout << "result: [" << result << "]" << std::endl
                  << std::endl;
    };

#ifndef _WIN32
    show("single line", "printf 'common_util_test\\n'");
    show("multiple lines", "printf 'line-1\\nline-2\\n'");
    // 有 stdout 即成功，即使进程非零退出。ota_engine -v 属于这一类。
    show("non-zero exit with stdout", "printf 'ota_version\\n'; exit 2");
    show("stderr only", "echo pipe_err_marker >&2; exit 1");
    show("whitespace only", "printf '  \\n'");
#else
    show("single line", "echo common_util_test");
    show("multiple lines", "echo line-1& echo line-2");
    show("non-zero exit with stdout", "echo ota_version& exit /b 2");
    show("stderr only", "echo pipe_err_marker 1>&2& exit /b 1");
    show("whitespace only", "echo.");
#endif
}

void TestCallcmdProgram()
{
    PrintSubTitle("TestCallcmdProgram");

    // 由 usage_demo 的构建写入。未定义时按当前目录下的 ./callcmd 调用。
#ifndef CALLCMD_TEST_BIN
#define CALLCMD_TEST_BIN "./callcmd"
#endif
    const std::string bin = CALLCMD_TEST_BIN;
    auto show = [](const std::string& cmd) {
        std::string result = "<previous>";
        const bool ret = cutl::callcmd(cmd, result);
        std::cout << "cmd: " << cmd << std::endl;
        std::cout << "return: " << ret << std::endl;
        std::cout << "result: [" << result << "]" << std::endl
                  << std::endl;
    };

    // -s 只有 stdout，callcmd 成功并取回这两行。
    show("" + bin + " -s");
    // -e 只有 stderr。stdout 为空，callcmd 失败，错误输出写到库日志。
    show("" + bin + " -e");
    // -a 两边都有。result 只保留 stdout，不把 stderr 拼进去。
    show("" + bin + " -a");
}

void TestGetEnv()
{
    PrintSubTitle("TestGetEnv");

    auto result = cutl::getenv("PATH", "not found");
    std::cout << "getenv for PATH, result:" << std::endl
              << result << std::endl;

    // for Windows testing
    std::cout << "USERPROFILE: " << cutl::getenv("USERPROFILE", "not found") << std::endl;
    std::cout << "PROCESSOR_ARCHITECTURE: " << cutl::getenv("PROCESSOR_ARCHITECTURE", "not found") << std::endl;
    // FOR UNIX/LINUX/MAC testing
    std::cout << "HOME: " << cutl::getenv("HOME", "not found") << std::endl;
}

void TestGetcwdAndHomedir()
{
    PrintSubTitle("TestGetcwdAndHomedir");

    std::cout << "working directory: " << cutl::getcwd() << std::endl;
    std::cout << "Home directory: " << cutl::homedir() << std::endl;
}

void TestSysutil()
{
    PrintTitle("sysutil");

    // TestPlatformName();
    // TestArchitecture();
    // TestCppStlVersion();
    // TestProgramBit();
    // TestEndian();
    // TestSystemCall();
    // TestSystemCall_v2();
    // TestCallcmd();
    TestCallcmdProgram();
    // TestGetEnv();
    // TestGetcwdAndHomedir();
}
