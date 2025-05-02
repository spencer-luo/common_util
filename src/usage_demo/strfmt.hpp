#pragma once

#include <iostream>
#include "common.hpp"
#include "strfmt.h"

void TestFormatUintAndDouble()
{
    PrintSubTitle("TestFormatUintAndDouble");

    std::cout << "fmt_uint 1: " << cutl::fmt_uint(12) << std::endl;
    std::cout << "fmt_uint 2: " << cutl::fmt_uint(12, 4) << std::endl;
    std::cout << "fmt_uint 3: " << cutl::fmt_uint(12, 4, 'x') << std::endl;
    std::cout << "fmt_double 1: " << cutl::fmt_double(3.141592653) << std::endl;
    std::cout << "fmt_double 2: " << cutl::fmt_double(3.141592653, 3) << std::endl;
}

void TestFormatFileSize()
{
    PrintSubTitle("TestFormatFileSize");

    std::cout << "fmt_filesize 1: " << cutl::fmt_filesize(378711367) << std::endl;
    std::cout << "fmt_filesize 2: " << cutl::fmt_filesize(378711367, true, 2) << std::endl;
    std::cout << "fmt_filesize 2: " << cutl::fmt_filesize(378711367, false, 2) << std::endl;
}

void TestFormatDurationTime()
{
    PrintSubTitle("TestFormatDurationTime");

    // 180100345), "2d:2h:1m:40s:345ms"
    std::cout << "duration1: " << cutl::fmt_timeduration_s(180100) << std::endl;
    std::cout << "duration2: " << cutl::fmt_timeduration_ms(180100345) << std::endl;
    std::cout << "duration3: " << cutl::fmt_timeduration_us(180100345678) << std::endl;
}

void TestFormatTimestamp()
{
    PrintSubTitle("TestFormatTimestamp");
    // timestamp
    auto curTimeS = cutl::timestamp(cutl::timeunit::s);
    auto curTimeMS = cutl::timestamp(cutl::timeunit::ms);
    auto curTimeUS = cutl::timestamp(cutl::timeunit::us);
    std::cout << "current datetime s: " << cutl::fmt_timestamp_s(curTimeS) << std::endl;
    std::cout << "current datetime s in UTC: " << cutl::fmt_timestamp(curTimeS, false, "%Y/%m/%d %H:%M:%S") << std::endl;
    std::cout << "current datetime ms: " << cutl::fmt_timestamp_ms(curTimeMS) << std::endl;
    std::cout << "current datetime ms in UTC: " << cutl::fmt_timestamp_ms(curTimeMS, false) << std::endl;
    std::cout << "current datetime us: " << cutl::fmt_timestamp_us(curTimeUS) << std::endl;
}

void TestToBin()
{
    PrintSubTitle("TestToBin");

    uint8_t a = 0x0f;
    std::cout << "uint8: " << cutl::to_bin(a) << std::endl;
    uint16_t b = 0xfc;
    std::cout << "uint16: " << cutl::to_bin(b) << std::endl;
    uint32_t c = 0x1b02aefc;
    std::cout << "uint32: " << cutl::to_bin(c) << std::endl;
    uint64_t d = 0xabcdef0123456789;
    std::cout << "uint64: " << cutl::to_bin(d) << std::endl;
}

void TestToHex()
{
    PrintSubTitle("TestToHex");

    uint8_t a = 0x0f;
    std::cout << "uint8: " << cutl::to_hex(a) << std::endl;
    uint16_t b = 0xfc;
    std::cout << "uint16: " << cutl::to_hex(b) << std::endl;
    uint32_t c = 0x1b02aefc;
    std::cout << "uint32: " << cutl::to_hex(c) << std::endl;
    uint64_t d = 0xabcdef0123456789;
    std::cout << "uint64: " << cutl::to_hex(d) << std::endl;
    uint8_t bytes[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10};
    std::cout << "bytes: " << cutl::to_hex(bytes, 16) << std::endl;
    int iVal = 10;
    printf("Address of iVal: %p\n", &iVal);
    std::cout << "[case1] &iVal: " << cutl::to_hex(&iVal) << std::endl;
    std::cout << "[case2] &iVal: " << cutl::to_hex(&iVal, true, true) << std::endl;
}

void TestStrfmt()
{
    PrintTitle("strfmt");
    TestFormatUintAndDouble();
    TestFormatFileSize();
    TestFormatDurationTime();
    TestFormatTimestamp();
    TestToBin();
    TestToHex();
}