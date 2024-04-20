#pragma once

#include <string>
#include <cstdio>
#include "filetype.h"

namespace cutl
{

    class filepath
    {
    public:
        filepath(const std::string &path);
        filepath(const filepath &other);
        filepath &operator=(const filepath &other);
        ~filepath() = default;

    public:
        static char separator();
        std::string str() const;
        filepath join(const std::string &filename) const;

        // file is exist or not
        bool exists() const;
        bool readable() const;
        bool writable() const;
        bool executable() const;

        filetype type() const;
        bool isfile() const;
        bool isdir() const;
        bool issymlink() const;

        // parent dir path
        std::string dirname() const;
        // filename or directory name
        std::string basename() const;
        // real path for the reference of symbolic link or shortcuts
        std::string realpath() const;
        // extension with dot
        std::string extension() const;

    private:
        std::string filepath_;
    };

    class file_guard
    {
    public:
        explicit file_guard(FILE *file);
        ~file_guard();
        FILE *getfd() const;

    private:
        FILE *file_;
    };

    std::ostream &operator<<(std::ostream &os, const filepath &fp);
    filepath path(const std::string &path);

    std::string filetype_flag(filetype type);

    std::string getcwd();

    bool createfile(const filepath &path);
    bool createlink(const filepath &referenece, const filepath &filepath);
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

    // copy file or symlink
    bool copyfile(const filepath &srcpath, const filepath &dstpath, bool attributes = false);
    // copy directory recursively
    bool copydir(const filepath &srcdir, const filepath &dstdir);

} // namespace cutl