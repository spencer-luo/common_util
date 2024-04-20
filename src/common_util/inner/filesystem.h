#pragma once

#include <string>
#include <cstdio>
#include "filetype.h"

namespace cutl
{

    // 当前程序执行的路径
    std::string current_program_dir();

    bool file_exists(const std::string &filepath);
    bool file_readable(const std::string &filepath);
    bool file_writable(const std::string &filepath);
    bool file_executable(const std::string &filepath);

    bool create_dir(const std::string &dir_path);
    // remove empty directory
    bool remove_dir(const std::string &dir_path);
    // remove directory recursively
    bool remove_dir_recursive(const std::string &dir_path);
    // fsync to disk, only for unix system
    bool file_sync(FILE *handle);
    // get directory size in bytes
    uint64_t get_dirsize(const std::string &dirpath);

    filetype get_file_type(const std::string &filepath);
    filevec list_sub_files(const std::string &dirpath, filetype type = filetype::all, bool recursive = false);

    bool copy_attributes(const std::string &srcpath, const std::string &dstpath);

} // namespace cutl
