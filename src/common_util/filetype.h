/**
 * @file filetype.h
 * @brief Define the structs about fileutil, such as filetype and file_entity.
 * @author spencer.luo
 * @date 2024-04-23
 */

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
        unknown = 0x00,
        /** directory */
        directory = 0x01,
        /** regular file */
        file = 0x02,
        /** symbolic link */
        symlink = 0x04,
        /** character device, only for unix */
        char_special = 0x08,
        /** block device, only for unix */
        block_special = 0x10,
        /** named pipe, only for unix */
        pipefifo = 0x20,
        /** socket file, only for unix */
        socket = 0x40,
        /** type mask, includes all types */
        all = 0xFF,
    };

    /**
     * @brief file entity struct. include file type and file path.
     *
     */
    struct file_entity
    {
        /** file type */
        filetype type = {filetype::unknown};
        /** file path */
        std::string filepath;
    };

    /**
     * @brief The vector of file_entity.
     *
     */
    using filevec = std::vector<file_entity>;

} // namespace cutl