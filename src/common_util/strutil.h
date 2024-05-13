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
 * @file strutil.h
 * @brief Common string utilities.
 * @author spencer.luo
 * @date 2024-05-13
 */

#pragma once

#include <string>
#include <vector>

namespace cutl
{
    /**
     * @brief The type of vector strings used in this library.
     *
     */
    using strvec = std::vector<std::string>;

    /**
    //  * @brief Convert a string to upper case.
     *
     * @param str the string to be converted.
     * @return std::string the converted string.
     */
    std::string to_upper(const std::string &str);
    /**
     * @brief Convert a string to lower case.
     *
     * @param str the string to be converted.
     * @return std::string the converted string.
     */
    std::string to_lower(const std::string &str);

    /**
     * @brief Remove leading whitespaces from a string.
     *
     * @param str the string to be stripped.
     * @return std::string the stripped string.
     */
    std::string lstrip(const std::string &str);
    /**
     * @brief Remove trailing whitespaces from a string.
     *
     * @param str the string to be stripped.
     * @return std::string the stripped string.
     */
    std::string rstrip(const std::string &str);
    /**
     * @brief Remove leading and trailing whitespaces from a string.
     *
     * @param str the string to be stripped.
     * @return std::string the stripped string.
     */
    std::string strip(const std::string &str);

    /**
     * @brief Check if a string starts with a given substring.
     *
     * @param str the string to be checked.
     * @param start the substring to be checked.
     * @param ignoreCase whether to ignore case when comparing, default is false.
     * @return true if the string starts with the substring, false otherwise.
     */
    bool starts_with(const std::string &str, const std::string &start, bool ignoreCase = false);
    /**
     * @brief Check if a string ends with a given substring.
     *
     * @param str the string to be checked.
     * @param end the substring to be checked.
     * @param ignoreCase whether to ignore case when comparing, default is false.
     * @return true if the string ends with the substring, false otherwise.
     */
    bool ends_with(const std::string &str, const std::string &end, bool ignoreCase = false);

    /**
     * @brief Split a string into a vector of substrings using a given separator.
     *
     * @param str the string to be split.
     * @param separator the separator to split the string.
     * @return strvec the vector of substrings.
     */
    strvec split(const std::string &str, const std::string &separator);
    /**
     * @brief Join a vector of strings into a single string using a given separator.
     *
     * @param strlist the vector of strings to be joined.
     * @param separator the separator to join the strings.
     * @return std::string the joined string.
     */
    std::string join(const strvec &strlist, const std::string &separator = "");

    /**
     * @brief Desensitizing a string by replacing some characters with '*'.
     *
     * @param str the string to be desensitized.
     * @return std::string the desensitized string.
     */
    std::string desensitizing(const std::string &str);

} // namespace cutl