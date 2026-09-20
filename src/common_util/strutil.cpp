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
 * @file strutil.cpp
 * @brief
 * @author spencer.luo
 * @date 2024-05-13
 */

#include "strutil.h"
#include "inner/logger.h"
#include "inner/string_util.h"
#include <algorithm>
#include <cctype>
#include <locale>

namespace cutl
{
    std::string to_upper(const std::string &str)
    {
        std::string result = str;
        // <cctype>里面声明了一个C版本的函数toupper/tolower,<local>里也声明了一个toupper/tolower的函数模板
        // 所以std命名空间下std::toupper有名称冲突，Linux下会编译失败，这里使用全局作用域的::toupper（即使用C语言的版本）
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }

    std::string to_lower(const std::string &str)
    {
        std::string result = str;
        // <cctype>里面声明了一个C版本的函数toupper/tolower,<local>里也声明了一个toupper/tolower的函数模板
        // 所以std命名空间下std::tolower有名称冲突，Linux下会编译失败，这里使用全局作用域的::tolower（即使用C语言的版本）
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }

    std::string lstrip(const std::string &str)
    {
        if (str.empty())
        {
            return "";
        }

        size_t index = 0;
        for (size_t i = 0; i < str.length(); i++)
        {
            if (!std::isspace(str[i]))
            {
                index = i;
                break;
            }
        }

        return str.substr(index, str.length() - index);
    }

    std::string rstrip(const std::string &str)
    {
        if (str.empty())
        {
            return "";
        }

        size_t index = str.length() - 1;
        for (size_t i = str.length() - 1; i >= 0; i--)
        {
            if (!std::isspace(str[i]))
            {
                index = i;
                break;
            }
        }
        return str.substr(0, index + 1);
    }

    std::string strip(const std::string &str)
    {
        if (str.empty())
        {
            return "";
        }

        size_t index1 = 0;
        for (size_t i = 0; i < str.length(); i++)
        {
            if (!std::isspace(str[i]))
            {
                index1 = i;
                break;
            }
        }
        size_t index2 = str.length() - 1;
        for (size_t i = str.length() - 1; i >= 0; i--)
        {
            if (!std::isspace(str[i]))
            {
                index2 = i;
                break;
            }
        }
        auto len = index2 - index1 + 1;

        return str.substr(index1, len);
    }

    bool starts_with(const std::string &str, const std::string &start, bool ignoreCase)
    {
        int srclen = str.size();
        int startlen = start.size();
        if (srclen < startlen)
        {
            return false;
        }

        std::string temp = str.substr(0, startlen);
        if (ignoreCase)
        {
            return to_lower(temp) == to_lower(start);
        }
        else
        {
            return temp == start;
        }
    }

    bool ends_with(const std::string &str, const std::string &end, bool ignoreCase)
    {
        int srclen = str.size();
        int endlen = end.size();
        if (srclen < endlen)
        {
            return false;
        }

        std::string temp = str.substr(srclen - endlen, endlen);
        if (ignoreCase)
        {
            return to_lower(temp) == to_lower(end);
        }
        else
        {
            return temp == end;
        }
    }

    strvec split(const std::string &str, const std::string &pattern)
    {
        strvec res;
        if (str.empty())
        {
            return res;
        }

        if (pattern.empty())
        {
            // 空分隔符无法切分，原样返回，避免调用方拿到一堆空串
            CUTL_ERROR("the separator is empty, cannot split the string: " << str);
            res.emplace_back(str);
            return res;
        }

        size_t startIndex = 0;
        while (true)
        {
            size_t pos = str.find(pattern, startIndex);
            if (pos == std::string::npos)
            {
                // 最后一段，分隔符结尾时这里会得到一个空串，与 Python str.split() 一致
                res.emplace_back(str.substr(startIndex));
                break;
            }

            res.emplace_back(str.substr(startIndex, pos - startIndex));
            // 必须跳过整个分隔符的长度，否则多字符分隔符会把剩余部分算进下一段
            startIndex = pos + pattern.length();
        }

        return res;
    }

    std::string join(const strvec &strlist, const std::string &separator)
    {
        std::string text;
        for (size_t i = 0; i < strlist.size(); i++)
        {
            text += strlist[i];
            if (i < strlist.size() - 1)
            {
                text += separator;
            }
        }

        return text;
    }

    // 字符串脱敏处理
    std::string desensitizing(const std::string &str)
    {
        std::string result;
        // 只打印前1/4和后1/4的内容，中间用*表示
        if (str.empty())
        {
            result = "";
        }
        else if (str.length() == 1)
        {
            result = "*";
        }
        else if (str.length() == 2)
        {
            result = str.substr(0, 1) + std::string(str.length() - 1, '*');
        }
        else if (str.length() <= 6)
        {
            result = str.substr(0, 2) + std::string(str.length() - 2, '*');
        }
        else if (str.length() < 10)
        {
            result = str.substr(0, 2) + std::string(str.length() - 4, '*') + str.substr(str.length() - 2, 2);
        }
        else if (str.length() < 16)
        {
            // 长度控制在最长12位，中间×不超过6
            auto startCount = (str.length() - 6) > 6 ? 6 : (str.length() - 6);
            result = str.substr(0, 3) + std::string(startCount, '*') + str.substr(str.length() - 3, 3);
        }
        else
        {
            // 长度控制在最长12位
            result = str.substr(0, 4) + std::string(4, '*') + str.substr(str.length() - 4, 4);
        }
        return result;
    }

    std::string ws2s(const std::wstring& ws, const std::string& locale)
    {
        std::string strLocale = setlocale(LC_ALL, locale.c_str());
        auto result = wstring2string(ws);
        setlocale(LC_ALL, strLocale.c_str());
        return result;
    }

    std::wstring s2ws(const std::string& s, const std::string& locale)
    {
        std::string strLocale = setlocale(LC_ALL, locale.c_str());
        auto result = string2wstring(s);
        setlocale(LC_ALL, strLocale.c_str());
        return result;
    }

} // namespace cutl