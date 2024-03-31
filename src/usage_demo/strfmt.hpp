#pragma once

#include <iostream>
#include "common.hpp"
#include "common_util/strfmt.h"

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