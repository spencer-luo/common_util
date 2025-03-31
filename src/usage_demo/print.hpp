#include "common.hpp"
#include "print.h"

void PrintArr()
{
    PrintSubTitle("print_arr");

    int8_t arr[] = { 1, -2, 3 };
    cutl::print_arr(arr, 3);
}

void PrintVec()
{
    PrintSubTitle("print_vec");

    std::vector<int> vec1 = { 1, 2, -3, 4 };
    cutl::print_vec(vec1);
    std::vector<float> vec2 = { 3.14, -2.56, 5.122 };
    cutl::print_vec(vec2);
}

void PrintMap()
{
    PrintSubTitle("print_map");

    std::map<std::string, int> map1 = { { "apple", 1 }, { "banana", 2 }, { "orange", 3 } };
    cutl::print_map(map1);
    cutl::print_map(map1, true);
}

void PrintColor()
{
    PrintSubTitle("print_clr");

    // 彩色颜色打印
    cutl::print_clr("red: Hello World", cutl::cli_clr_red);
    cutl::print_clr("bright_red: Hello World", cutl::cli_clr_bright_red);
    cutl::print_clr("green: Hello World", cutl::cli_clr_green);
    cutl::print_clr("bright_green: Hello World", cutl::cli_clr_bright_green);
    cutl::print_clr("blue: Hello World", cutl::cli_clr_blue);
    cutl::print_clr("bright_blue: Hello World", cutl::cli_clr_bright_blue);
    cutl::print_clr("yellow: Hello World", cutl::cli_clr_yellow);
    cutl::print_clr("bright_yellow: Hello World", cutl::cli_clr_bright_yellow);
    cutl::print_clr("purple: Hello World", cutl::cli_clr_purple);
    cutl::print_clr("bright_purple: Hello World", cutl::cli_clr_bright_purple);
    cutl::print_clr("cyan: Hello World", cutl::cli_clr_cyan);
    cutl::print_clr("bright_cyan: Hello World", cutl::cli_clr_bright_cyan);
    cutl::print_clr("white: Hello World", cutl::cli_clr_white);
    cutl::print_clr("gray: Hello World", cutl::cli_clr_gray, cutl::cli_clr_blue);
    cutl::print_clr("deep_gray: Hello World", cutl::cli_clr_deep_gray, cutl::cli_clr_white);
    // 正常打印
    std::cout << "norman: Hello World" << std::endl;
}

void TestPrint()
{
    PrintTitle("print");

    PrintColor();
    PrintArr();
    PrintVec();
    PrintMap();
}
