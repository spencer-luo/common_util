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
 * @file verutil.cpp
 * @brief
 * @author spencer.luo
 * @date 2024-05-13
 */

#include <regex>
#include "verutil.h"
#include "strutil.h"
#include "inner/logger.h"

namespace cutl
{

    bool is_version(const std::string &text)
    {
        try
        {
            std::regex versionRule(R"(^\d{1,2}([.]\d{1,2}){0,2}[.]\d{1,3}$)");
            return regex_match(text, versionRule);
        }
        catch (const std::exception &e)
        {
            CUTL_ERROR(e.what());
            return false;
        }
    }

    std::string get_version(const std::string &text)
    {
        try
        {
            std::regex versionRule(R"(\d{1,2}(\.\d{1,2}){0,2}\.\d{1,3})");
            std::smatch result;
            if (regex_search(text, result, versionRule))
            {
                return result.str();
            }
            return "";
        }
        catch (const std::regex_error &e)
        {
            CUTL_ERROR(e.what());
            return "";
        }
    }

    int compare_version(const std::string &v1, const std::string &v2)
    {
        auto arr1 = split(v1, ".");
        auto arr2 = split(v2, ".");
        int len1 = arr1.size();
        int len2 = arr2.size();
        int len = std::max(len1, len2);
        for (int i = 0; i < len; i++)
        {
            int num1 = i < len1 ? std::stoi(arr1[i]) : 0;
            int num2 = i < len2 ? std::stoi(arr2[i]) : 0;
            if (num1 < num2)
            {
                return -1;
            }
            else if (num1 > num2)
            {
                return 1;
            }
        }
        return 0;
    }

} // namespace cutl