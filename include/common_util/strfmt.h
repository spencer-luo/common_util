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
 * @file strfmt.h
 * @brief String format functions. this file defines a series of string-formatted functions.
 * @author spencer.luo
 * @date 2024-05-13
 */

#pragma once

#include "timeutil.h"
#include <array>
#include <cstdint>
#include <iomanip>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace cutl
{
    /**
     * @brief Format uint64 value to a string with a given width and fill character.
     *
     * @param val the value to be formatted.
     * @param width the width of the formatted string.
     * @param fill the fill character of the formatted string, default is '0'.
     * @return std::string the formatted string.
     */
    std::string fmt_uint(uint64_t val, uint8_t width = 0, char fill = '0');
    /**
     * @brief Format double value to a string with a given precision.
     *
     * @param val the value to be formatted.
     * @param precision the precision of the formatted string, default is 2.
     * @return std::string the formatted string.
     */
    std::string fmt_double(double val, int precision = 2);

    /**
     * @brief Format a file size to a human-readable string with a given precision.
     *
     * @param size the size to be formatted.
     * @param simplify whether to use a simplify unit.
     * @param precision the precision of the formatted string, default is 1.
     * @return std::string the formatted string.
     */
    std::string fmt_filesize(uint64_t size, bool simplify = true, int precision = 1);

    /**
     * @brief Format a time duration to a human-readable string.
     *
     * @param seconds the duration in seconds.
     * @return std::string the formatted string.
     */
    std::string fmt_timeduration_s(uint64_t seconds);
    /**
     * @brief Format a time duration to a human-readable string.
     *
     * @param microseconds the duration in microseconds.
     * @return std::string the formatted string.
     */
    std::string fmt_timeduration_ms(uint64_t microseconds);
    /**
     * @brief Format a time duration to a human-readable string.
     *
     * @param nanoseconds the duration in nanoseconds.
     * @return std::string the formatted string.
     */
    std::string fmt_timeduration_us(uint64_t nanoseconds);

    /**
     * @brief Format a timestamp to a human-readable string with a given format.
     *
     * @param second the timestamp in seconds.
     * @param local whether to use local time or UTC time, default is local time.
     * If local is true, the function will format the timestamp to local time, otherwise, it will format the timestamp to UTC time.
     * @param fmt the format of the formatted string. useage like std::put_time, see https://en.cppreference.com/w/cpp/io/manip/put_time
     * @return std::string the formatted string.
     */
    std::string fmt_timestamp(uint64_t second, bool local, const std::string &fmt);
    /**
     * @brief Format a timestamp to a human-readable string.
     *
     * @param second the timestamp in seconds.
     * @param local whether to use local time or UTC time, default is local time.
     * If local is true, the function will format the timestamp to local time, otherwise, it will format the timestamp to UTC time.
     * @return std::string the formatted string.
     */
    std::string fmt_timestamp_s(uint64_t second, bool local = true);
    /**
     * @brief Format a timestamp to a human-readable string.
     *
     * @param ms the timestamp in milliseconds.
     * @param local whether to use local time or UTC time, default is local time.
     * If local is true, the function will format the timestamp to local time, otherwise, it will format the timestamp to UTC time.
     * @return std::string the formatted string.
     */
    std::string fmt_timestamp_ms(uint64_t ms, bool local = true);
    /**
     * @brief Format a timestamp to a human-readable string.
     *
     * @param us the timestamp in microseconds.
     * @param local whether to use local time or UTC time, default is local time.
     * If local is true, the function will format the timestamp to local time, otherwise, it will format the timestamp to UTC time.
     * @return std::string the formatted string.
     */
    std::string fmt_timestamp_us(uint64_t us, bool local = true);

    /**
     * @brief Format data to a hex string.
     *
     * @param data the data to be formatted.
     * @param len the length of the data.
     * @param upper whether to use upper case or lower case for hex characters, default is upper case.
     * @param separator the separator between each pair of hex characters, default is space.
     * @return std::string the formatted string.
     */
    std::string to_hex(const uint8_t *data, size_t len, bool upper = true, char separator = ' ');
    /**
     * @brief Format a uint8_t value to a hex string.
     *
     * @param value the value to be formatted.
     * @param upper whether to use upper case or lower case for hex characters, default is upper case.
     * @param prefix the prefix of the formatted string, default is empty.
     * @return std::string the formatted string.
     */
    std::string to_hex(uint8_t value, bool upper = true, const std::string &prefix = "");
    /**
     * @brief Format a uint16_t value to a hex string.
     *
     * @param value the value to be formatted.
     * @param upper whether to use upper case or lower case for hex characters, default is upper case.
     * @param prefix the prefix of the formatted string, default is empty.
     * @return std::string the formatted string.
     */
    std::string to_hex(uint16_t value, bool upper = true, const std::string &prefix = "");
    /**
     * @brief Format a uint32_t value to a hex string.
     *
     * @param value the value to be formatted.
     * @param upper whether to use upper case or lower case for hex characters, default is upper case.
     * @param prefix the prefix of the formatted string, default is empty.
     * @return std::string the formatted string.
     */
    std::string to_hex(uint32_t value, bool upper = true, const std::string &prefix = "");
    /**
     * @brief Format a uint64_t value to a hex string.
     *
     * @param value the value to be formatted.
     * @param upper whether to use upper case or lower case for hex characters, default is upper case.
     * @param prefix the prefix of the formatted string, default is empty.
     * @return std::string the formatted string.
     */
    std::string to_hex(uint64_t value, bool upper = true, const std::string &prefix = "");

    /**
     * @brief Format a pointer to a hex string.
     *
     * @param ptr the pointer to be formatted.
     * @param upper whether to use upper case or lower case for hex characters, default is lower
     * case.
     * @param auto_fill_prefix wheather to auto fill the prefix with "0" or not, default is false.
     * @return std::string
     */
    std::string to_hex(void* ptr, bool upper = false, bool auto_fill_prefix = false);

    /**
     * @brief Format uint8_t value to a binary string.
     *
     * @param value the value to be formatted.
     * @param separator the separator between each pair of binary characters, default is comma.
     * @return std::string the formatted string.
     */
    std::string to_bin(uint8_t value, char separator = ',');
    /**
     * @brief Format uint16_t value to a binary string.
     *
     * @param value the value to be formatted.
     * @param separator the separator between each pair of binary characters, default is space.
     * @return std::string the formatted string.
     */
    std::string to_bin(uint16_t value, char separator = ' ');
    /**
     * @brief Format uint32_t value to a binary string.
     *
     * @param value the value to be formatted.
     * @param separator the separator between each pair of binary characters, default is space.
     * @return std::string the formatted string.
     */
    std::string to_bin(uint32_t value, char separator = ' ');
    /**
     * @brief Format uint64_t value to a binary string.
     *
     * @param value the value to be formatted.
     * @param separator the separator between each pair of binary characters, default is space.
     * @return std::string the formatted string.
     */
    std::string to_bin(uint64_t value, char separator = ' ');

    /**
     * @brief Format array to string with basic data type, such as int, float, double, char, etc.
     *
     * @tparam T the data type of array's element
     * @param arr array
     * @param size size of array
     */
    template<typename T>
    std::string fmt_arr(T* arr, uint32_t size)
    {
        if (size <= 0 || arr == nullptr)
        {
            return "[]";
        }

        std::string res("[" + std::to_string(arr[0]));
        for (uint32_t i = 1; i < size; i++)
        {
            res += ", " + std::to_string(arr[i]);
        }
        res += "]";

        return res;
    }

    /**
     * @brief Format array to string. support basic data type, such as int, float, double, char,
     * etc.
     *
     * @tparam T the data type of array's element
     * @tparam _Nm
     * @param arr array
     * @return std::string
     */
    template<typename T, std::size_t _Nm>
    std::string fmt_arr(const std::array<T, _Nm>& arr)
    {
        if (arr.empty())
        {
            return "[]";
        }

        std::string res("[" + std::to_string(arr[0]));
        for (uint32_t i = 1; i < arr.size(); i++)
        {
            res += ", " + std::to_string(arr[i]);
        }
        res += "]";

        return res;
    }

    /**
     * @brief Format simple vector to string with basic data type, such as int, float, double,
     * char, string etc.
     *
     * @tparam T the data type of vector's element
     * @param vec vector's object
     */
    template<typename T>
    std::string fmt_vec(const std::vector<T>& vec)
    {
        if (vec.empty())
        {
            return "[]";
        }

        std::string res("[" + std::to_string(vec[0]));
        for (int i = 1; i < vec.size(); i++)
        {
            res += ", " + std::to_string(vec[i]);
        }
        res += "]";

        return res;
    }

    /**
     * @brief Format simple map to string with basic data type, such as int, float, double, char,
     string etc.
     *
     * @tparam K the data type of key in map
     * @tparam V the data type of value in map
     * @param mp map's object
     * @param format wheather format the output or not, if true, use multi-line output,
     otherwise,
     * use one-line output
     */
    template<typename K, typename V>
    std::string fmt_map(const std::map<K, V>& mp, bool format = false)
    {
        if (mp.empty())
        {
            return "{}";
        }

        std::stringstream ss;
        if (format)
        {
            ss << "{\n";
            for (auto it = mp.begin(); it != mp.end(); it++)
            {
                ss << "    " << it->first << ": " << it->second << ",\n";
            }
        }
        else
        {
            ss << "{";
            auto it_begin = mp.begin();
            ss << it_begin->first << ": " << it_begin->second;
            it_begin++;
            for (auto it = it_begin; it != mp.end(); it++)
            {
                ss << ",  " << it->first << ": " << it->second;
            }
        }
        ss << "}";

        return ss.str();
    }

    /**
     * @brief Format simple unordered_map to string with basic data type, such as int, float,
     double, char, string etc.
     *
     * @tparam K the data type of key in map
     * @tparam V the data type of value in map
     * @param mp map's object
     * @param format wheather format the output or not, if true, use multi-line output,
     otherwise,
     * use one-line output
     */
    template<typename K, typename V>
    std::string fmt_unordered_map(const std::unordered_map<K, V>& mp, bool format = false)
    {
        if (mp.empty())
        {
            return "{}";
        }

        std::stringstream ss;
        if (format)
        {
            ss << "{\n";
            for (auto it = mp.begin(); it != mp.end(); it++)
            {
                ss << "    " << it->first << ": " << it->second << ",\n";
            }
        }
        else
        {
            ss << "{";
            auto it_begin = mp.begin();
            ss << it_begin->first << ": " << it_begin->second;
            it_begin++;
            for (auto it = it_begin; it != mp.end(); it++)
            {
                ss << ",  " << it->first << ": " << it->second;
            }
        }
        ss << "}";

        return ss.str();
    }

    /**
     * @brief Format simple set with basic data type, such as int, float, double, char, string etc.
     *
     * @tparam T the data type of set's element
     * @param s std::set's object
     */
    template<typename T>
    std::string fmt_set(const std::set<T>& s)
    {
        if (s.empty())
        {
            return "{}";
        }

        auto itr = s.begin();
        std::string res("{" + std::to_string(*itr));
        itr++;
        for (; itr != s.end(); itr++)
        {
            res += ", " + std::to_string(*itr);
        }
        res += "}";

        return res;
    }

    /**
     * @brief Format simple unordered_set with basic data type, such as int, float, double, char,
     * string etc.
     *
     * @tparam T the data type of set's element
     * @param s std::set's object
     */
    template<typename T>
    std::string fmt_unordered_set(const std::unordered_set<T>& s)
    {
        if (s.empty())
        {
            return "{}";
        }

        auto itr = s.begin();
        std::string res("{" + std::to_string(*itr));
        itr++;
        for (; itr != s.end(); itr++)
        {
            res += ", " + std::to_string(*itr);
        }
        res += "}";

        return res;
    }

    /**
     * @brief Format a matrix to string, Matrices supporting the following parameters: float,
     double, int8_t, int16_t, int32_t, int64_t.
     *
     * @tparam T data type
     * @param matrix matrix pointer
     * @param rows
     * @param cols
     * @param name the name of matrix
     * @param precision The number of decimal places to be retained. When printing an integer
     * matrix, set this parameter to 0.
     * @param startY
     * @param startX
     * @param h
     * @param w
     */
    template<typename T>
    std::string fmt_matrix(const T* matrix,
                           uint32_t rows,
                           uint32_t cols,
                           const std::string& name,
                           uint32_t precision = 2,
                           uint32_t startY = 0,
                           uint32_t startX = 0,
                           uint32_t h = 5,
                           uint32_t w = 5)
    {
        uint32_t H = rows - startY;
        if (H <= 0)
        {
            return "startY should less than rows:" + rows;
        }
        uint32_t W = cols - startX;
        if (W <= 0)
        {
            return "startX should less than cols:" + cols;
        }
        H = std::min(H, h);
        W = std::min(W, w);

        std::stringstream ss;
        // 设置输出格式：固定小数点和指定精度
        ss << std::fixed << std::setprecision(precision);
        // 打印矩阵
        ss << "The sub matrix of " << name << ", start: (" << startY << ", " << startX
           << "), size: " << h << " * " << w << "\n";
        for (uint32_t i = startY; i < startY + H; i++)
        {
            for (uint32_t j = startX; j < startX + W; j++)
            {
                if (precision == 0)
                {
                    // 整数矩阵，这里强制类型转换，否则int8_t类型会被当成char打印字符
                    ss << (int)(matrix[i * cols + j]) << " ";
                }
                else
                {
                    ss << matrix[i * cols + j] << " ";
                }
            }
            ss << "\n";
        }

        return ss.str();
    }

} // namespace