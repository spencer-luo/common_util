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
 * @file verutil.h
 * @brief Version utilities.
 * @author spencer.luo
 * @date 2024-05-13
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