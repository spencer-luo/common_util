#pragma once

#include <string>
#include <cstdio>

namespace cutl
{

    // 当前程序执行的路径
    std::string current_program_dir();

    bool create_dir(const std::string &dir_path);
    // remove empty directory
    bool remove_dir(const std::string &dir_path);
    // remove directory recursively
    bool remove_dir_recursive(const std::string &dir_path);

    bool file_sync(FILE *handle);
} // namespace cutl
