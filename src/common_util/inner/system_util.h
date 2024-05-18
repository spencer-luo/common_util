#pragma once

#include <string>

namespace cutl
{

    // 系统调用函数
    bool call_system(const std::string &cmd);

    FILE *pipline_open(const std::string &cmd);

    int pipline_close(FILE *stream);

} // namespace cutl
