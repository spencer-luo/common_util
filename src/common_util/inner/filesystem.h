#pragma once

#include <string>

namespace cutl
{

    // 当前程序执行的路径
    std::string current_program_dir();

    bool create_dir(const std::string &dir_path);
    bool remove_dir(const std::string &dir_path);

} // namespace cutl
