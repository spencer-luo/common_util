#pragma once

#include <string>
#include <ostream>

namespace cutl
{

    enum filetype
    {
        unknown = 0x00,       // undefined
        directory = 0x01,     // directory
        file = 0x02,          // regular file
        symlink = 0x04,       // symbolic link
        char_special = 0x08,  // character device, only for unix
        block_special = 0x10, // block device, only for unix
        pipefifo = 0x20,      // named pipe, only for unix
        socket = 0x40,        // socket file, only for unix
        all = 0xFF,           // all type mask, includes all types
    };

    class file_entity
    {
    public:
        filetype type = {filetype::unknown};
        std::string filepath;
    };

    using filevec = std::vector<file_entity>;

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

        // // TODO: implement these functions
        // bool isfile() const;
        // bool isdir() const;
        // bool islink() const;
        // bool issymlink() const;
        // bool ismount() const;

        // parent dir path
        std::string dirname() const;
        // filename or directory name
        std::string basename() const;
        // real path for links and shortcuts
        std::string realpath() const;
        // extension with dot
        std::string extension() const;

    private:
        std::string filepath_;
    };

    std::ostream &operator<<(std::ostream &os, const filepath &fp);
    filepath path(const std::string &path);

} // namespace cutl