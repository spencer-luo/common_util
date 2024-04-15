#pragma once

#include <string>
#include <ostream>

namespace cutl
{

    enum file_type
    {
        unknown = 0x00,
        directory = 0x02,
        file = 0x04,
        link = 0x08,
        // symlink,      // todo
        mount = 0x10, // mount device
        all = 0xFF,   // all types
    };

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