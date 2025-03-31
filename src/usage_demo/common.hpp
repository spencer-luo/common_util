#pragma once

#include "print.h"
#include <iostream>
#include <string>

void PrintTitle(const std::string& title)
{
    auto len = title.length();
    constexpr int maxLen = 100;
    auto str1Len = (maxLen - len) / 2;
    auto str2Len = maxLen - len - str1Len;
    std::string str1(str1Len, '=');
    std::string str2(str2Len, '=');
    // std::cout << std::endl
    //           << str1 << title << str2 << std::endl;
    cutl::print_clr(str1 + title + str2, cutl::cli_clr_blue);
}

void PrintSubTitle(const std::string& title)
{
    auto len = title.length();
    constexpr int maxLen = 100;
    auto str1Len = (maxLen - len) / 2;
    auto str2Len = maxLen - len - str1Len;
    std::string str1(str1Len, '-');
    std::string str2(str2Len, '-');
    std::cout << str1 << title << str2 << std::endl;
}