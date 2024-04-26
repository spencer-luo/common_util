/**
 * @file verutil.h
 * @brief Version utilities.
 * @author spencer.luo
 * @date 2024-04-26
 */

#pragma once

#include <string>

namespace cutl
{

    /**
     * @brief Check if a string is a valid version.
     *
     * @param text The string to check.
     * @return true if the string is a valid version, false otherwise.
     */
    bool is_version(const std::string &text);
    /**
     * @brief Parse a version string from a text.
     *
     * @param text The text to parse.
     * @return std::string The parsed version string.
     */
    std::string get_version(const std::string &text);
    /**
     * @brief Compare two version strings.
     *
     * @param v1 version string 1.
     * @param v2 version string 2.
     * @return int The result of the comparison. -1 if v1 < v2, 0 if v1 == v2, 1 if v1 > v2.
     */
    int compare_version(const std::string &v1, const std::string &v2);

} // namespace cutl