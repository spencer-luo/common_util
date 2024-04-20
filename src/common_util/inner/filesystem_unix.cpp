// TODO:
#ifndef WIN32

#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stack>
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
            std::string filename(file_info->d_name);
            if (is_special_dir(filename))
            {
                continue;
            }
            struct stat file_stat; // 文件的信息
            std::string filepath = dir_path + "/" + filename;
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

    bool file_sync(FILE *handle)
    {
        int32_t fd = fileno(handle);
        if (fd < 0)
        {
            CUTL_ERROR(std::string("get file fd is error: ") + strerror(errno));
            return false;
        }
        int ret = fsync(fd);
        if (ret != 0)
        {
            CUTL_ERROR(std::string("fsync error: ") + strerror(errno));
            return false;
        }
        return true;
    }

    uint64_t get_dirsize(const std::string &dirpath)
    {
        uint64_t totalSize = 0;

        std::stack<std::string> dirs;
        DIR *dir = opendir(dirpath.c_str());
        if (dir == nullptr)
        {
            CUTL_ERROR("open " + dirpath + " error");
            return 0;
        }

        dirs.push(dirpath);

        while (!dirs.empty())
        {
            std::string currentDir = dirs.top();
            dirs.pop();
            DIR *subDir = opendir(currentDir.c_str());
            if (subDir == nullptr)
            {
                CUTL_ERROR("open " + currentDir + " error");
                continue;
            }
            struct dirent *file_info = nullptr;
            while ((file_info = readdir(subDir)) != nullptr)
            {
                std::string filename(file_info->d_name);
                if (file_info->d_type == DT_DIR)
                {
                    if (is_special_dir(filename))
                    {
                        // do nothing
                        continue;
                    }
                    else
                    {
                        // 子文件夹
                        std::string subDirPath = currentDir + "/" + filename;
                        dirs.push(subDirPath);
                    }
                }
                else
                {
                    // 普通文件
                    std::string filepath = currentDir + "/" + filename;
                    // TODO: 肯需要替换成函数
                    struct stat statbuf;
                    stat(filepath.c_str(), &statbuf);
                    totalSize += statbuf.st_size;
                }
            }
            closedir(subDir);
        }
        closedir(dir);

        return totalSize;
    }

    filetype get_file_type(int mode)
    {
        filetype type = filetype::unknown;
        if (S_ISBLK(mode))
        {
            type = filetype::block_special;
        }
        else if (S_ISCHR(mode))
        {
            type = filetype::char_special;
        }
        else if (S_ISDIR(mode))
        {
            type = filetype::directory;
        }
        else if (S_ISFIFO(mode))
        {
            type = filetype::pipefifo;
        }
        else if (S_ISREG(mode))
        {
            type = filetype::file;
        }
        else if (S_ISLNK(mode))
        {
            type = filetype::symlink;
        }
        else if (S_ISSOCK(mode))
        {
            type = filetype::socket;
        }
        return type;
    }

    filevec list_sub_files(const std::string &dirpath, filetype type, bool recursive)
    {
        filevec file_list;

        DIR *dir = opendir(dirpath.c_str()); // 打开这个目录
        if (dir == NULL)
        {
            CUTL_ERROR("opendir error. dirpath:" + dirpath + ", error:" + strerror(errno));
            return file_list;
        }
        struct dirent *file_info = NULL;
        // 逐个读取目录中的文件到file_info
        while ((file_info = readdir(dir)) != NULL)
        {
            // 系统有个系统文件，名为“..”和“.”,对它不做处理
            std::string filename(file_info->d_name);
            if (is_special_dir(filename))
            {
                continue;
            }
            struct stat file_stat; // 文件的信息
            std::string filepath = dirpath + "/" + filename;
            int ret = stat(filepath.c_str(), &file_stat);
            if (0 != ret)
            {
                CUTL_ERROR("stat error. filepath:" + filepath + ", error:" + strerror(errno));
                return file_list;
            }
            auto ftype = get_file_type(file_stat.st_mode);
            if (ftype & type)
            {
                file_entity entity;
                entity.type = ftype;
                entity.filepath = filepath;
                file_list.emplace_back(entity);
            }

            if (S_ISDIR(file_stat.st_mode) && recursive)
            {
                auto sub_files = list_sub_files(filepath, type, recursive);
                if (!sub_files.empty())
                {
                    file_list.insert(file_list.end(), sub_files.begin(), sub_files.end());
                }
            }
        }
        closedir(dir);

        return file_list;
    }

} // namespace cutl

#endif // defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)