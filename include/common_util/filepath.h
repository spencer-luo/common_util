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
 * @file filepath.h
 * @brief The class for file path operations.
 * @author spencer.luo
 * @date 2024-05-13
 */

#pragma once

#include "filetype.h"
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

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
         * @brief Get the path separator of the current os platform.
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
         * @brief Get the real path referenced by symbolic link
         *
         * @note This function only works on Unix-like systems, not support on Windows.
         *
         * @return real path referenced by symbolic link or shortcuts
         */
        std::string realpath() const;
        /**
         * @brief Get the absolute path of the filepath.
         *
         * @return std::string the absolute path of the filepath.
         */
        std::string abspath() const;
        /**
         * @brief Get the extension of the filepath.
         *
         * @param dot_number: It represents the number of dots in the file extension. For example,
         * for '.md', it is 1; for '.tar.gz', it is 2; and for '.min.js.gz', it is 3.
         * @return extension with dot
         */
        std::string extension(uint8_t dot_number = 1) const;

        /**
         * @brief Replace the file extension with the new one and return the new path
         *
         * @param new_extension: The new string of the file extension to be replaced.
         * @param dot_number: The number of dots in the original file extension. For example, it is
         * 1 for '.md', 2 for '.tar.gz', and 3 for '.min.js.gz'.
         * @return std::string The new path
         */
        std::string replace_extension(const std::string& new_extension,
                                      uint8_t dot_number = 1) const;

    private:
        std::string filepath_;
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

} // namespace cutl