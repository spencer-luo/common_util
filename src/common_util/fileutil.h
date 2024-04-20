#pragma once

#include <string>
#include <cstdio>
#include "strutil.h"
#include "filepath.h"

namespace cutl
{
    class file_guard
    {
    public:
        explicit file_guard(FILE *file);
        ~file_guard();
        FILE *getfd() const;

    private:
        FILE *file_;
    };

    std::string getcwd();

    bool createfile(const filepath &path);
    // recursive: create parent directories if not exist, like the 'mkdir -p' command
    bool createdir(const filepath &path, bool recursive = false);
    bool removefile(const filepath &path);
    // recursive: remove the whole directory recursively, like the 'rm -rf' command
    bool removedir(const filepath &path, bool recursive = false);
    // bool removefile_by_extenstion(const filepath &path, const filepath &ext);

    std::string readtext(const filepath &path);
    bool writetext(const filepath &path, const std::string &content);

    uint64_t filesize(const filepath &filepath);
    uint64_t dirsize(const filepath &dirpath);

    filevec list_files(const filepath &dirpath, filetype type = filetype::all, bool recursive = false);
    filevec find_files(const filepath &dirpath, const std::string &name, bool recursive = false);
    filevec find_files_by_extension(const filepath &dirpath, const std::string &extension, bool recursive = false);

    // bool copyfile(const filepath &src, const filepath &dst);
    // bool copydir(const filepath &src, const filepath &dst);
    // bool copyfileordir(const filepath &src, const filepath &dst);

} // namespace cutl