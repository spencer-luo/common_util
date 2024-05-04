// #if defined(_WIN32) || defined(__WIN32__)

#include <io.h>
#include <direct.h>
#include <Windows.h>
#include <stdlib.h>
#include "strutil.h"
#include "filesystem.h"
#include "logger.h"

namespace cutl
{

    static constexpr char win_separator = '\\';

    bool is_special_dir(const std::string &filePath)
    {
        return filePath == "." || filePath == "..";
    }

    // https : // learn.microsoft.com/zh-cn/cpp/c-runtime-library/reference/getcwd-wgetcwd?view=msvc-170
    std::string current_program_dir()
    {
        char buffer[MAX_PATH_LEN] = {0};
        char *presult = _getcwd(buffer, MAX_PATH_LEN);
        if (nullptr == presult)
        {
            CUTL_ERROR("getcwd failure, presult is nullptr");
            return "";
        }
        return std::string(presult);
    }

    // 相对路径转绝对路径
    // https://learn.microsoft.com/zh-cn/cpp/c-runtime-library/reference/fullpath-wfullpath?view=msvc-170
    std::string absolute_path(const std::string &releative_path)
    {
        if (starts_with(releative_path, "~"))
        {
            // _fullpath 无法处理~开头的路径
            std::string home_dir;
            const char *text = std::getenv("USERPROFILE");
            if (text != nullptr)
            {
                home_dir = std::string(text);
                return home_dir + releative_path.substr(1);
            }
            else
            {
                CUTL_ERROR("getenv USERPROFILE failure, text is nullptr");
                return releative_path;
            }
        }
        else
        {
            char absPath[MAX_PATH_LEN] = {0};
            auto pAbsolutePath = _fullpath(absPath, releative_path.c_str(), MAX_PATH_LEN);
            if (pAbsolutePath == nullptr)
            {
                CUTL_ERROR("_fullpath failure, pAbsolutePath is nullptr");
                return "";
            }
            return std::string(pAbsolutePath);
        }

        return releative_path;
    }

    // https://learn.microsoft.com/zh-cn/cpp/c-runtime-library/reference/access-waccess?view=msvc-170
    bool file_exists(const std::string &filepath)
    {
        return (_access(filepath.c_str(), 0) == 0);
    }

    // https://learn.microsoft.com/zh-cn/cpp/c-runtime-library/reference/access-waccess?view=msvc-170
    bool file_readable(const std::string &filepath)
    {
        // 04: 只读, 06: 读取和写入
        return (_access(filepath.c_str(), 4) == 0) || (_access(filepath.c_str(), 6) == 0);
    }

    bool file_writable(const std::string &filepath)
    {
        // 02: 只写, 06: 读取和写入
        return (_access(filepath.c_str(), 2) == 0) || (_access(filepath.c_str(), 6) == 0);
    }

    bool file_executable(const std::string &filepath)
    {
        CUTL_WARN("executable() is not supported on Windows");
        return false;
    }

    std::string file_readlink(const std::string &filepath)
    {
        CUTL_ERROR("file_readlink() is not supported on Windows");
        return "";
    }

    bool file_createlink(const std::string &referenece, const std::string &filepath)
    {
        CUTL_ERROR("file_createlink() is not supported on Windows");
        return false;
    }

    // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/rmdir-wrmdir?view=msvc-170
    bool create_dir(const std::string &dir_path)
    {
        if (_mkdir(dir_path.c_str()) != 0)
        {
            CUTL_ERROR("mkdir error. dir_path:" + dir_path + ", error:" + strerror(errno));
            return false;
        }
        return true;
    }

    // remove empty directory
    // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/rmdir-wrmdir?view=msvc-170
    bool remove_dir(const std::string &dir_path)
    {
        if (_rmdir(dir_path.c_str()) != 0)
        {
            CUTL_ERROR("rmdir error. dir_path:" + dir_path + ", error:" + strerror(errno));
            return false;
        }
        return true;
    }

