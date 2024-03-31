#pragma once

#include <iostream>
#include "common_util/sysutil.h"
#include "common.hpp"

void TestSysutil()
{
    PrintTitle("sysutil");
    std::cout << "C++标准的版本：" << cutl::cpp_stl_version() << std::endl;
}
