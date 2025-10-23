#if defined(_WIN32)
// do nothing
#else

#include "filesystem.h"
#include "inner/logger.h"
#include "timeutil.h"
#include <cstring>
#include <dirent.h>
#include <stack>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <utime.h>

namespace cutl
{
    static constexpr char unix_separator = '/';

    bool is_special_dir(const std::string &filePath)
    {
        return filePath == "." || filePath == "..";
    }

    // https://blog.csdn.net/giveaname/article/details/88973102
    std::string current_program_dir()
    {
        char buffer[MAX_PATH_LEN] = {0};
        char *presult = getcwd(buffer, MAX_PATH_LEN);
        if (nullptr == presult)
        {
            CUTL_ERROR("getcwd failure, presult is nullptr");
            return "";
        }
        return std::string(presult);
    }

    // 相对路径转绝对路径
    // https://www.man7.org/linux/man-pages/man3/realpath.3.html
    std::string absolute_path(const std::string &releative_path)
    {
        char absPath[PATH_MAX] = {0};
        auto pAbsolutePath = realpath(releative_path.c_str(), absPath);
        if (pAbsolutePath == nullptr)
        {
            CUTL_WARN("realpath failure for " + releative_path + ", pAbsolutePath is nullptr, absPath:" + absPath);
            return std::string(absPath);
        }

        return std::string(absPath);
    }

    bool file_exists(const std::string &filepath)
    {
        return (access(filepath.c_str(), 0) == 0);
    }

    bool file_readable(const std::string &filepath)
    {
        return (access(filepath.c_str(), R_OK) == 0);
    }

    bool file_writable(const std::string &filepath)
    {
        return (access(filepath.c_str(), W_OK) == 0);
    }

    bool file_executable(const std::string &filepath)
    {
        return (access(filepath.c_str(), X_OK) == 0);
    }

    std::string file_readlink(const std::string &filepath)
    {
        char buffer[MAX_PATH_LEN] = {0};
        ssize_t len = ::readlink(filepath.c_str(), buffer, MAX_PATH_LEN);
        if (len < 0)
        {
            CUTL_ERROR("readlink error. filepath:" + filepath + ", error:" + strerror(errno));
            return "";
        }
        return std::string(buffer, len);
    }

    bool file_createlink(const std::string &referenece, const std::string &filepath)
    {
        int ret = ::symlink(referenece.c_str(), filepath.c_str());
        if (ret != 0)
        {
            CUTL_ERROR("symlink error. filepath:" + filepath + ", referenece:" + referenece + ", error:" + strerror(errno));
            return false;
        }
        return true;
    }

