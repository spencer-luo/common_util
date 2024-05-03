#if defined(_WIN32) || defined(__WIN32__)

#include <io.h>
#include <Windows.h>
#include "strutil.h"
#include "filesystem.h"
#include "logger.h"

namespace cutl
{

    static constexpr char win_separator = '\\';

    std::string current_program_dir()
    {
        // todo: implement this
        return "";
    }

    bool file_exists(const std::string &filepath)
    {
        return (_access(filepath.c_str(), 0) == 0);
    }

    bool file_readable(const std::string &filepath)
    {
        return (_access(filepath.c_str(), 0x100) == 0);
    }

    bool file_writable(const std::string &filepath)
    {
        return (_access(filepath.c_str(), 0x10) == 0);
    }

    bool file_executable(const std::string &filepath)
    {
        CUTL_WARN("executable() is not supported on Windows");
        return false;
    }

    std::string file_readlink(const std::string &filepath)
    {
        // todo: implement this
        return "";
    }

    bool file_createlink(const std::string &referenece, const std::string &filepath)
    {
        // todo: implement this
        return true;
    }

    bool create_dir(const std::string &dir_path)
    {
        // todo: implement this
        return true;
    }
    // remove empty directory
    bool remove_dir(const std::string &dir_path)
    {
        // todo: implement this
        return true;
    }
    // remove directory recursively
    bool remove_dir_recursive(const std::string &dir_path)
    {
        // todo: implement this
        return true;
    }

    bool file_sync(FILE *handle)
    {
        return true;
    }

    // get directory size in bytes
    uint64_t get_dirsize(const std::string &dirpath)
    {
        // todo: implement this
        return 0;
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

    bool is_special_dir(const std::string &filePath)
    {
        return filePath == "." || filePath == "..";
    }

    filevec list_sub_files(const std::string &dirpath, filetype type, bool recursive)
    {
        filevec file_list;

        // 使用FindFirstFileA查找第一个文件
        auto findpath = dirpath + win_separator + "*.*";
        WIN32_FIND_DATAA findData = {0};
        HANDLE hFind = FindFirstFileA(findpath.c_str(), &findData);
        if (hFind == NULL || hFind == INVALID_HANDLE_VALUE)
        {
            CUTL_ERROR("FindFirstFileA failed for " + findpath + ", errCode: " + std::to_string(GetLastError()));
            return file_list;
        }

        do
        {
            auto dwAttrs = findData.dwFileAttributes;
            auto filename = std::string(findData.cFileName);
            CUTL_DEBUG(filename + ", attributes: " + std::to_string(dwAttrs));
            // “..”和“.”不做处理
            if (is_special_dir(filename))
            {
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

    bool copy_attributes(const std::string &srcpath, const std::string &dstpath)
    {
        // todo: implement this
        return true;
    }

} // namespace cutl

#endif // defined(_WIN32) || defined(__WIN32__)