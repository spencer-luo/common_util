/**
 * @file callcmd.cpp
 * @brief 供 cutl::callcmd 演示调用的命令。
 *
 * ./callcmd -s  打印 stdout 的内容
 * ./callcmd -e  打印 stderr 的内容
 * ./callcmd -a  打印 stdout 和 stderr 的内容
 */

#include <iostream>
#include <string>

namespace
{

void print_stdout()
{
    std::cout << "stdout-line-1\nstdout-line-2\n";
}

void print_stderr()
{
    std::cerr << "stderr-line-1\nstderr-line-2\n";
}

void print_usage()
{
    std::cerr << "usage: callcmd -s|-e|-a\n";
}

} // namespace

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        print_usage();
        return 2;
    }

    const std::string mode(argv[1]);
    if (mode == "-s")
    {
        print_stdout();
    }
    else if (mode == "-e")
    {
        print_stderr();
    }
    else if (mode == "-a")
    {
        print_stdout();
        print_stderr();
    }
    else
    {
        print_usage();
        return 2;
    }
    return 0;
}
