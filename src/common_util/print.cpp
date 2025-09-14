#include "print.h"
#include <iostream>
#if defined(_WIN32)
#include <windows.h>
#endif

namespace cutl
{

void set_color(uint8_t fgcolor, uint8_t bgcolor)
{
#if defined(_WIN32)
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (bgcolor << 4) | fgcolor);
#else
    // 【16 色模式】这里采用16色模式，16色模式的兼容性更好
    //  前景色：\033[XXm（XX 为颜色代码）
    //  背景色：\033[YYm（YY = XX + 10）
    std::cout << "\033[" << std::to_string(fgcolor) << "m" << "\033["
              << std::to_string(bgcolor + 10) << "m";
    // 【256 色模式】
    // 前景色：\033[38;5;{n}m（n 为 0-255）
    // 背景色：\033[48;5;{n}m
    // std::cout << "\033[38;5;" << std::to_string(fgcolor) << "m" << "\033[38;5;"
    //           << std::to_string(bgcolor + 10) << "m";

#endif
}

void reset_color()
{
    set_color(cli_clr_white, cli_clr_black);
}

void print_clr(const std::string& str, uint8_t fgcolor, uint8_t bgcolor)
{
    set_color(fgcolor, bgcolor);
    std::cout << str;
    reset_color();
    std::cout << std::endl;
}

void print_debug(const std::string& str)
{
    cutl::print_clr(str, cutl::cli_clr_deep_gray);
}

void print_info(const std::string& str)
{
    cutl::print_clr(str, cutl::cli_clr_white);
}

void print_warn(const std::string& str)
{
    cutl::print_clr(str, cutl::cli_clr_bright_purple);
}

void print_error(const std::string& str)
{
    cutl::print_clr(str, cutl::cli_clr_bright_red);
}

void print_success(const std::string& str)
{
    cutl::print_clr(str, cutl::cli_clr_bright_green);
}

} // namespace cutl
