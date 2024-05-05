/**
 * @file fileutil.h
 * @brief The functions and classes about filesystem, such as file and directory operations.
 * @author spencer.luo
 * @date 2024-04-23
 */

#pragma once

#include <string>
#include <cstdio>
#include "filetype.h"
#include "filepath.h"

namespace cutl
{

    /**
     * @brief The file guard class to manage the FILE pointer automatically.
     * file_guard object can close the FILE pointer automatically when his scope is exit.
     */
    class file_guard
    {
    public:
        /**
         * @brief Construct a new file guard object
         *
         * @param file the pointer of the FILE object
         */
        explicit file_guard(FILE *file);

        /**
         * @brief Destroy the file guard object
         *
         */
        ~file_guard();

        /**
         * @brief Get the FILE pointer.
         *
         * @return FILE*
         */
        FILE *getfd() const;

    private:
        FILE *file_;
    };

    /**
     * @brief Get the file type flag string.
     *
     * @param type file type
     * @return the flag string of the file type
     */
    std::string filetype_flag(filetype type);

    /**
     * @brief Get the working directory of the current excuting process.
     *
     * @return file path of the working directory
     */
    std::string getcwd();

    /**
     * @brief Create a new file.
     *
     * @param path the filepath of the new file to be created
     * @return true if the file is created successfully, false otherwise.
     */
    bool createfile(const filepath &path);

    /**
     * @brief Create a symbolic link or shortcut.
     *
     * @param referenece the real path referenced by the symbolic link or shortcut
     * @param filepath the filepath of the symbolic link or shortcut to be created
     * @return true if the symbolic link or shortcut is created successfully, false otherwise.
     */
    bool createlink(const filepath &referenece, const filepath &filepath);

    // recursive: create parent directories if not exist, like the 'mkdir -p' command
    /**
     * @brief Create a new directory.
     *
     * @param path the filepath of the new directory to be created
     * @param recursive whether to create parent directories, default is false.
     * If true, means create parent directories if not exist, like the 'mkdir -p' command.
     * @return true if the directory is created successfully, false otherwise.
     */
    bool createdir(const filepath &path, bool recursive = false);

    /**
     * @brief Remove a regular file or symbolic link(shortcut on windows).
     *
     * @param path the filepath of the file or symbolic link to be removed
     * @return true if the file or symbolic link is removed successfully, false otherwise.
     */
    bool removefile(const filepath &path);

    /**
     * @brief Remove a directory.
     *
     * @param path the filepath of the directory to be removed
     * @param recursive whether to remove the whole directory recursively, default is false.
     * If true, means remove the whole directory recursively, like the 'rm -rf' command.
     * @return true if the directory is removed successfully, false otherwise.
     */
    bool removedir(const filepath &path, bool recursive = false);

    /**
     * @brief Read the text content of a file.
     *
     * @param path the filepath of the file to be read
     * @param max_read_size the maximum size to be read, default is 4096 bytes.
     *
     * If the file size is larger than max_read_size, only the first max_read_size bytes will be read. otherwise, the whole file will be read.
     *
     * @return text content of the file
     */
    std::string readtext(const filepath &path, uint64_t max_read_size = 4096);

    /**
     * @brief Write the text content to a file.
     *
     * @param path the filepath of the file to be written
     * @param content text content to be written to the file
     * @return true if the content is written successfully, false otherwise.
     */
    bool writetext(const filepath &path, const std::string &content);

    /**
     * @brief Get the size of a file.
     *
     * @param filepath the filepath of the file to be checked
     *
     * @param link_target whether to get the size of the file pointed by symbolic link, default is false.
     * If link_target is true, the function will get the size of the file pointed by symbolic link, not the symbolic link itself.
     *
     * @note link_target parameter only works on Unix-like systems, not support on Windows.
     *
     * @return file size in bytes
     */
    uint64_t filesize(const filepath &filepath, bool link_target = false);

    /**
     * @brief Get the size of a directory, include all files and subdirectories.
     *
     * @param dirpath the filepath of the directory to be checked
     * @return the total size of the directory in bytes
     */
    uint64_t dirsize(const filepath &dirpath);

    /**
     * @brief List all files in a directory.
     *
     * @param dirpath the filepath of the directory to be listed
     * @param type file type to be listed, default is all types.
     * @param recursive whether to list the whole directory recursively, default is false.
     * If true, means list the whole directory recursively, like the 'ls -R' command.
     * @return filevec the vector of file_entity, filepaths in the directory.
     */
    filevec list_files(const filepath &dirpath, filetype type = filetype::all, bool recursive = false);

    /**
     * @brief Find all files in a directory by name.
     *
     * @param dirpath the filepath of the directory to be searched
     * @param name the name of the file to be found, substring match is supported.
     * @param recursive whether to search the whole directory recursively, default is false.
     * If true, means search the whole directory recursively, like the 'find' command.
     * @return filevec the vector of file_entity, filepaths in the directory.
     */
    filevec find_files(const filepath &dirpath, const std::string &name, bool recursive = false);

    /**
     * @brief Find all files in a directory by extension.
     *
     * @param dirpath the filepath of the directory to be searched
     * @param extension the extension of the file to be found, with dot, like ".txt".
     * @param recursive whether to search the whole directory recursively, default is false.
     * If true, means search the whole directory recursively, like the 'find' command.
     * @return filevec the vector of file_entity, filepaths in the directory.
     */
    filevec find_files_by_extension(const filepath &dirpath, const std::string &extension, bool recursive = false);

    /**
     * @brief Copy a file or symbolic link(shortcut on windows).
     *
     * @param srcpath the filepath of the source file or symbolic link to be copied
     * @param dstpath the filepath of the destination file or symbolic link to be copied to
     * @param attributes whether to copy the file attributes, default is false.
     * If true, means copy the file attributes, like the 'cp -p' command.
     * @return true if the file or symbolic link is copied successfully, false otherwise.
     * @note If the destination file or directory already exists, it will be overwritten.
     * @return false
     */
    bool copyfile(const filepath &srcpath, const filepath &dstpath, bool attributes = false);

    // copy directory recursively
    /**
     * @brief Copy a directory recursively.
     *
     * @param srcdir the filepath of the source directory to be copied
     * @param dstdir the filepath of the destination directory to be copied to
     * @return true if the directory is copied successfully, false otherwise.
     * @note If the destination directory already exists, it will be overwritten.
     */
    bool copydir(const filepath &srcdir, const filepath &dstdir);

} // namespace cutl