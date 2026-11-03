#pragma once

#include "common.hpp"
#include "common_util/strutil.h"

void TestUpperLower()
{
    PrintSubTitle("TestUpperLower");

    std::string str1 = "Hello, world!";
    std::string str2 = "GOODBYE, WORLD!";
    std::cout << "[to_upper] str1, before: " << str1 << ", after: " << cutl::to_upper(str1) << std::endl;
    std::cout << "[to_lower] str2, before: " << str2 << ", after: " << cutl::to_lower(str2) << std::endl;
}

void TestStrip()
{
    PrintSubTitle("TestStrip");

    std::string text = "  \tThis is a test string. \n ";
    // std::string text = "  \t中国 \n ";
    std::cout << "text: " << text << std::endl;
    std::cout << "trim left text: " << cutl::lstrip(text) << std::endl;
    std::cout << "trim right text: " << cutl::rstrip(text) << std::endl;
    std::cout << "trim text: " << cutl::strip(text) << std::endl;
}

void TestStartswithEndswith()
{
    PrintSubTitle("TestStartswithEndswith");

    std::string str1 = "Hello, world!";
    std::string str2 = "GOODBYE, WORLD!";
    std::cout << str1 << " start with hello : " << cutl::starts_with(str1, "hello") << std::endl;
    std::cout << str1 << " start with hello ignoreCase: " << cutl::starts_with(str1, "hello", true) << std::endl;
    std::cout << str2 << " end with world! : " << cutl::ends_with(str2, "world!") << std::endl;
    std::cout << str2 << " end with world! ignoreCase: " << cutl::ends_with(str2, "world!", true) << std::endl;
}

void TestJoinSplit()
{
    PrintSubTitle("TestJoinSplit");

    // split
    std::string fruits = "apple, banana, orange, pear";
    std::cout << "fruits: " << fruits << std::endl;
    auto fruits_vec = cutl::split(fruits, ",");
    std::cout << "list fruits item:" << std::endl;
    for (size_t i = 0; i < fruits_vec.size(); i++)
    {
        auto item = fruits_vec[i];
        auto fruit = cutl::strip(item);
        std::cout << item << ", after strip:" << fruit << std::endl;
        fruits_vec[i] = fruit;
    }
    // join
    std::cout << "join fruits with comma: " << cutl::join(fruits_vec, "; ") << std::endl;
}

namespace
{

int g_desens_demo_failures = 0;

void DesensCheck(const std::string& input,
                 uint8_t keep_left,
                 uint8_t keep_right,
                 const std::string& expected,
                 const char* what)
{
    const std::string actual = cutl::desensitizing(input, keep_left, keep_right);
    std::cout << "  " << what << ": \"" << input << "\" -> \"" << actual << "\"" << std::endl;
    if (actual == expected)
    {
        return;
    }
    ++g_desens_demo_failures;
    std::cout << "  FAIL " << what << ", expected \"" << expected << "\"" << std::endl;
}

} // namespace

void TestDesensitizing()
{
    PrintSubTitle("TestDesensitizing");
    g_desens_demo_failures = 0;

    // 0 表示该侧自动保留 len/4。长度小于 4 时整串变成 '*'。
    DesensCheck("", 0, 0, "", "empty");
    DesensCheck("abc", 0, 0, "***", "shorter than 4");
    DesensCheck("2515774", 0, 0, "2*****4", "password, auto keep 1/1");
    DesensCheck("12345678", 0, 0, "12****78", "auto keep 2/2");
    // 中间恰好隐藏 6 个字符时按实际个数填 '*'，再多一个就写成 "**..**"。
    DesensCheck("1234567890", 0, 0, "12******90", "hidden == 6");
    DesensCheck("18500425678", 0, 0, "18**..**78", "phone, hidden > 6");

    DesensCheck("18500425678", 3, 4, "185****5678", "keep 3/4");
    DesensCheck("18500425678", 3, 2, "185******78", "keep 3/2, hidden == 6");
    DesensCheck("18500425678", 2, 2, "18**..**78", "keep 2/2, hidden > 6");
    DesensCheck("abcd", 2, 2, "****", "keep covers the whole string");
    // 只把传入 0 的那一侧按 len/4 计算。
    DesensCheck("12345678", 1, 0, "1*****78", "left 1, right auto");
    DesensCheck("12345678", 0, 1, "12*****8", "left auto, right 1");

    const std::string phone = "18500425678";
    cutl::desensitizing(phone, 3, 4);
    if (phone == "18500425678")
    {
        std::cout << "  ok  input is left unchanged" << std::endl;
    }
    else
    {
        ++g_desens_demo_failures;
        std::cout << "  FAIL input was modified" << std::endl;
    }

    if (g_desens_demo_failures == 0)
    {
        std::cout << "desensitizing demo: all checks passed" << std::endl;
    }
    else
    {
        std::cout << "desensitizing demo: " << g_desens_demo_failures << " checks failed"
                  << std::endl;
    }
}

void TestStrConvert()
{
    // 注意： std::cout与std::wcout不能混用
    PrintSubTitle("TestStrConvert");

    std::string str1 = "中国";
    std::wstring wstr1 = cutl::s2ws(str1);
    std::wcout << "wstr1: " << wstr1 << std::endl;
    std::wcout.flush();

    std::wstring wstr2 = L"中华大地";
    std::string str2 = cutl::ws2s(wstr2);
    std::cout << "str2: " << str2 << std::endl;
    std::cout.flush();
}

void TestStrUtil()
{
    // PrintTitle("strutil");

    // TestUpperLower();
    // TestStrip();
    // TestStartswithEndswith();
    // TestJoinSplit();
    TestDesensitizing();
    // TestStrConvert();
}