#include <cstdio>
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
            fclose(file_);
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
            CUTL_ERROR("Invalid path: " + path.str());
            return false;
        }
        if (!cutl::path(dirPath).exists())
        {
            CUTL_ERROR("Directory does not exist: " + dirPath);
            return false;
        }

        FILE *fp = fopen(path.str().c_str(), "w+");
        if (nullptr == fp)
        {
            CUTL_ERROR("fail to open file:" + path.str());
            return false;
        }

        int ret = fflush(fp);
        if (0 != ret)
        {
            CUTL_ERROR("fail to flush file:" + path.str());
            fclose(fp);
            return false;
        }

        // TODO: add fsync
        // int32_t fd = fileno(fp);
        // if (0 > fd)
        // {
        //     ROBOLOG_ERROR(UpgradeController) << "get file fd is error. file:" << path.str();
        //     fclose(fp);
        //     return false;
        // }
        // (void)fsync(fd);

        ret = fclose(fp);
        if (0 != ret)
        {
            CUTL_ERROR("fail to close file:" + path.str());
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
                CUTL_ERROR("Invalid path: " + path.str());
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
                CUTL_ERROR("Invalid path: " + path.str());
                return false;
            }
            if (!cutl::path(dirPath).exists())
            {
                CUTL_ERROR("Directory does not exist: " + dirPath);
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
            CUTL_ERROR("Directory does not exist: " + path.str());
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
        file_guard fg(fopen(path.str().c_str(), "rb"));
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
        delete[] buffer;
        return std::string(buffer);
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
        return written_size == content.length();
    }

} // namespace cutl