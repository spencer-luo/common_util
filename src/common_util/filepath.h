#pragma once

#include <string>
#include <ostream>

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

        // parent dir path
        std::string dirname() const;
        // filename or directory name
        std::string basename() const;
        // extension without dot
        std::string extension() const;

    private:
        std::string filepath_;
    };

    std::ostream &operator<<(std::ostream &os, const filepath &fp);
    filepath path(const std::string &path);

} // namespace cutl