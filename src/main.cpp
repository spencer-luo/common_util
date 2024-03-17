#include <iostream>
#include <thread>
#include <map>
#include <mutex>
#include "common_util/timeutil.h"
#include "common_util/sysutil.h"
#include "common_util/strfmt.h"
#include "common_util/datetime.h"
#include "common_util/timecount.h"
#include "common_util/config.h"

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

std::string loglevel2flag(cutl::loglevel level)
{
    static std::map<cutl::loglevel, std::string> levelMap = {
        {cutl::loglevel::debug_level, "[D]"},
        {cutl::loglevel::info_level, "[I]"},
        {cutl::loglevel::warn_level, "[W]"},
        {cutl::loglevel::error_level, "[E]"},
    };
    auto itr = levelMap.find(level);
    if (itr != levelMap.end())
    {
        return itr->second;
    }
    return "[?]";
}

static std::mutex g_log_mtx_;
void static library_log_func(cutl::loglevel level, const std::string &msg)
{
    std::lock_guard<std::mutex> lock(g_log_mtx_);
    // todo
    auto curTime = cutl::fmt_timestamp_ms(cutl::timestamp_ms());
    auto threadId = std::this_thread::get_id();
    if (cutl::loglevel::error_level == level)
    {
        std::cerr << "[" << curTime << "]" << loglevel2flag(level) << "]" << threadId << "](cutl) " << msg << std::endl;
    }
    else
    {
        std::cout << "[" << curTime << "]" << loglevel2flag(level) << "]" << threadId << "](cutl) " << msg << std::endl;
    }
}

void LibraryInit()
{
    PrintTitle("library library_init");
    cutl::library_init(library_log_func);
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
    std::cout << "fmt_uint: " << cutl::fmt_uint(12, 5) << std::endl;
    // 180100345), "2d:2h:1m:40s:345ms"
    std::cout << "duration1: " << cutl::fmt_timeduration(180100) << std::endl;
    std::cout << "duration2: " << cutl::fmt_timeduration_ms(180100345) << std::endl;
    std::cout << "duration3: " << cutl::fmt_timeduration_us(180100345678) << std::endl;

    // timestamp
    auto curTime = cutl::timestamp();
    std::cout << "current datetime: " << cutl::fmt_timestamp(curTime, true) << std::endl;
}

void TestDatetime()
{
    PrintTitle("datetime");
    auto now = cutl::datetime::now();
    std::cout << "系统当前时间戳(毫秒): " << now.timestamp() << std::endl;
    std::cout << "系统当前时间(UTC时间): " << now.format() << std::endl;
    std::cout << "系统当前时间(北京时间): " << now.localtime() << std::endl;
    std::cout << "系统当前时间(UTC时间) 格式b: " << now.format(false, true, cutl::datetime_format::datetime_format_b) << std::endl;
    std::cout << "系统当前时间(UTC时间) 格式b,不显示毫秒: " << now.format(false, false, cutl::datetime_format::datetime_format_b) << std::endl;
    std::cout << "系统当前时间(UTC时间) 格式c: " << now.format(false, true, cutl::datetime_format::datetime_format_c) << std::endl;
    std::cout << "系统当前时间(UTC时间) 格式d: " << now.format(false, true, cutl::datetime_format::datetime_format_d) << std::endl;
}

void TestTimecount()
{
    cutl::timecount tct("TestTimecount");
    PrintTitle("timecount");
    std::cout << "TestTimecount begin" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "TestTimecount end" << std::endl;
}

int main()
{
    LibraryInit();
    // TestTimeutil();
    // TestSysutil();
    // TestStrfmt();
    TestDatetime();
    TestTimecount();

    return 0;
}