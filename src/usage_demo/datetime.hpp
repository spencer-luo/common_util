#pragma once

#include "common.hpp"
#include "common_util/datetime.h"
#include <iostream>

void TestDatetimeBasicUsage()
{
    PrintSubTitle("TestDatetimeBasicUsage");

    auto now = cutl::datetime::now();
    std::cout << "current timestamp(ms): " << now.timestamp() << std::endl;
    std::cout << "current time(UTC time): " << now.utctime() << std::endl;
    std::cout << "current time(local time): " << now.format() << std::endl;
    std::cout << "current time(UTC time) format b: " << now.format(cutl::datetime_format::datetime_format_b, false, true) << std::endl;
    std::cout << "current time(UTC time) format b, don't show milliseconds: " << now.format(cutl::datetime_format::datetime_format_b, false, false) << std::endl;
    std::cout << "current time(UTC time) format c: " << now.format(cutl::datetime_format::datetime_format_c, false, true) << std::endl;
    std::cout << "current time(UTC time) format d: " << now.format(cutl::datetime_format::datetime_format_d, false, true) << std::endl;
    std::cout << "current time(UTC time) custom format 1: " << now.format("%c %Z", false, true) << std::endl;
    std::cout << "current time(UTC time) custom format 2: " << now.format("%m/%d/%Y/ %H:%M:%S", false, false) << std::endl;
}

void TestDatetimeOperator()
{
    // 运算符重载
    PrintSubTitle("TestDatetimeOperator");

    std::cout << "one day == " << cutl::datetime::day << "ms" << std::endl;
    std::cout << "one hour == " << cutl::datetime::hour << "ms" << std::endl;
    std::cout << "one minute == " << cutl::datetime::min << "ms" << std::endl;

    auto now = cutl::datetime::now();
    std::cout << "current time: " << now << std::endl;
    auto dt1 = now - cutl::datetime::min;
    std::cout << "before one minute: " << dt1 << std::endl;
    // std::cout << "current time 1: " << now << std::endl;
    auto dt2 = now + cutl::datetime::min;
    std::cout << "after one minute: " << dt2 << std::endl;
    // std::cout << "current time 2: " << now << std::endl;

    now -= (2 * cutl::datetime::hour);
    std::cout << "before two hours: " << now << std::endl;
    now += (4 * cutl::datetime::hour);
    std::cout << "after two hours: " << now << std::endl;

    auto dt3 = cutl::datetime::get("2024-03-01 10:00:00");
    auto dt4 = cutl::datetime::get("2024-03-30 14:18:44");
    auto duration1 = dt4 - dt3;
    std::cout << "the difference between " << dt3 << " and " << dt4 << " is: " << duration1 << "ms, formatted: " << cutl::fmt_timeduration_ms(duration1) << std::endl;
    auto duration2 = dt3 - dt4;
    std::cout << "the difference between " << dt4 << " and " << dt3 << " is: " << duration2 << "ms" << std::endl;
}

void TestDatetimeParseString()
{
    // 字符串解析成时间
    PrintSubTitle("TestDatetimeParseString");
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
    // mounth error
    auto dt9 = cutl::datetime::get(" 2024-13-02 14:18:44 ");
    std::cout << "dt9: " << dt9 << std::endl;
    // leap year error
    auto dt10 = cutl::datetime::get(" 2023-02-29 14:18:44 ");
    std::cout << "dt10: " << dt10 << std::endl;
    // day error
    auto dt11 = cutl::datetime::get(" 2024-03-42 14:18:44 ");
    std::cout << "dt11: " << dt11 << std::endl;

    // year > 2038
    auto dt12 = cutl::datetime::get(" 2044-03-02 14:18:44 ");
    std::cout << "dt12: " << dt12 << std::endl;
    std::cout << "dt12 timestamp: " << dt12.timestamp() << std::endl;
}

void TestFormatByStr(const cutl::datetime& dt, bool is_logging, int log_index)
{
    // auto s = dt.timestamp() / 1000;
    // std::string str_datetime = cutl::fmt_timestamp(s, true, "%Y%m%d_%H%M");
    std::string str_datetime = dt.format("%Y%m%d_%H%M", true, false);
    // TINYLOG_INFO(tinylog) << "str_datetime:" << str_datetime;
    auto suffix = (is_logging ? "logging" : "log");
    str_datetime += "." + std::to_string(log_index);
    str_datetime += "." + std::string(suffix);
    std::cout << "str_datetime: " << str_datetime << std::endl;
}

void TestDatetime()
{
    PrintTitle("datetime");
    // TestDatetimeBasicUsage();
    // TestDatetimeOperator();
    // TestDatetimeParseString();

    TestFormatByStr(cutl::datetime::now(), true, 1);
}
