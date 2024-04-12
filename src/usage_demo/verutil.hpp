#pragma once

#include "common.hpp"
#include "common_util/verutil.h"

void TestVerUtil()
{
    PrintTitle("verutil");
    auto version1 = cutl::get_version("cmake version 3.28.3");
    std::cout << "version1: " << version1 << std::endl;
    auto version2 = "v3.2.2";
    std::cout << "version2 is version: " << cutl::is_version(version2) << std::endl;
    auto version3 = cutl::get_version(version2);
    std::cout << "version3 is version: " << cutl::is_version(version3) << std::endl;
    auto ret = cutl::compare_version(version1, version3);
    std::cout << "version1 is greater than version3: " << (ret > 0) << std::endl;
}