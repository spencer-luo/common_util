#include "filepath.h"
#include "inner/logger.h"
#include "inner/filesystem.h"
#include "strutil.h"
#include "sysutil.h"

namespace cutl
{

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
#if defined(_WIN32) || defined(__WIN32__)
        return win_separator;
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
        return file_exists(filepath_);
    }

    bool filepath::readable() const
    {
        return file_readable(filepath_);
    }

    bool filepath::writable() const
    {
        return file_writable(filepath_);
    }

    bool filepath::executable() const
    {
        return file_executable(filepath_);
    }

    filetype filepath::type() const
    {
        return get_file_type(filepath_);
    }

    bool filepath::isfile() const
    {
        return type() == filetype::file;
    }

    bool filepath::isdir() const
    {
        return type() == filetype::directory;
    }

    bool filepath::issymlink() const
    {
        return type() == filetype::symlink;
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
        if (issymlink())
        {
            return file_readlink(filepath_);
        }

        CUTL_ERROR("not a symlink, cannot get realpath");
        return "";
    }

    std::string filepath::abspath() const
    {
        auto filepath = filepath_;
        if (starts_with(filepath_, "~"))
        {
            // 把 ~ 替换成用户目录
            filepath = homedir() + filepath_.substr(1);
        }
        return absolute_path(filepath);
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