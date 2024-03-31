#pragma once

#include <iostream>
#include <string>
#include "common_util/timeutil.h"
#include "common.hpp"

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
