// TODO:
#ifndef WIN32

#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include "filesystem.h"
#include "inner/logger.h"

namespace cutl
{
    // https://blog.csdn.net/giveaname/article/details/88973102
    std::string current_program_dir()
    {
        constexpr int size = 1024;
        char buffer[size] = {0};
        char *presult = getcwd(buffer, size);
        if (nullptr == presult)
        {
            CUTL_ERROR("presult is nullptr");
            return "";
        }
        return std::string(presult);
    }

    // https://blog.csdn.net/venom_snake/article/details/88066475
    bool create_dir(const std::string &dir_path)
    {
        if (mkdir(dir_path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO | S_IWOTH) != 0)
        {
            CUTL_ERROR("mkdir error. dir_path:" + dir_path + ", error:" + strerror(errno));
            return false;
        }
        return true;
    }

    bool remove_dir(const std::string &dir_path)
    {
        if (rmdir(dir_path.c_str()) != 0)
        {
            CUTL_ERROR("rmdir error. dir_path:" + dir_path + ", error:" + strerror(errno));
            return false;
        }
        return true;
    }

    bool is_special_dir(const std::string &filePath) { return filePath == "." || filePath == ".."; }
    bool remove_dir_recursive(const std::string &dir_path)
    {
        DIR *dir = opendir(dir_path.c_str()); // 打开这个目录
        if (dir == NULL)
        {
            CUTL_ERROR("opendir error. dir_path:" + dir_path + ", error:" + strerror(errno));
            return false;
        }
        struct dirent *file_info = NULL;
        // 逐个读取目录中的文件到file_info
        while ((file_info = readdir(dir)) != NULL)
        {
            // 系统有个系统文件，名为“..”和“.”,对它不做处理
            std::string fileName(file_info->d_name);
            if (is_special_dir(fileName))
            {
                continue;
            }
            struct stat file_stat; // 文件的信息
            std::string filepath = dir_path + +"/" + fileName;
            int ret = stat(filepath.c_str(), &file_stat);
            if (0 != ret)
            {
                CUTL_ERROR("stat error. filepath:" + filepath + ", error:" + strerror(errno));
                return false;
            }
            if (S_ISDIR(file_stat.st_mode))
            {
                if (!remove_dir_recursive(filepath))
                {
                    closedir(dir);
                    return false;
                }
            }
            else
            {
                int ret = remove(filepath.c_str());
                if (ret != 0)
                {
                    CUTL_ERROR("remove " + filepath + " error, ret:" + std::to_string(ret));
                    return false;
                }
            }
        }
        closedir(dir);

        // 删除当前文件夹
        int ret = rmdir(dir_path.c_str());
        if (ret != 0)
        {
            // CUTL_ERROR("remove " + dir_path + " error");
            CUTL_ERROR("rmdir error. dir_path:" + dir_path + ", error:" + strerror(errno));
            return false;
        }

        return true;
    }

} // namespace cutl

#endif // defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)