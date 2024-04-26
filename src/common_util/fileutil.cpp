#include <cstdio>
#include <map>
#include <iostream>
#include <sys/stat.h>
#include "fileutil.h"
#include "inner/logger.h"
#include "inner/filesystem.h"
#include "strutil.h"

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

    file_guard::file_guard(FILE *file)
        : file_(file)
    {
    }

    file_guard::~file_guard()
    {
        if (file_)
        {
            int ret = fclose(file_);
            if (ret != 0)
            {
                CUTL_ERROR("fail to close file, ret" + std::to_string(ret));
            }
            file_ = nullptr;
        }
        // ROBOLOG_DCHECK(file_ == nullptr);
    }

    FILE *file_guard::getfd() const
    {
        return file_;
    }

    std::string getcwd()
    {
        return current_program_dir();
    }

    bool createfile(const filepath &path)
    {
        auto dirPath = path.dirname();
        if (dirPath.empty())
        {
            CUTL_ERROR("invalid path: " + path.str());
            return false;
        }
        if (!cutl::path(dirPath).exists())
        {
            CUTL_ERROR("directory does not exist: " + dirPath);
            return false;
        }

        file_guard fg(fopen(path.str().c_str(), "w"));
        if (fg.getfd() == nullptr)
        {
            CUTL_ERROR("fail to open file:" + path.str());
            return false;
        }

        int ret = fflush(fg.getfd());
        if (0 != ret)
        {
            CUTL_ERROR("fail to flush file:" + path.str());
            return false;
        }

        if (!file_sync(fg.getfd()))
        {
            CUTL_ERROR("file_sync failed for " + path.str());
            return false;
        }

        return true;
    }

    bool createlink(const filepath &referenece, const filepath &filepath)
    {
        return file_createlink(referenece.str(), filepath.str());
    }

    bool createdir(const filepath &path, bool recursive)
    {
        if (recursive)
        {
            constexpr int buf_size = MAX_PATH_LEN;
            char buffer[buf_size] = {0};
            int ret = snprintf(buffer, buf_size, "%s", path.str().c_str());
            if (ret < 0 || ret >= buf_size)
            {
                CUTL_ERROR("invalid path: " + path.str());
                return false;
            }
            int len = strlen(buffer);
            if (buffer[len - 1] != filepath::separator())
            {
                buffer[len++] = filepath::separator();
            }

            int32_t idx = (buffer[0] == filepath::separator()) ? 1 : 0;
            for (; idx < len; ++idx)
            {
                if (buffer[idx] != filepath::separator())
                {
                    continue;
                }
                buffer[idx] = '\0';
                filepath temp_path(buffer);
                if (!temp_path.exists())
                {
                    if (!create_dir(temp_path.str()))
                    {
                        CUTL_ERROR("createdir error. dir:" + temp_path.str());
                        return false;
                    }
                }
                buffer[idx] = filepath::separator();
            }
            return true;
        }
        else
        {
            auto dirPath = path.dirname();
            if (dirPath.empty())
            {
                CUTL_ERROR("invalid path: " + path.str());
                return false;
            }
            if (!cutl::path(dirPath).exists())
            {
                CUTL_ERROR("directory does not exist: " + dirPath);
                return false;
            }

            return create_dir(path.str());
        }
    }

    bool removefile(const filepath &path)
    {
        int ret = remove(path.str().c_str());
        if (ret != 0)
        {
            CUTL_ERROR("remove " + path.str() + " error, ret:" + std::to_string(ret));
            return false;
        }
        return true;
    }

    bool removedir(const filepath &path, bool recursive)
    {
        if (!path.exists())
        {
            CUTL_ERROR("directory does not exist: " + path.str());
            return false;
        }

        if (recursive)
        {
            return remove_dir_recursive(path.str());
        }
        else
        {
            return remove_dir(path.str());
        }
    }

    std::string readtext(const filepath &path)
    {
        // max read size 4k
        static constexpr size_t MAX_READ_SIZE = 4 * 1024;
        file_guard fg(fopen(path.str().c_str(), "r"));
        if (fg.getfd() == nullptr)
        {
            CUTL_ERROR("open file failed for " + path.str());
            return "";
        }

        // TODO: use filesize to optimize read size
        // get file size
        fseek(fg.getfd(), 0, SEEK_END);
        size_t data_len = static_cast<size_t>(ftell(fg.getfd()));
        rewind(fg.getfd());

        // get read size
        if (data_len > MAX_READ_SIZE)
        {
            data_len = MAX_READ_SIZE;
            CUTL_WARN("file size is large than 4k, only read the first 4k data for file:" + path.str());
        }

        char *buffer = new char[data_len + 1];
        if (buffer == nullptr)
        {
            CUTL_ERROR("buffer alloc failed, data_len:" + std::to_string(data_len));
            return "";
        }
        size_t read_len = static_cast<size_t>(fread(buffer, 1, data_len, fg.getfd()));
        if (read_len < data_len)
        {
            CUTL_ERROR("read file failed, only read " + std::to_string(read_len) + " bytes for " + path.str());
        }

        auto text = std::string(buffer);
        delete[] buffer;

        return text;
    }

    bool writetext(const filepath &path, const std::string &content)
    {
        // std::lock_guard
        file_guard fg(fopen(path.str().c_str(), "w"));
        if (fg.getfd() == nullptr)
        {
            CUTL_ERROR("open file failed, " + path.str());
            return false;
        }

        size_t written_size = fwrite(content.c_str(), 1, content.length(), fg.getfd());
        if (written_size != content.length())
        {
            CUTL_ERROR("written size is not equal to content size for " + path.str());
            return false;
        }

        int ret = fflush(fg.getfd());
        if (0 != ret)
        {
            CUTL_ERROR("fail to flush file:" + path.str());
            return false;
        }

        if (!file_sync(fg.getfd()))
        {
            CUTL_ERROR("file_sync failed for " + path.str());
            return false;
        }

        return true;
    }

    uint64_t filesize(const filepath &filepath)
    {
        if (!filepath.exists())
        {
            CUTL_ERROR("filepath does not exist: " + filepath.str());
            return 0;
        }

        struct stat statbuf;
        int ret = lstat(filepath.str().c_str(), &statbuf);
        if (ret != 0)
        {
            CUTL_ERROR("lstat " + filepath.str() + " error, ret:" + std::to_string(ret));
            return 0;
        }

        return static_cast<uint64_t>(statbuf.st_size);
    }

    uint64_t dirsize(const filepath &dirpath)
    {
        return get_dirsize(dirpath.str());
    }

    filevec list_files(const filepath &dirpath, filetype type, bool recursive)
    {
        return list_sub_files(dirpath.str(), type, recursive);
    }

    filevec find_files(const filepath &dirpath, const std::string &name, bool recursive)
    {
        filevec filelist = list_files(dirpath, filetype::all, recursive);
        filevec result;
        for (auto &file : filelist)
        {
            if (file.type == filetype::directory)
            {
                continue;
            }
            auto filename = cutl::path(file.filepath).basename();
            if (filename.find(name) != std::string::npos)
            {
                result.emplace_back(file);
            }
        }
        return result;
    }

    filevec find_files_by_extension(const filepath &dirpath, const std::string &extension, bool recursive)
    {
        filevec filelist = list_files(dirpath, filetype::all, recursive);
        filevec result;
        for (auto &file : filelist)
        {
            if (file.type == filetype::directory)
            {
                continue;
            }
            auto extname = cutl::path(file.filepath).extension();
            if (cutl::to_lower(extname) == cutl::to_lower(extension))
            {
                result.emplace_back(file);
            }
        }
        return result;
    }

    bool copyfile(const filepath &srcpath, const filepath &dstpath, bool attributes)
    {
        // copy file content
        if (srcpath.isfile())
        {
            file_guard frd(fopen(srcpath.str().c_str(), "rb"));
            if (frd.getfd() == nullptr)
            {
                CUTL_ERROR("open file failed, " + srcpath.str());
                return false;
            }
            file_guard fwt(fopen(dstpath.str().c_str(), "wb"));
            if (fwt.getfd() == nullptr)
            {
                CUTL_ERROR("open file failed, " + dstpath.str());
                return false;
            }

            static constexpr size_t buf_size = 8 * 1024;
            uint8_t buffer[buf_size] = {0};
            size_t read_len = 0;
            size_t write_len = 0;
            while ((read_len = fread(buffer, 1, buf_size, frd.getfd())) > 0)
            {
                write_len = fwrite(buffer, 1, read_len, fwt.getfd());
                if (write_len != read_len)
                {
                    CUTL_ERROR("write file failed, only write " + std::to_string(write_len) + ", read_len:" + std::to_string(read_len));
                    return false;
                }
            }
        }
        else if (srcpath.issymlink())
        {
            auto link_path = file_readlink(srcpath.str());
            if (link_path.empty())
            {
                CUTL_ERROR("readlink failed for " + srcpath.str());
                return false;
            }
            if (!file_createlink(link_path, dstpath.str()))
            {
                CUTL_ERROR("createlink failed for " + dstpath.str());
                return false;
            }
        }
        else
        {
            CUTL_ERROR("not a file or symlink, cannot copy: [" + filetype_flag(srcpath.type()) + "]" + srcpath.str());
            return false;
        }

        // copy file attributes
        if (attributes && srcpath.isfile())
        {
            return copy_attributes(srcpath.str(), dstpath.str());
        }

        return true;
    }

    // https://www.cnblogs.com/harrypotterjackson/p/12113382.html
    bool copydir(const filepath &srcdir, const filepath &dstdir)
    {
        if (!srcdir.isdir())
        {
            CUTL_ERROR("srcdir is not a directory: " + srcdir.str());
            return false;
        }

        if (!dstdir.exists() && !createdir(dstdir, true))
        {
            CUTL_ERROR("createdir failed for " + dstdir.str());
            return false;
        }

        auto filelist = list_files(srcdir, filetype::all, true);
        for (size_t i = 0; i < filelist.size(); i++)
        {
            auto file = filelist[i];
            auto src_file = file.filepath;
            auto reletive_path = src_file.substr(srcdir.str().length() + 1);
            auto dstpath = dstdir.join(reletive_path);
            auto srcpath = cutl::path(src_file);
            if (file.type == filetype::file || file.type == filetype::symlink)
            {
                if (!copyfile(srcpath, dstpath, true))
                {
                    return false;
                }
            }
            else if (file.type == filetype::directory)
            {
                if (!createdir(dstpath, true))
                {
                    return false;
                }

                if (!copy_attributes(src_file, dstpath.str()))
                {
                    return false;
                }
            }
            else
            {
                CUTL_WARN("the file cannot be copy: [" + filetype_flag(srcpath.type()) + "]" + srcpath.str());
                continue;
            }
        }

        return true;
    }

} // namespace cutl