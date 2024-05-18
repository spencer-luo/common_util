#pragma once

#include <iostream>
#include <string>
#include "timeutil.h"
#include "common.hpp"

void TestTimestamp()
{
    PrintSubTitle("TestTimestamp");

    std::cout << "current timestamp( s): " << cutl::timestamp(cutl::timeunit::s) << std::endl;
    std::cout << "current timestamp(ms): " << cutl::timestamp(cutl::timeunit::ms) << std::endl;
    std::cout << "current timestamp(us): " << cutl::timestamp(cutl::timeunit::us) << std::endl;
}

void TestClocktime()
{
    PrintSubTitle("TestClocktime");

    auto s = cutl::clocktime(cutl::timeunit::s);
    auto ms = cutl::clocktime(cutl::timeunit::ms);
    auto us = cutl::clocktime(cutl::timeunit::us);
    std::cout << "duration time from system start( s): " << s << std::endl;
    std::cout << "duration time from system start(ms): " << ms << std::endl;
    std::cout << "duration time from system start(us): " << us << std::endl;
}

void TestTimeConversion()
{
    PrintSubTitle("TestTimeConversion");

    auto s = cutl::clocktime(cutl::timeunit::s);
    auto ms = cutl::clocktime(cutl::timeunit::ms);
    auto us = cutl::clocktime(cutl::timeunit::us);
    std::cout << " s: " << s << std::endl;
    std::cout << "ms: " << ms << std::endl;
    std::cout << "us: " << us << std::endl;
    std::cout << "ms2s: " << cutl::ms2s(ms) << std::endl;
    std::cout << "us2s: " << cutl::us2s(us) << std::endl;
    std::cout << "us2ms: " << cutl::us2ms(us) << std::endl;
    std::cout << "s2ms: " << cutl::s2ms(s) << std::endl;
    std::cout << "s2us: " << cutl::s2us(s) << std::endl;
    std::cout << "ms2us: " << cutl::ms2us(ms) << std::endl;
}

void TestTimeutil()
{
    PrintTitle("timeutil");

    TestTimestamp();
    TestClocktime();
    TestTimeConversion();
}
