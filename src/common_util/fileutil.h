#pragma once

#include <string>
#include "strutil.h"
#include "filepath.h"

namespace cutl
{
    std::string getcwd();

    bool createfile(const filepath &path);
    // recursive: create parent directories if not exist, like the 'mkdir -p' command
    bool createdir(const filepath &path, bool recursive = false);
    bool removefile(const filepath &path);
    // recursive: remove the whole directory recursively, like the 'rm -rf' command
    bool removedir(const filepath &path, bool recursive = false);
    // bool removefileordir(const filepath &path);
    // bool removefile_by_extenstion(const filepath &path, const filepath &ext);

    // std::string readfile(const filepath &path);
    // bool writefile(const filepath &path, const filepath &content);

    // uint64_t filesize(const filepath &filepath);
    // uint64_t dirsize(const filepath &dirpath);

    // strvec list_files(const filepath &dirpath, file_type type = file_type::ALL, bool recursive = false);
    // strvec find_files_by_name(const filepath &dirpath, const filepath &name, bool recursive = false);
    // strvec find_files_by_extenstion(const filepath &dirpath, const filepath &extenstion, bool recursive = false);

    // bool copyfile(const filepath &src, const filepath &dst);
    // bool copydir(const filepath &src, const filepath &dst);
    // bool copyfileordir(const filepath &src, const filepath &dst);

} // namespace cutl