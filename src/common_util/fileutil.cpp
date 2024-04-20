#include <cstdio>
#include <sys/stat.h>
#include "fileutil.h"
#include "inner/logger.h"
#include "inner/filesystem.h"
#include "filepath.h"

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
        stat(filepath.str().c_str(), &statbuf);
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

} // namespace cutl