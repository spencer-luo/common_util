#pragma once

#include "common.hpp"
#include "verutil.h"

void TestIsVersion()
{
    PrintSubTitle("TestIsVersion");

    // is version
    auto version1 = "3.28.3";
    std::cout << version1 << " is version: " << cutl::is_version(version1) << std::endl;
    auto version2 = "v3.2.2";
    std::cout << version2 << " is version: " << cutl::is_version(version2) << std::endl;
}

void TestGetVersion()
{
    PrintSubTitle("TestGetVersion");

    // get version from string
    auto version1 = cutl::get_version("cmake version 3.28.3");
    std::cout << "version1: " << version1 << std::endl;
}

void TestCompareVersion()
{
    PrintSubTitle("TestCompareVersion");

    auto version1 = "3.28.3";
    auto version2 = "3.2.2";
    auto version3 = "3.30.0";

    auto adjective = [](int ret)
    { return ret > 0 ? " greater than " : (ret < 0 ? " less than " : " equal to "); };

    auto ret1 = cutl::compare_version(version1, version2);
    std::cout << version1 << adjective(ret1) << version2 << std::endl;
    auto ret2 = cutl::compare_version(version1, version3);
    std::cout << version1 << adjective(ret2) << version3 << std::endl;
}

void TestVerUtil()
{
    PrintTitle("verutil");

    TestGetVersion();
    TestIsVersion();
    TestCompareVersion();
}