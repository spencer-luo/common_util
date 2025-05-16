#pragma once

#include <string>
#include <cstdio>
#include <cstdint>
#include "filetype.h"

namespace cutl
{

    // 当前程序执行的路径
    std::string current_program_dir();

    // 相对路径转绝对路径
    std::string absolute_path(const std::string &releative_path);

    bool is_special_dir(const std::string &filePath);

    bool file_exists(const std::string &filepath);
    bool file_readable(const std::string &filepath);
    bool file_writable(const std::string &filepath);
    bool file_executable(const std::string &filepath);

    std::string file_readlink(const std::string &filepath);
    bool file_createlink(const std::string &referenece, const std::string &filepath);
    bool file_removelink(const std::string &filepath);

    bool create_dir(const std::string &dir_path);
    // remove empty directory
    bool remove_dir(const std::string &dir_path);
    // remove directory recursively
    bool remove_dir_recursive(const std::string &dir_path);

    // get file size in bytes
    uint64_t get_filesize(const std::string &filepath, bool link_target);
    // get directory size in bytes
    uint64_t get_dirsize(const std::string &dirpath);

    filetype get_file_type(const std::string &filepath);
    filevec list_sub_files(const std::string& dirpath,
                           filetype type = filetype::ft_all,
                           bool recursive = false);

    // 拷贝文件/文件夹的属性，isdir参数只在windows下生效,Unix-like系统下不起作用
    bool copy_attributes(const std::string &srcpath, const std::string &dstpath, bool isdir = false);

    // fsync to disk, only for unix system
    bool file_sync(FILE *handle);

    // 获取文件最近修改时间，返回以秒为单位的时间戳
    uint64_t get_last_modified_time_s(const std::string& filepath);

} // namespace cutl