    // remove directory recursively
    bool remove_dir_recursive(const std::string &dir_path)
    {
        auto findpath = dir_path + win_separator + "*.*";
        WIN32_FIND_DATAA data = {0};
        HANDLE hFind = FindFirstFileA(findpath.c_str(), &data);
        bool unicode = true;
        if (hFind == INVALID_HANDLE_VALUE || hFind == NULL)
        {
            CUTL_ERROR("FindFirstFileA failed for " + findpath + ", errCode: " + std::to_string(GetLastError()));
            return false;
        }

        do
        {
            auto dwAttrs = data.dwFileAttributes;
            auto filename = std::string(data.cFileName);
            if (is_special_dir(filename))
            {
                // “..”和“.”不做处理
                continue;
            }
            std::string filepath = dir_path + win_separator + filename;
            if ((dwAttrs & FILE_ATTRIBUTE_DIRECTORY))
            {
                // directory
                if (!remove_dir_recursive(filepath))
                {
                    FindClose(hFind);
                    return false;
                }
            }
            else
            {
                // file
                int ret = remove(filepath.c_str());
                if (ret != 0)
                {
                    CUTL_ERROR("remove " + filepath + " error, ret:" + std::to_string(ret));
                    return false;
                }
            }
        } while (FindNextFileA(hFind, &data));
        // 关闭句柄
        FindClose(hFind);

        // 删除当前文件夹
        if (_rmdir(dir_path.c_str()) != 0)
        {
            CUTL_ERROR("rmdir error. dir_path:" + dir_path + ", error:" + strerror(errno));
            return false;
        }

        return true;
    }

    // get file size in bytes
    uint64_t get_filesize(const std::string &filepath, bool link_target)
    {
        struct stat statbuf;
        int ret = stat(filepath.c_str(), &statbuf);
        if (ret != 0)
        {
            CUTL_ERROR("stat " + filepath + " error, ret:" + std::to_string(ret));
            return 0;
        }

        return static_cast<uint64_t>(statbuf.st_size);
    }

    // get directory size in bytes
    uint64_t get_dirsize(const std::string &dirpath)
    {
        uint64_t totalSize = 0;

        auto findpath = dirpath + win_separator + "*.*";
        WIN32_FIND_DATAA data = {0};
        HANDLE hFind = FindFirstFileA(findpath.c_str(), &data);
        bool unicode = true;
        if (hFind == INVALID_HANDLE_VALUE || hFind == NULL)
        {
            CUTL_ERROR("FindFirstFileA failed for " + findpath + ", errCode: " + std::to_string(GetLastError()));
            return totalSize;
        }

        do
        {
            auto dwAttrs = data.dwFileAttributes;
            auto filename = std::string(data.cFileName);
            if (is_special_dir(filename))
            {
                // “..”和“.”不做处理
                continue;
            }
            std::string filepath = dirpath + win_separator + filename;
            if ((dwAttrs & FILE_ATTRIBUTE_DIRECTORY))
            {
                // directory
                auto subdir_size = get_dirsize(filepath);
                totalSize += subdir_size;
            }
            else
            {
                // file
                auto subfile_size = get_filesize(filepath, false);
                totalSize += subfile_size;
            }
        } while (FindNextFileA(hFind, &data));
        // 关闭句柄
        FindClose(hFind);

        return totalSize;
    }

    filetype get_file_type(DWORD attributes, const std::string &extension)
    {
        filetype type = filetype::unknown;

        if (attributes == INVALID_FILE_ATTRIBUTES)
        {
            CUTL_WARN("Failed to get file attributes, error code: " + std::to_string(GetLastError()));
            if (extension == ".lnk")
            {
                // 注意：测试时发现，有些快捷方式访问会失败，用后缀名判断进行兜底
                type = filetype::symlink;
            }
            return type;
        }
        else
        {
            if (attributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                type = filetype::directory; // directory
            }
            else if (attributes & FILE_ATTRIBUTE_NORMAL)
            {
                type = filetype::file; // regular file
            }
            else if ((attributes & FILE_ATTRIBUTE_ARCHIVE) && extension == ".lnk")
            {
                // windows的快捷方式
                type = filetype::symlink; // symbolic link
            }
        }

        return type;
    }

    std::string get_file_extension(const std::string &filepath)
    {
        auto pos = filepath.find_last_of('.');
        std::string extension = "";
        if (pos != std::string::npos)
        {
            extension = filepath.substr(pos);
            extension = cutl::to_lower(extension);
        }
        return extension;
    }

