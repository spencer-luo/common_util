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
    std::string str1(str1Len, '=');
    std::string str2(str2Len, '=');
    std::cout << str1 << title << str2 << std::endl;
}

void PrintSubTitle(const std::string &title)
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
    auto curTime = cutl::fmt_timestamp_ms(cutl::timestamp(cutl::timeunit::ms));
    auto threadId = std::this_thread::get_id();
    if (cutl::loglevel::error_level == level)
    {
        std::cerr << "[" << curTime << "]" << loglevel2flag(level) << "]" << threadId << "](cutl) " << msg << std::endl;
    }
    else if (cutl::loglevel::debug_level == level)
    {
        // todo: no print anything
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
    // auto timeStampMs = cutl::timestamp(cutl::time_unit::ms);
    std::cout << "当前时间戳（秒） ：" << cutl::timestamp(cutl::timeunit::s) << std::endl;
    std::cout << "当前时间戳（毫秒）：" << cutl::timestamp(cutl::timeunit::ms) << std::endl;
    std::cout << "当前时间戳（微秒）：" << cutl::timestamp(cutl::timeunit::us) << std::endl;
    auto s = cutl::clocktime(cutl::timeunit::s);
    auto ms = cutl::clocktime(cutl::timeunit::ms);
    auto us = cutl::clocktime(cutl::timeunit::us);
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
    std::cout << "duration1: " << cutl::fmt_timeduration_s(180100) << std::endl;
    std::cout << "duration2: " << cutl::fmt_timeduration_ms(180100345) << std::endl;
    std::cout << "duration3: " << cutl::fmt_timeduration_us(180100345678) << std::endl;

    // timestamp
    auto curTime = cutl::timestamp(cutl::timeunit::ms);
    std::cout << "current datetime: " << cutl::fmt_timestamp_ms(curTime) << std::endl;
}

void TestDatetimeBasicUsage()
{
    PrintSubTitle("datetime basic usage");
    auto now = cutl::datetime::now();
    std::cout << "系统当前时间戳(毫秒): " << now.timestamp() << std::endl;
    std::cout << "系统当前时间(UTC时间): " << now.utctime() << std::endl;
    std::cout << "系统当前时间(本地时间): " << now.format() << std::endl;
    std::cout << "系统当前时间(UTC时间) 格式b: " << now.format(cutl::datetime_format::datetime_format_b, false, true) << std::endl;
    std::cout << "系统当前时间(UTC时间) 格式b,不显示毫秒: " << now.format(cutl::datetime_format::datetime_format_b, false, false) << std::endl;
    std::cout << "系统当前时间(UTC时间) 格式c: " << now.format(cutl::datetime_format::datetime_format_c, false, true) << std::endl;
    std::cout << "系统当前时间(UTC时间) 格式d: " << now.format(cutl::datetime_format::datetime_format_d, false, true) << std::endl;
    std::cout << "系统当前时间(UTC时间) 自定义格式1: " << now.format("%c %Z", false, true) << std::endl;
    std::cout << "系统当前时间(UTC时间) 自定义格式2: " << now.format("%Y年%m月%d日 %H点%M分%S秒", false, false) << std::endl;
}

void TestDatetimeOperator()
{
    // 运算符重载
    PrintSubTitle("datetime operator overwrite");
    auto now = cutl::datetime::now();
    std::cout << "1天 == " << cutl::datetime::day << "毫秒" << std::endl;
    auto dt1 = now - cutl::datetime::min;
    auto dt2 = now + cutl::datetime::min;
    std::cout << "系统当前时间(标准输出): " << now << std::endl;
    std::cout << "1分钟前: " << dt1 << std::endl;
    std::cout << "1分钟后: " << dt2 << std::endl;
    std::cout << "系统当前时间(标准输出): " << now << std::endl;
    now -= (2 * cutl::datetime::hour);
    std::cout << "2小时前: " << now << std::endl;
    now += (4 * cutl::datetime::hour);
    std::cout << "2小时后: " << now << std::endl;
}

void TestDatetimeParseString()
{
    // 字符串解析成时间
    PrintSubTitle("datetime parse string");
    auto dt0 = cutl::datetime::get(" 2024-03-02 14:18:44 ");
    std::cout << "dt0: " << dt0 << std::endl;
    auto dt1 = cutl::datetime::get(" 2024-03-02 14:18:44.023 ");
    std::cout << "dt1: " << dt1 << std::endl;
    auto dt2 = cutl::datetime::get(" 2024.03.12 14:18:44");
    std::cout << "dt2: " << dt2 << std::endl;
    auto dt3 = cutl::datetime::get(" 2024.03.12 14:18:44.003");
    std::cout << "dt3: " << dt3 << std::endl;
    auto dt4 = cutl::datetime::get("2024/03/22 14:18:44 ");
    std::cout << "dt4: " << dt4 << std::endl;
    auto dt5 = cutl::datetime::get("2024/03/22 14:18:44.200 ");
    std::cout << "dt5: " << dt5 << std::endl;
    auto dt6 = cutl::datetime::get("2024/03/23 09:28:04");
    std::cout << "dt6: " << dt6 << std::endl;
    auto dt7 = cutl::datetime::get("2024/03/23 09:28:04.276");
    std::cout << "dt7: " << dt7 << std::endl;
    // format error
    auto dt8 = cutl::datetime::get(" 2024-0322 14:18:44 ");
    std::cout << "dt8: " << dt8 << std::endl;
}

void TestDatetime()
{
    PrintTitle("datetime");
    TestDatetimeBasicUsage();
    TestDatetimeOperator();
    TestDatetimeParseString();
}

void TestTimecount()
{
    cutl::timecount tct("TestTimecount");
    PrintTitle("timecount");
    std::cout << "TestTimecount begin" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "TestTimecount end" << std::endl;
}

int main()
{
    LibraryInit();
    // TestTimeutil();
    // TestSysutil();
    // TestStrfmt();
    TestDatetime();
    // TestTimecount();

    return 0;
}