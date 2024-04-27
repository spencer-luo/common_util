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

namespace cutl
{

    /**
     * @brief The class for file path operations.
     *
     */
    class filepath
    {
    public:
        /**
         * @brief Construct a new filepath object
         *
         * @param path file path string
         */
        filepath(const std::string &path);

        /**
         * @brief Construct a new filepath object by copy
         *
         * @param other other filepath object
         */
        filepath(const filepath &other);

        /**
         * @brief Assign operator, assign a new filepath object by copy
         *
         * @param other other filepath object
         * @return filepath& the reference of the current filepath object
         */
        filepath &operator=(const filepath &other);

        /**
         * @brief Destroy the filepath object
         *
         */
        ~filepath() = default;

    public:
        /**
         * @brief Get the path separator of the current platform.
         *
         * @return the path separator
         */
        static char separator();

        /**
         * @brief Get the string of the filepath.
         *
         * @return the filepath
         */
        std::string str() const;

        /**
         * @brief Join the current filepath with a new filename.
         *
         * @param filename the filename to be joined
         * @return the new filepath object
         */
        filepath join(const std::string &filename) const;

        // file is exist or not
        /**
         * @brief Check if the filepath is exists.
         *
         * @return true means the filepath is exists, otherwise not exists.
         */
        bool exists() const;

        /**
         * @brief Check if the filepath is readable.
         *
         * @return true means readable, false means not readable.
         */
        bool readable() const;

        /**
         * @brief Check if the filepath is writable.
         *
         * @return true menas writable, false means not writable.
         */
        bool writable() const;

        /**
         * @brief Check if the filepath is executable.
         *
         * @return true means executable, false means not executable.
         */
        bool executable() const;

        /**
         * @brief Get the file type of the filepath.
         *
         * @return file type
         */
        filetype type() const;

        /**
         * @brief Check if the filepath is a regular file.
         *
         * @return true mena regular file, false means not regular file.
         */
        bool isfile() const;

        /**
         * @brief Check if the filepath is a directory.
         *
         * @return true means directory, false means not directory.
         */
        bool isdir() const;

        /**
         * @brief Check if the filepath is a symbolic link.
         *
         * @return true means symbolic link, false means not symbolic link.
         */
        bool issymlink() const;

        /**
         * @brief Get the parent directory of the filepath.
         *
         * @return parent directory path
         */
        std::string dirname() const;

        /**
         * @brief Get the filename or directory name of the filepath.
         *
         * @return filename or directory name
         */
        std::string basename() const;
        /**
         * @brief Get the real path referenced by symbolic link or shortcuts.
         *
         * @return real path referenced by symbolic link or shortcuts
         */
        std::string realpath() const;
        /**
         * @brief Get the extension of the filepath.
         *
         * @return extension with dot
         */
        std::string extension() const;

    private:
        std::string filepath_;
    };

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
     * @brief Define the output stream operator for filepath object.
     *
     * @param os the std::ostream object
     * @param fp the filepath object to be output
     * @return std::ostream& the reference of the std::ostream object after outputing the filepath object.
     */
    std::ostream &operator<<(std::ostream &os, const filepath &fp);

    /**
     * @brief Create a filepath object from a string.
     *
     * @param path file path string
     * @return filepath object
     */
    filepath path(const std::string &path);

    /**
     * @brief Get the file type flag string.
     *
     * @param type file type
     * @return the flag string of the file type
     */
    std::string filetype_flag(filetype type);

    /**
     * TODO: should be return filepath instead of std::string
     *
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
     * @return file size in bytes
     */
    uint64_t filesize(const filepath &filepath);

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
     * TODO: should be overwritten if the destination file or directory already exists.
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
     * TODO: should be overwritten if the destination directory already exists.
     * @note If the destination directory already exists, it will be overwritten.
     */
    bool copydir(const filepath &srcdir, const filepath &dstdir);

} // namespace cutl