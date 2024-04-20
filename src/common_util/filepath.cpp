#include "filepath.h"
#include <iostream>

#include <map>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <io.h>
#define ACCESS _access
#else
#include <unistd.h>
#define ACCESS access
#endif

namespace cutl
{

    std::string filetype_flag(filetype type)
    {
        static std::map<filetype, std::string> flag_map = {
            {filetype::unknown, "u"},
            {filetype::directory, "d"},
            {filetype::file, "-"},
            {filetype::symlink, "l"},
            {filetype::char_special, "c"},
            {filetype::block_special, "b"},
            {filetype::pipefifo, "p"},
            {filetype::socket, "s"},
            {filetype::all, "a"},
        };

        auto itr = flag_map.find(type);
        if (itr != flag_map.end())
        {
            return itr->second;
        }
        return "";
    }
    static constexpr char win_separator = '\\';
    static constexpr char unix_separator = '/';

    void fixpath(std::string &path)
    {
        if (win_separator == filepath::separator())
        {
            for (size_t i = 0; i < path.size(); i++)
            {
                if (path[i] == unix_separator)
                {
                    path[i] = win_separator;
                }
            }
        }
        else if (unix_separator == filepath::separator())
        {
            for (size_t i = 0; i < path.size(); i++)
            {
                if (path[i] == win_separator)
                {
                    path[i] = unix_separator;
                }
            }
        }
        else
        {
            // do nothing
        }

        while (path.empty() || path.back() == filepath::separator())
        {
            path.pop_back();
        }
    }

    filepath::filepath(const std::string &path)
    {
        filepath_ = path;
        fixpath(filepath_);
    }

    filepath::filepath(const filepath &other)
    {
        filepath_ = other.filepath_;
    }

    filepath &filepath::operator=(const filepath &other)
    {
        this->filepath_ = other.filepath_;
        return *this;
    }

    char filepath::separator()
    {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        return win_char;
#else
        return unix_separator;
#endif
    }

    std::string filepath::str() const
    {
        return filepath_;
    }

    filepath filepath::join(const std::string &filename) const
    {
        std::string path = filepath_ + separator() + filename;
        return filepath(path);
    }

    // https://www.cnblogs.com/opensmarty/p/17108977.html
    // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/access-waccess?view=msvc-170
    bool filepath::exists() const
    {
        return (ACCESS(filepath_.c_str(), 0) == 0);
    }

    bool filepath::readable() const
    {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        return (ACCESS(filepath_.c_str(), 0x100) == 0);
#else
        return (ACCESS(filepath_.c_str(), R_OK) == 0);
#endif
    }

    bool filepath::writable() const
    {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        return (ACCESS(filepath_.c_str(), 0x10) == 0);
#else
        return (ACCESS(filepath_.c_str(), W_OK) == 0);
#endif
    }

    bool filepath::executable() const
    {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        CUTL_WARN("executable() is not supported on Windows");
        return false;
#else
        return (ACCESS(filepath_.c_str(), X_OK) == 0);
#endif
    }

    std::string filepath::dirname() const
    {
        auto index = filepath_.find_last_of(separator());
        if (index == std::string::npos)
        {
            return "";
        }
        return filepath_.substr(0, index);
    }

    std::string filepath::basename() const
    {
        auto index = filepath_.find_last_of(separator());
        // auto len = filepath_.length() - index - 1;
        if (index == std::string::npos)
        {
            return filepath_;
        }
        return filepath_.substr(index + 1);
    }

    std::string filepath::realpath() const
    {
        // TODO
        return "";
    }

    std::string filepath::extension() const
    {
        auto pos = filepath_.find_last_of('.');
        if (pos == std::string::npos)
        {
            return "";
        }

        return filepath_.substr(pos);
    }

    std::ostream &operator<<(std::ostream &os, const filepath &fp)
    {
        os << fp.str();
        return os;
    }

    filepath path(const std::string &path)
    {
        return filepath(path);
    }

} // namespace cutl