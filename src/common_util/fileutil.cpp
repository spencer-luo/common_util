#include "fileutil.h"
#include "inner/logger.h"
#include "inner/filesystem.h"
#include "filepath.h"

namespace cutl
{

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
        //     ROBOLOG_ERROR(UpgradeController) << "get file fd is error. file:" << fileName;
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

    std::string getcwd()
    {
        return current_program_dir();
    }

} // namespace cutl