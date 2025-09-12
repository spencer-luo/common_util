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

#include "strfmt.h"
#include <cstdint>
#include <iostream>
#include <string>

namespace cutl
{

#if defined(_WIN32)
constexpr uint8_t cli_clr_black = 0;
constexpr uint8_t cli_clr_red = 4;
constexpr uint8_t cli_clr_green = 2;
constexpr uint8_t cli_clr_blue = 1;
constexpr uint8_t cli_clr_yellow = 6;
constexpr uint8_t cli_clr_purple = 5;
constexpr uint8_t cli_clr_cyan = 3;
constexpr uint8_t cli_clr_white = 15;
constexpr uint8_t cli_clr_gray = 7;
constexpr uint8_t cli_clr_bright_red = 12;
constexpr uint8_t cli_clr_bright_green = 10;
constexpr uint8_t cli_clr_bright_blue = 9;
constexpr uint8_t cli_clr_bright_yellow = 14;
constexpr uint8_t cli_clr_bright_purple = 13;
constexpr uint8_t cli_clr_bright_cyan = 11;
constexpr uint8_t cli_clr_deep_gray = 8;
#else
constexpr uint8_t cli_clr_black = 30;
constexpr uint8_t cli_clr_red = 31;
constexpr uint8_t cli_clr_green = 32;
constexpr uint8_t cli_clr_blue = 34;
constexpr uint8_t cli_clr_yellow = 33;
constexpr uint8_t cli_clr_purple = 35;
constexpr uint8_t cli_clr_cyan = 36;
constexpr uint8_t cli_clr_white = 37;
constexpr uint8_t cli_clr_gray = cli_clr_white;
constexpr uint8_t cli_clr_deep_gray = 90;
constexpr uint8_t cli_clr_bright_red = 91;
constexpr uint8_t cli_clr_bright_green = 92;
constexpr uint8_t cli_clr_bright_blue = 94;
constexpr uint8_t cli_clr_bright_yellow = 93;
constexpr uint8_t cli_clr_bright_purple = 95;
constexpr uint8_t cli_clr_bright_cyan = 96;
#endif

/**
 * @brief Print text with color in console
 *
 * @param str the text string for print
 * @param fgcolor fore-ground, use constant value from cli_clr_xxx
 * @param bgcolor back-ground, use constant value from cli_clr_xxx
 */
void print_clr(const std::string& str, uint8_t fgcolor, uint8_t bgcolor = cli_clr_black);

void print_debug(const std::string& str);
void print_info(const std::string& str);
void print_warn(const std::string& str);
void print_error(const std::string& str);
void print_success(const std::string& str);

/**
 * @brief Print array with basic data type, such as int, float, double, char, etc.
 *
 * @tparam T the data type of array's element
 * @param arr array
 * @param size size of array
 */
template<typename T>
void print_arr(T* arr, uint32_t size)
{
    std::cout << fmt_arr(arr, size) << std::endl;
}

/**
 * @brief Print array to string. support basic data type, such as int, float, double, char,
 * etc.
 *
 * @tparam T the data type of array's element
 * @tparam _Nm
 * @param arr array
 * @return std::string
 */
template<typename T, std::size_t _Nm>
void print_arr(const std::array<T, _Nm>& arr)
{
    std::cout << fmt_arr(arr) << std::endl;
}

/**
 * @brief Print simple vector with basic data type, such as int, float, double, char, string etc.
 *
 * @tparam T the data type of vector's element
 * @param vec vector's object
 */
template<typename T>
void print_vec(const std::vector<T>& vec)
{
    std::cout << fmt_vec(vec) << std::endl;
}

/**
 * @brief Print simple map with basic data type, such as int, float, double, char, string etc.
 *
 * @tparam K the data type of key in map
 * @tparam V the data type of value in map
 * @param mp map's object
 * @param format wheather format the output or not, if true, use multi-line output, otherwise, use
 * one-line output
 */
template<typename K, typename V>
void print_map(const std::map<K, V>& mp, bool format = false)
{
    std::cout << fmt_map(mp, format) << std::endl;
}

/**
 * @brief Print simple map with basic data type, such as int, float, double, char, string etc.
 *
 * @tparam K the data type of key in map
 * @tparam V the data type of value in map
 * @param mp map's object
 * @param format wheather format the output or not, if true, use multi-line output, otherwise, use
 * one-line output
 */
template<typename K, typename V>
void print_unordered_map(const std::unordered_map<K, V>& mp, bool format = false)
{

    std::cout << fmt_unordered_map(mp, format) << std::endl;
}

/**
 * @brief Print a matrix, Matrices supporting the following parameters: float, double, int8_t,
 * int16_t, int32_t, int64_t.
 *
 * @tparam T data type
 * @param matrix matrix pointer
 * @param rows
 * @param cols
 * @param name the name of matrix
 * @param precision The number of decimal places to be retained. When printing an integer matrix,
 * set this parameter to 0.
 * @param startY
 * @param startX
 * @param h
 * @param w
 */
template<typename T>
void print_matrix(const T* matrix,
                  uint32_t rows,
                  uint32_t cols,
                  const std::string& name,
                  uint32_t precision = 2,
                  uint32_t startY = 0,
                  uint32_t startX = 0,
                  uint32_t h = 5,
                  uint32_t w = 5)
{
    std::cout << fmt_matrix(matrix, rows, cols, name, precision, startY, startX, h, w)
              << std::endl;
}

} // namespace cutl