    bool file_removelink(const std::string &filepath)
    {
        int ret = ::unlink(filepath.c_str());
        if (ret != 0)
        {
            CUTL_ERROR("unlink error. filepath:" + filepath + ", error:" + strerror(errno));
            return false;
        }
        return true;
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
            std::string filepath = dir_path + unix_separator + filename;
            int ret = lstat(filepath.c_str(), &file_stat);
            if (0 != ret)
            {
                CUTL_ERROR("stat error. filepath:" + filepath + ", error:" + strerror(errno));
                closedir(dir);
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
                    closedir(dir);
                    return false;
                }
            }
        }
        closedir(dir);

        // 删除当前文件夹
        int ret = rmdir(dir_path.c_str());
        if (ret != 0)
        {
            CUTL_ERROR("rmdir error. dir_path:" + dir_path + ", error:" + strerror(errno));
            return false;
        }

        return true;
    }

    uint64_t get_filesize(const std::string &filepath, bool link_target)
    {
        struct stat statbuf;
        int ret = 0;
        if (link_target)
        {
            ret = stat(filepath.c_str(), &statbuf);
        }
        else
        {
            ret = lstat(filepath.c_str(), &statbuf);
        }

        if (ret != 0)
        {
            CUTL_ERROR("stat " + filepath + " error, ret:" + std::to_string(ret));
            return 0;
        }

        return static_cast<uint64_t>(statbuf.st_size);
    }

    filetype get_file_type(int mode)
    {
        filetype type = filetype::ft_unknown;
        if (S_ISBLK(mode))
        {
            type = filetype::ft_block_special;
        }
        else if (S_ISCHR(mode))
        {
            type = filetype::ft_char_special;
        }
        else if (S_ISDIR(mode))
        {
            type = filetype::ft_directory;
        }
        else if (S_ISFIFO(mode))
        {
            type = filetype::ft_pipefifo;
        }

        else if (S_ISLNK(mode))
        {
            type = filetype::ft_symlink;
        }
        else if (S_ISREG(mode))
        {
            type = filetype::ft_file;
        }
        else if (S_ISSOCK(mode))
        {
            type = filetype::ft_socket;
        }
        return type;
    }

    uint64_t get_dirsize(const std::string &dirpath)
    {
        uint64_t totalSize = 0;
        // filevec file_list;

        DIR *dir = opendir(dirpath.c_str()); // 打开这个目录
        if (dir == NULL)
        {
            CUTL_ERROR("opendir error. dirpath:" + dirpath + ", error:" + strerror(errno));
            return totalSize;
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
            std::string filepath = dirpath + unix_separator + filename;
            int ret = lstat(filepath.c_str(), &file_stat);
            if (0 != ret)
            {
                CUTL_ERROR("stat error. filepath:" + filepath + ", error:" + strerror(errno));
                closedir(dir);
                return totalSize;
            }

            if (S_ISDIR(file_stat.st_mode))
            {
                auto subdir_size = get_dirsize(filepath);
                totalSize += subdir_size;
            }
            else
            {
                auto subfile_size = get_filesize(filepath, false);
                totalSize += subfile_size;
            }
        }
        closedir(dir);

        return totalSize;
    }

    filetype get_file_type(const std::string &filepath)
    {
        struct stat file_stat;
        int ret = lstat(filepath.c_str(), &file_stat);
        if (0 != ret)
        {
            CUTL_ERROR("stat error. filepath:" + filepath + ", error:" + strerror(errno));
            return filetype::ft_unknown;
        }

        return get_file_type(file_stat.st_mode);
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
            std::string filepath = dirpath + unix_separator + filename;
            int ret = lstat(filepath.c_str(), &file_stat);
            if (0 != ret)
            {
                CUTL_ERROR("stat error. filepath:" + filepath + ", error:" + strerror(errno));
                closedir(dir);
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

    bool copy_attributes(const std::string &srcpath, const std::string &dstpath, bool isdir)
    {
        struct stat attr_of_src;
        int ret = lstat(srcpath.c_str(), &attr_of_src);
        if (ret != 0)
        {
            CUTL_ERROR("lstat error. srcpath:" + srcpath + ", error:" + strerror(errno));
            return false;
        }

        // 修改文件属性
        ret = chmod(dstpath.c_str(), attr_of_src.st_mode);
        if (ret != 0)
        {
            CUTL_ERROR("chmod error. dstpath:" + dstpath + ", error:" + strerror(errno));
            return false;
        }
        // 修改文件用户组
        ret = chown(dstpath.c_str(), attr_of_src.st_uid, attr_of_src.st_gid);
        if (ret != 0)
        {
            CUTL_ERROR("chown error. dstpath:" + dstpath + ", error:" + strerror(errno));
            return false;
        }

        // 修改文件访问、修改时间
        if (S_ISLNK(attr_of_src.st_mode))
        {
            // TODO: 编译还有问题，需要确定编译宏
            // struct timeval time_buf[2];
            // time_buf[0].tv_sec = attr_of_src.st_atim.tv_sec;
            // time_buf[0].tv_usec = attr_of_src.st_atim.tv_nsec / 1000;
            // time_buf[1].tv_sec = attr_of_src.st_mtim.tv_sec;
            // time_buf[1].tv_usec = attr_of_src.st_mtim.tv_nsec / 1000;
            // ret = lutimes(dstpath.c_str(), time_buf);
            // if (ret != 0)
            // {
            //     CUTL_ERROR("lutimes error. dstpath:" + dstpath + ", error:" + strerror(errno));
            //     return false;
            // }
        }
        else
        {
            struct utimbuf tbuf;
            tbuf.actime = attr_of_src.st_atime;
            tbuf.modtime = attr_of_src.st_mtime;
            ret = utime(dstpath.c_str(), &tbuf);
            if (ret != 0)
            {
                CUTL_ERROR("utime error. dstpath:" + dstpath + ", error:" + strerror(errno));
                return false;
            }
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

    uint64_t get_last_modified_time_s(const std::string& filepath)
    {
        // Linux/macOS实现
        struct stat st;
        int ret = stat(filepath.c_str(), &st);
        if (ret != 0)
        {
            CUTL_ERROR("Get last modified time failed for " + filepath +
                       " ret:" + std::to_string(ret));
            return timestamp(timeunit::s);
        }

        // 时间精确到秒
        return static_cast<uint64_t>(st.st_mtime);
    }

} // namespace cutl

#endif // defined(_WIN32)