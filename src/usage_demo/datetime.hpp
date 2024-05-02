#pragma once

#include <iostream>
#include "datetime.h"
#include "common.hpp"

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

    auto dt3 = cutl::datetime::get(" 2024-03-01 10:00:00");
    auto dt4 = cutl::datetime::get(" 2024-03-30 14:18:44");
    auto duration1 = dt4 - dt3;
    std::cout << dt3 << " 与 " << dt4 << " 相差" << duration1 << "毫秒, 时长：" << cutl::fmt_timeduration_ms(duration1) << std::endl;
    auto duration2 = dt3 - dt4;
    std::cout << dt4 << " 与 " << dt3 << " 相差" << duration2 << "毫秒" << std::endl;
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
    // mounth error
    auto dt9 = cutl::datetime::get(" 2024-13-02 14:18:44 ");
    std::cout << "dt9: " << dt9 << std::endl;
    // leap year error
    auto dt10 = cutl::datetime::get(" 2023-02-29 14:18:44 ");
    std::cout << "dt10: " << dt10 << std::endl;
    // day error
    auto dt11 = cutl::datetime::get(" 2024-03-42 14:18:44 ");
    std::cout << "dt11: " << dt11 << std::endl;
}

void TestDatetime()
{
    PrintTitle("datetime");
    TestDatetimeBasicUsage();
    TestDatetimeOperator();
    TestDatetimeParseString();
}
