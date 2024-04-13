#pragma once

#include "common.hpp"
#include "common_util/strutil.h"

void TestUpperLower()
{
    PrintSubTitle("to upper/lower");

    std::string str1 = "Hello, world!";
    std::string str2 = "GOODBYE, WORLD!";
    std::cout << "[to_upper] str1, before: " << str1 << ", after: " << cutl::to_upper(str1) << std::endl;
    std::cout << "[to_lower] str2, before: " << str2 << ", after: " << cutl::to_lower(str2) << std::endl;
}

void TestStrip()
{
    PrintSubTitle("strip");

    std::string text = "  \tThis is a test string. \n ";
    // std::string text = "  \t中国 \n ";
    std::cout << "text:" << text << std::endl;
    std::cout << "trim left text:" << cutl::lstrip(text) << std::endl;
    std::cout << "trim right text:" << cutl::rstrip(text) << std::endl;
    std::cout << "trim text:" << cutl::strip(text) << std::endl;
}

void TestStartswithEndswith()
{
    PrintSubTitle("starts_with/ends_with");

    std::string str1 = "Hello, world!";
    std::string str2 = "GOODBYE, WORLD!";
    std::cout << str1 << " start with hello : " << cutl::starts_with(str1, "hello") << std::endl;
    std::cout << str1 << " start with hello ignoreCase: " << cutl::starts_with(str1, "hello", true) << std::endl;
    std::cout << str2 << " end with world! : " << cutl::ends_with(str2, "world!") << std::endl;
    std::cout << str2 << " end with world! ignoreCase: " << cutl::ends_with(str2, "world!", true) << std::endl;
}

void TestJoinSplit()
{
    PrintSubTitle("join/split");

    std::string fruits = "apple, banana, orange, pear";
    auto fruits_vec = cutl::split(fruits, ",");
    std::cout << "list fruits:" << std::endl;
    for (size_t i = 0; i < fruits_vec.size(); i++)
    {
        auto item = fruits_vec[i];
        auto fruit = cutl::strip(item);
        std::cout << item << ", strip:" << fruit << std::endl;
        fruits_vec[i] = fruit;
    }
    std::cout << "join fruits with comma: " << cutl::join(fruits_vec, "; ") << std::endl;
}

void TestDesensitizing()
{
    PrintSubTitle("desensitizing");

    std::string password = "2515774";
    std::cout << "password: " << cutl::desensitizing(password) << std::endl;
    std::string phone = "18500425678";
    std::cout << "phone: " << cutl::desensitizing(phone) << std::endl;
}

void TestStrUtil()
{
    PrintTitle("strutil");

    TestUpperLower();
    TestStrip();
    TestStartswithEndswith();
    TestJoinSplit();
    TestDesensitizing();
}