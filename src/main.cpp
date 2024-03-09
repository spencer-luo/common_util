#include <iostream>
#include "common_util/timeutil.h"
#include "common_util/sysutil.h"
#include "common_util/strfmt.h"

void PrintTitle(const std::string &title)
{
    auto len = title.length();
    constexpr int maxLen = 100;
    auto str1Len = (maxLen - len) / 2;
    auto str2Len = maxLen - len - str1Len;
    std::string str1(str1Len, '-');
    std::string str2(str2Len, '-');
    std::cout << str1 << title << str2 << std::endl;
}

void TestTimeutil()
{
    PrintTitle("timeutil");
    // auto timeStampMs = cutl::timestamp_ms();
    std::cout << "当前时间戳（秒） ：" << cutl::timestamp() << std::endl;
    std::cout << "当前时间戳（毫秒）：" << cutl::timestamp_ms() << std::endl;
    std::cout << "当前时间戳（微秒）：" << cutl::timestamp_us() << std::endl;
    auto s = cutl::clocktime();
    auto ms = cutl::clocktime_ms();
    auto us = cutl::clocktime_us();
    std::cout << "系统启动到当前的时间（秒） ：" << s << std::endl;
    std::cout << "系统启动到当前的时间（毫秒）：" << ms << std::endl;
    std::cout << "系统启动到当前的时间（微秒）：" << us << std::endl;
    std::cout << "ms2s: " << cutl::ms2s(ms) << std::endl;
    std::cout << "us2s: " << cutl::us2s(us) << std::endl;
    std::cout << "us2ms: " << cutl::us2ms(us) << std::endl;
    std::cout << "s2ms: " << cutl::s2ms(s) << std::endl;
    std::cout << "s2us: " << cutl::s2us(s) << std::endl;
    std::cout << "ms2us: " << cutl::ms2us(ms) << std::endl;
}

void TestSysutil()
{
    PrintTitle("sysutil");
    std::cout << "C++标准的版本：" << cutl::cpp_stl_version() << std::endl;
}

void TestStrfmt()
{
    PrintTitle("strfmt");
    // 180100345), "2d:2h:1m:40s:345ms"
    std::cout << "duration1: " << cutl::fmt_time_duration(180100) << std::endl;
    std::cout << "duration2: " << cutl::fmt_time_duration_ms(180100345) << std::endl;
    std::cout << "duration3: " << cutl::fmt_time_duration_us(180100345678) << std::endl;
}

int main()
{
    // TestTimeutil();
    // TestSysutil();
    TestStrfmt();
    return 0;
}