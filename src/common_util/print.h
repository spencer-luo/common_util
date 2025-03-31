/**
 * @copyright Copyright (c) 2025, Spencer.Luo. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the
 * License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing permissions and
 * limitations.
 *
 * @file print.h
 * @brief print function, such as print_arr, print vector, print map, and print_clr
 * @author Spencer
 * @date 2025-03-31
 */
#pragma once

#include <cstdint>
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace cutl
{

#if defined(_WIN32)

#else
constexpr int cli_clr_black = 0;
constexpr int cli_clr_red = 1;
constexpr int cli_clr_green = 2;
constexpr int cli_clr_yellow = 3;
constexpr int cli_clr_blue = 4;
constexpr int cli_clr_purple = 5;
constexpr int cli_clr_cyan = 6;
constexpr int cli_clr_white = 7;
constexpr int cli_clr_bright_red = 196;
#endif

void print_clr(const std::string& str, uint8_t fgcolor, uint8_t bgcolor = cli_clr_black);

template<typename T>
void print_arr(T* arr, uint32_t size)
{
    if (size <= 0 || arr == nullptr)
    {
        std::cout << "[]" << std::endl;
        return;
    }
    std::cout << "[" << std::to_string(arr[0]);
    for (uint32_t i = 1; i < size; i++)
    {
        std::cout << ", " << std::to_string(arr[i]);
    }

    std::cout << "]" << std::endl;
}

template<typename T>
void print_vec(const std::vector<T>& vec)
{
    if (vec.empty())
    {
        std::cout << "[]" << std::endl;
        return;
    }
    std::cout << "[" << std::to_string(vec[0]);
    for (int i = 1; i < vec.size(); i++)
    {
        std::cout << ", " << std::to_string(vec[i]);
    }

    std::cout << "]" << std::endl;
}

template<typename K, typename V>
void print_map(const std::map<K, V>& mp, bool format = false)
{
    if (mp.empty())
    {
        std::cout << "{}" << std::endl;
        return;
    }
    if (format)
    {
        std::cout << "{" << std::endl;
        for (auto it = mp.begin(); it != mp.end(); it++)
        {
            std::cout << "    " << it->first << ": " << it->second << "," << std::endl;
        }
        std::cout << "}" << std::endl;
    }
    else
    {
        std::cout << "{";
        auto it_begin = mp.begin();
        std::cout << it_begin->first << ": " << it_begin->second;
        it_begin++;
        for (auto it = it_begin; it != mp.end(); it++)
        {
            std::cout << ",  " << it->first << ": " << it->second;
        }
        std::cout << "}" << std::endl;
    }
}

} // namespace cutl
