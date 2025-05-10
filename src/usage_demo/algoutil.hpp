#pragma once

#include "algoutil.h"
#include "common.hpp"

void TestClamp()
{
    PrintSubTitle("Clamp Test");
    int min_val = 0;
    int max_val = 10;
    int val = -2;
    std::cout << "clamp(" << val << ", " << min_val << ", " << max_val
              << ") = " << cutl::clamp(val, min_val, max_val) << std::endl;
    val = 15;
    std::cout << "clamp(" << val << ", " << min_val << ", " << max_val
              << ") = " << cutl::clamp(val, min_val, max_val) << std::endl;
    val = 7;
    std::cout << "clamp(" << val << ", " << min_val << ", " << max_val
              << ") = " << cutl::clamp(val, min_val, max_val) << std::endl;
}

void TestInRange()
{
    PrintSubTitle("Clamp Test");
    int min_val = 0;
    int max_val = 10;
    int val = -2;
    std::cout << "in_range(" << val << ", " << min_val << ", " << max_val
              << ") = " << cutl::in_range(val, min_val, max_val) << std::endl;
    val = 15;
    std::cout << "in_range(" << val << ", " << min_val << ", " << max_val
              << ") = " << cutl::in_range(val, min_val, max_val) << std::endl;
    val = 7;
    std::cout << "in_range(" << val << ", " << min_val << ", " << max_val
              << ") = " << cutl::in_range(val, min_val, max_val) << std::endl;

    val = 0;
    std::cout << "in_range(" << val << ", " << min_val << ", " << max_val
              << ") = " << cutl::in_range(val, min_val, max_val) << std::endl;
    val = 10;
    std::cout << "in_range(" << val << ", " << min_val << ", " << max_val
              << ") = " << cutl::in_range(val, min_val, max_val) << std::endl;
}

void TestAlgorithmUtil()
{
    PrintTitle("Algorithm Util Test");

    TestClamp();
    TestInRange();
}
