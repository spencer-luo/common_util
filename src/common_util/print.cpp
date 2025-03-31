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
    std::cout << "\033[38;5;" << std::to_string(fgcolor) << "m" << "\033[48;5;"
              << std::to_string(bgcolor) << "m";
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

} // namespace cutl
