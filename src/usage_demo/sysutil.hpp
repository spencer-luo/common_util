#pragma once

#include <iostream>
#include "common.hpp"
#include "common_util/sysutil.h"

void TestBaseFunc()
{
    PrintSubTitle("TestBaseFunc");

    std::cout << "操作系统的平台：" << cutl::platform_name() << std::endl;
    std::cout << "C++标准的版本：" << cutl::cpp_stl_version() << std::endl;
    std::cout << "程序位数：" << cutl::program_bit() << "位程序" << std::endl;
}

void TestEndian()
{
    PrintSubTitle("TestEndian");

    if (cutl::endian_type() == cutl::endian::little)
    {
        std::cout << "系统大小端：小端" << std::endl;
    }
    else
    {
        std::cout << "系统大小端：大端" << std::endl;
    }

    uint16_t a = 0x1234;
    std::cout << "byteswap for a:" << cutl::to_hex(cutl::byteswap(a)) << std::endl;
    uint32_t b = 0x12345678;
    std::cout << "byteswap for b:" << cutl::to_hex(cutl::byteswap(b)) << std::endl;
    uint64_t c = 0x123456789ABCDEF0;
    std::cout << "byteswap for c:" << cutl::to_hex(cutl::byteswap(c)) << std::endl;
    uint8_t bytes[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10};
    cutl::byteswap(bytes, 16);
    std::cout << "byteswap for bytes:" << cutl::to_hex(bytes, 16) << std::endl;
}

void TestSystemCall()
{
    PrintSubTitle("TestSystemCall");

    bool ret = cutl::system("echo hello");
    std::cout << "system call return value: " << ret << std::endl;

    auto cmd = "cmake --version";
    std::string result_text;
    ret = cutl::callcmd(cmd, result_text);
    std::cout << "callcmd return value: " << ret << std::endl;
    std::cout << "callcmd result text: " << result_text << std::endl;

    auto result = cutl::getenv("PATH", "not found");
    std::cout << "getenv result: " << result << std::endl;
}

void TestSysutil()
{
    PrintTitle("sysutil");

    TestBaseFunc();
    TestEndian();
    TestSystemCall();
}