    filetype get_file_type(const std::string &filepath)
    {
        auto attributes = GetFileAttributesA(filepath.c_str());
        auto extension = get_file_extension(filepath);
        // CUTL_DEBUG(filepath + ", extension: " + extension + ", attributes: " + std::to_string(attributes));
        return get_file_type(attributes, extension);
    }

    filevec list_sub_files(const std::string &dirpath, filetype type, bool recursive)
    {
        filevec file_list;

        // 使用FindFirstFileA查找第一个文件
        auto findpath = dirpath + win_separator + "*.*";
        WIN32_FIND_DATAA findData = {0};
        HANDLE hFind = FindFirstFileA(findpath.c_str(), &findData);
        if (hFind == INVALID_HANDLE_VALUE || hFind == NULL)
        {
            CUTL_ERROR("FindFirstFileA failed for " + findpath + ", errCode: " + std::to_string(GetLastError()));
            return file_list;
        }

        do
        {
            auto dwAttrs = findData.dwFileAttributes;
            auto filename = std::string(findData.cFileName);
            CUTL_DEBUG(filename + ", attributes: " + std::to_string(dwAttrs));
            if (is_special_dir(filename))
            {
                // “..”和“.”不做处理
                continue;
            }
            std::string filepath = dirpath + win_separator + filename;
            auto extension = get_file_extension(filename);
            auto ftype = get_file_type(dwAttrs, extension);
            if (ftype & type)
            {
                file_entity entity;
                entity.type = ftype;
                entity.filepath = filepath;
                file_list.emplace_back(entity);
            }
            if ((dwAttrs & FILE_ATTRIBUTE_DIRECTORY) && recursive)
            {
                // directory
                auto sub_files = list_sub_files(filepath, type, recursive);
                if (!sub_files.empty())
                {
                    file_list.insert(file_list.end(), sub_files.begin(), sub_files.end());
                }
            }
        } while (FindNextFileA(hFind, &findData));
        // 关闭句柄
        FindClose(hFind);

        return file_list;
    }

    // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-setfileattributesa
    // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-setfileattributesw
    // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfiletime
    // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-setfiletime
    bool copy_attributes(const std::string &srcpath, const std::string &dstpath)
    {
        // 修改文件属性
        DWORD attributes = GetFileAttributesA(srcpath.c_str());
        if (attributes == INVALID_FILE_ATTRIBUTES)
        {
            CUTL_ERROR("Failed to get file attributes for " + srcpath + ", error code: " + std::to_string(GetLastError()));
            return false;
        }
        if (!SetFileAttributesA(dstpath.c_str(), attributes))
        {
            CUTL_ERROR("Failed to set file attributes for " + dstpath + ", error code: " + std::to_string(GetLastError()));
            return false;
        }

        // 获取 修改访问、修改时间
        FILETIME t_create, t_access, t_write;
        HANDLE h_src = CreateFileA(
            srcpath.c_str(),
            GENERIC_READ,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS,
            NULL);
        if (h_src == INVALID_HANDLE_VALUE)
        {
            CUTL_ERROR("Failed to open file " + srcpath + ", error code: " + std::to_string(GetLastError()));
            CloseHandle(h_src);
            return false;
        }
        if (!GetFileTime(h_src, &t_create, &t_access, &t_write))
        {
            CUTL_ERROR("Failed to get file times for " + srcpath + ", error code: " + std::to_string(GetLastError()));
            CloseHandle(h_src);
            return false;
        }
        CloseHandle(h_src);

        // 设置 修改访问、修改时间
        HANDLE h_dst = ::CreateFile(
            dstpath.c_str(),
            GENERIC_WRITE | GENERIC_READ,
            0,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
        if (h_dst == INVALID_HANDLE_VALUE)
        {
            CUTL_ERROR("Failed to open file " + dstpath + ", error code: " + std::to_string(GetLastError()));
            CloseHandle(h_dst);
            return false;
        }
        if (!SetFileTime(h_dst, &t_create, &t_access, &t_write))
        {
            CUTL_ERROR("Failed to set file times for " + dstpath + ", error code: " + std::to_string(GetLastError()));
            CloseHandle(h_dst);
            return false;
        }
        CloseHandle(h_dst);

        return true;
    }

    bool file_sync(FILE *handle)
    {
        return true;
    }

} // namespace cutl

// #endif // defined(_WIN32) || defined(__WIN32__)