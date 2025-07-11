/**
 * @copyright Copyright (c) 2024, Spencer.Luo. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations.
 *
 * @file fileutil.cpp
 * @brief
 * @author spencer.luo
 * @date 2024-05-13
 */

#include <cstdio>
#include <map>
#include <iostream>
#include <cstring>
#include <sys/stat.h>
#include "fileutil.h"
#include "inner/logger.h"
#include "inner/filesystem.h"
#include "strutil.h"

namespace cutl
{

    std::string filetype_flag(filetype type)
    {
        static std::map<filetype, std::string> flag_map = {
            { filetype::ft_unknown, "u" },   { filetype::ft_directory, "d" },
            { filetype::ft_file, "-" },      { filetype::ft_symlink, "l" },
            { filetype::ft_char_special, "c" }, { filetype::ft_block_special, "b" },
            { filetype::ft_pipefifo, "p" },     { filetype::ft_socket, "s" },
            { filetype::ft_all, "a" },
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
            // CUTL_DEBUG("close file");
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

    bool renamefile(const filepath& oldfile, const filepath& newfile)
    {
        if (!oldfile.exists())
        {
            CUTL_ERROR(oldfile.str() + "is not exist.");
            return false;
        }

        if (newfile.exists())
        {
            CUTL_ERROR("Already exist target file: " + oldfile.str());
            return false;
        }

        int ret = rename(oldfile.str().c_str(), newfile.str().c_str());
        if (ret != 0)
        {
            CUTL_ERROR("rename " + oldfile.str() + " error, ret:" + std::to_string(ret));
            return false;
        }
        return true;
    }

    uint64_t last_modified_time(const filepath& path)
    {
        return get_last_modified_time_s(path.str());
    }

    std::string readtext(const filepath &path, uint64_t max_read_size)
    {
        file_guard fg(fopen(path.str().c_str(), "r"));
        if (fg.getfd() == nullptr)
        {
            CUTL_ERROR("open file failed for " + path.str());
            return "";
        }

        // 从文件系统的文档信息里读取文件大小，性能更高
        auto data_len = filesize(path, true);
        // 公国文件操作的方式和获取文件大小
        // fseek(fg.getfd(), 0, SEEK_END);
        // size_t data_len = static_cast<size_t>(ftell(fg.getfd()));
        // rewind(fg.getfd());
        CUTL_DEBUG("file size: " + std::to_string(data_len) + ", file: " + path.str());

        // get read size
        if (data_len > max_read_size)
        {
            data_len = max_read_size;
            CUTL_WARN("file size is large than " + std::to_string(max_read_size) + ", file:" + path.str());
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

        buffer[read_len] = '\0';
        auto text = std::string(buffer);
        delete[] buffer;

        return text;
    }

    // https://en.cppreference.com/w/cpp/header/cstdio
    bool writetext(const filepath &path, const std::string &content)
    {
        // std::lock_guard
        file_guard fg(fopen(path.str().c_str(), "w"));
        if (fg.getfd() == nullptr)
        {
            CUTL_ERROR("open file failed for " + path.str() + ", error: " + strerror(errno));
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

    uint64_t filesize(const filepath &filepath, bool link_target)
    {
        if (!filepath.exists())
        {
            CUTL_ERROR("filepath does not exist: " + filepath.str());
            return 0;
        }

        return get_filesize(filepath.str(), link_target);
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
        filevec filelist = list_files(dirpath, filetype::ft_all, recursive);
        filevec result;
        for (auto &file : filelist)
        {
            if (file.type == filetype::ft_directory)
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
        filevec filelist = list_files(dirpath, filetype::ft_all, recursive);
        filevec result;
        for (auto &file : filelist)
        {
            if (file.type == filetype::ft_directory)
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
        // CUTL_INFO("file type: " + std::to_string(srcpath.type()) + ", " + filetype_flag(srcpath.type()) + ", " + srcpath.str() + ", dstpath:" + dstpath.str());

        // copy file content
        if (srcpath.isfile())
        {
            if (dstpath.exists())
            {
                // remove if exists
                removefile(dstpath);
            }
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
            // flush file to disk
            int ret = fflush(fwt.getfd());
            if (0 != ret)
            {
                CUTL_ERROR("fail to flush file:" + dstpath.str());
                return false;
            }
            if (!file_sync(fwt.getfd()))
            {
                CUTL_ERROR("file_sync failed for " + dstpath.str());
                return false;
            }
        }
        else if (srcpath.issymlink())
        {
            if (dstpath.exists())
            {
                // remove if exists
                file_removelink(dstpath.str());
            }
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

        auto filelist = list_files(srcdir, filetype::ft_all, true);
        for (size_t i = 0; i < filelist.size(); i++)
        {
            auto file = filelist[i];
            auto src_file = file.filepath;
            auto reletive_path = src_file.substr(srcdir.str().length() + 1);
            auto dstpath = dstdir.join(reletive_path);
            auto srcpath = cutl::path(src_file);
            if (file.type == filetype::ft_file || file.type == filetype::ft_symlink)
            {
                if (!copyfile(srcpath, dstpath, true))
                {
                    return false;
                }
            }
            else if (file.type == filetype::ft_directory)
            {
                if (!createdir(dstpath, true))
                {
                    return false;
                }

                if (!copy_attributes(src_file, dstpath.str(), true))
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