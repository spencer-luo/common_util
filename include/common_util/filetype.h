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
 * @file filetype.h
 * @brief Define the structs about fileutil, such as filetype and file_entity.
 * @author spencer.luo
 * @date 2024-05-13
 */

#include <vector>
#include <string>

#pragma once

namespace cutl
{

    /**
     * @brief Constants value: max path length.
     *
     */
    constexpr int MAX_PATH_LEN = 1024;

    /**
     * @brief The type of file.
     *
     */
    enum filetype
    {
        /** undefined */
        ft_unknown = 0x00,
        /** directory */
        ft_directory = 0x01,
        /** regular file */
        ft_file = 0x02,
        /** symbolic link */
        ft_symlink = 0x04,
        /** character device, only for unix */
        ft_char_special = 0x08,
        /** block device, only for unix */
        ft_block_special = 0x10,
        /** named pipe, only for unix */
        ft_pipefifo = 0x20,
        /** socket file, only for unix */
        ft_socket = 0x40,
        /** type mask, includes all types */
        ft_all = 0xFF,
    };

    /**
     * @brief file entity struct. include file type and file path.
     *
     */
    struct file_entity
    {
        /** file type */
        filetype type = { filetype::ft_unknown };
        /** file path */
        std::string filepath;
    };

    /**
     * @brief The vector of file_entity.
     *
     */
    using filevec = std::vector<file_entity>;

} // namespace cutl