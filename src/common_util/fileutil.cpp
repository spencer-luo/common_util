#include <cstdio>
#include <sys/stat.h>
#include "fileutil.h"
#include "inner/logger.h"
#include "inner/filesystem.h"
#include "filepath.h"
#include "strutil.h"

namespace cutl
{

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

    bool createdir(const filepath &path, bool recursive)
    {
        if (recursive)
        {
            constexpr int buf_size = 1024;
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

        // copy file attributes
        if (attributes)
        {
            return copy_attributes(srcpath.str(), dstpath.str());
        }

        return true;
    }

    // https://www.cnblogs.com/harrypotterjackson/p/12113382.html
    bool copydir(const filepath &srcdir, const filepath &dstdir)
    {
        return false;
    }

} // namespace cutl