#pragma once

#include <string>
#include <cstdio>
#include "filepath.h"

namespace cutl
{

    // 当前程序执行的路径
    std::string current_program_dir();

    bool create_dir(const std::string &dir_path);
    // remove empty directory
    bool remove_dir(const std::string &dir_path);
    // remove directory recursively
    bool remove_dir_recursive(const std::string &dir_path);
    // fsync to disk, only for unix system
    bool file_sync(FILE *handle);
    // get directory size in bytes
    uint64_t get_dirsize(const std::string &dirpath);

    filevec list_sub_files(const std::string &dirpath, file_type type = file_type::all, bool recursive = false);

} // namespace cutl
