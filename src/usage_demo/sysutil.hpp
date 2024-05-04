#pragma once

#include <iostream>
#include "common.hpp"
#include "sysutil.h"

void TestBaseFunc()
{
    PrintSubTitle("TestBaseFunc");

    auto type = cutl::platform_type();
    std::cout << "OS platform: " << cutl::platform_name(type) << std::endl;
    std::cout << "C++ STL version: " << cutl::cpp_stl_version() << std::endl;
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

    bool ret = cutl::system("echo hello");
    std::cout << "system call 'echo hello', return: " << ret << std::endl;

    auto cmd = "cmake --version";
    std::string result_text;
    ret = cutl::callcmd(cmd, result_text);
    std::cout << "callcmd " << cmd << ", return: " << ret << std::endl;
    std::cout << "callcmd " << cmd << ", result text: " << result_text << std::endl;

    auto result = cutl::getenv("PATH", "not found");
    std::cout << "getenv for PATH, result:" << std::endl
              << result << std::endl;

    // for Windows testing
    std::cout << "USERPROFILE: " << cutl::getenv("USERPROFILE", "not found") << std::endl;
    std::cout << "HOMEDRIVE: " << cutl::getenv("HOMEDRIVE", "not found") << std::endl;
    std::cout << "HOMEPATH: " << cutl::getenv("HOMEPATH", "not found") << std::endl;
}

void TestSysutil()
{
    PrintTitle("sysutil");

    TestBaseFunc();
    TestEndian();
    TestSystemCall();
}
