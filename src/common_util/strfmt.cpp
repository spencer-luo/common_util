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
 * @file strfmt.cpp
 * @brief
 * @author spencer.luo
 * @date 2024-05-13
 */

#include "strfmt.h"
#include "inner/logger.h"
#include "inner/time_util.h"
#include <algorithm>
#include <bitset>
#include <iomanip>
#include <sstream>

namespace cutl
{
    constexpr static int ONE_MIN = 60;
    constexpr static int ONE_HOUR = 60 * ONE_MIN;
    constexpr static int ONE_DAY = 24 * ONE_HOUR;
    constexpr static int THOUSAND = 1000;
    constexpr static int MILLION = 1000000;

    std::string fmt_uint(uint64_t val, uint8_t width, char fill)
    {
        std::stringstream ss;
        ss << std::setfill(fill) << std::setw(width) << val;
        return ss.str();
    }

    std::string fmt_double(double val, int precision)
    {
        std::stringstream ss;
        ss << std::setiosflags(std::ios::fixed) << std::setprecision(precision) << val;
        return ss.str();
    }

    std::string fmt_filesize(uint64_t size, bool simplify, int precision)
    {
        static const double KBSize = 1024;
        static const double MBSize = 1024 * 1024;
        static const double GBSize = 1024 * 1024 * 1024;
        const std::string gb = simplify ? "G" : "GB";
        const std::string mb = simplify ? "M" : "MB";
        const std::string kb = simplify ? "K" : "KB";
        const std::string byte = simplify ? "B" : "Byte";

        if (size > GBSize)
        {
            double hSize = (double)size / GBSize;
            return fmt_double(hSize, precision) + gb;
        }
        else if (size > MBSize)
        {
            double hSize = (double)size / MBSize;
            return fmt_double(hSize, precision) + mb;
        }
        else if (size > KBSize)
        {
            double hSize = (double)size / KBSize;
            return fmt_double(hSize, precision) + kb;
        }
        else
        {
            return fmt_double(size, precision) + byte;
        }

        return "";
    }

    std::string fmt_timeduration_s(uint64_t seconds)
    {
        std::string text;
        if (seconds > ONE_DAY)
        {
            uint64_t day = seconds / ONE_DAY;
            text += std::to_string(day) + "d:";
        }

        if (seconds > ONE_HOUR)
        {
            uint64_t hour = (seconds % ONE_DAY) / ONE_HOUR;
            text += fmt_uint(hour, 2) + "h:";
        }

        if (seconds > ONE_MIN)
        {
            uint64_t min = (seconds % ONE_HOUR) / ONE_MIN;
            text += fmt_uint(min, 2) + "m:";
        }

        uint64_t sec = (seconds % ONE_MIN);
        text += fmt_uint(sec, 2) + "s";

        return text;
    }

    std::string fmt_timeduration_ms(uint64_t microseconds)
    {
        auto s = microseconds / THOUSAND;
        auto ms = microseconds % THOUSAND;
        auto text = fmt_timeduration_s(s);
        text += "." + fmt_uint(ms, 3) + "ms";
        return text;
    }

    std::string fmt_timeduration_us(uint64_t nanoseconds)
    {
        auto s = nanoseconds / MILLION;
        auto ms = nanoseconds % MILLION;
        auto text = fmt_timeduration_s(s);
        text += "." + fmt_uint(ms, 6) + "us";
        return text;
    }

    // https://blog.csdn.net/u010087712/article/details/50731222
    std::string fmt_timestamp(uint64_t second, bool local, const std::string &fmt)
    {
        std::time_t t(second);
        struct tm datetime;
        if (local)
        {
            datetime = localtime_security(t);
        }
        else
        {
            datetime = gmtime_security(t);
        }

        std::stringstream ss;
        ss << std::put_time(&datetime, fmt.c_str());
        return ss.str();
    }

    // 格式化时间戳，second单位：秒
    std::string fmt_timestamp_by_unit(uint64_t t, timeunit unit, bool local)
    {
        uint64_t s = 0;
        std::string extension;
        switch (unit)
        {
        case timeunit::s:
            s = t;
            break;
        case timeunit::ms:
        {
            s = t / THOUSAND;
            auto ms = t % THOUSAND;
            extension += "." + fmt_uint(ms, 3);
        }
        break;
        case timeunit::us:
        {
            s = t / MILLION;
            auto us = t % MILLION;
            extension += "." + fmt_uint(us, 6);
        }
        break;
        default:
            break;
        }

        std::string fmt("%Y-%m-%d %H:%M:%S");
        auto time_str = fmt_timestamp(s, local, fmt);
        time_str += extension;
        return time_str;
    }

    std::string fmt_timestamp_s(uint64_t t, bool local)
    {
        return fmt_timestamp_by_unit(t, timeunit::s, local);
    }

    std::string fmt_timestamp_ms(uint64_t t, bool local)
    {
        return fmt_timestamp_by_unit(t, timeunit::ms, local);
    }

    std::string fmt_timestamp_us(uint64_t t, bool local)
    {
        return fmt_timestamp_by_unit(t, timeunit::us, local);
    }

    static const char HEX_CHARS_UPPER[] = "0123456789ABCDEF";
    static const char HEX_CHARS_LOWER[] = "0123456789abcdef";

    std::string to_hex(const uint8_t *data, size_t len, bool upper, char separator)
    {
        const char *hex_chars = upper ? HEX_CHARS_UPPER : HEX_CHARS_LOWER;

        std::string output;
        output.reserve(3 * len);
        for (size_t i = 0; i < len; i++)
        {
            const char temp = data[i];
            output.push_back(hex_chars[temp / 16]);
            output.push_back(hex_chars[temp % 16]);
            if (separator)
            {
                output.push_back(separator);
            }
            else
            {
                // separator为0或nullptr时，不添加分隔符
            }
        }

        return output;
    }

    std::string to_hex(uint8_t value, bool upper, const std::string &prefix)
    {
        const char *hex_chars = upper ? HEX_CHARS_UPPER : HEX_CHARS_LOWER;
        std::string text = prefix;
        int c1 = value / 16;
        int c2 = value % 16;
        text.push_back(hex_chars[c1]);
        text.push_back(hex_chars[c2]);
        return text;
    }
    std::string to_hex(uint16_t value, bool upper, const std::string &prefix)
    {
        std::string text = prefix;
        text += to_hex((uint8_t)((value >> 8) & 0xFF), upper);
        text += to_hex((uint8_t)(value & 0xFF), upper);
        return text;
    }

    std::string to_hex(uint32_t value, bool upper, const std::string &prefix)
    {
        std::string text = prefix;
        text += to_hex((uint8_t)((value >> 24) & 0xFF), upper);
        text += to_hex((uint8_t)((value >> 16) & 0xFF), upper);
        text += to_hex((uint8_t)((value >> 8) & 0xFF), upper);
        text += to_hex((uint8_t)(value & 0xFF), upper);
        return text;
    }

    std::string to_hex(uint64_t value, bool upper, const std::string &prefix)
    {
        std::string text = prefix;
        text += to_hex((uint8_t)((value >> 56) & 0xFF), upper);
        text += to_hex((uint8_t)((value >> 48) & 0xFF), upper);
        text += to_hex((uint8_t)((value >> 40) & 0xFF), upper);
        text += to_hex((uint8_t)((value >> 32) & 0xFF), upper);
        text += to_hex((uint8_t)((value >> 24) & 0xFF), upper);
        text += to_hex((uint8_t)((value >> 16) & 0xFF), upper);
        text += to_hex((uint8_t)((value >> 8) & 0xFF), upper);
        text += to_hex((uint8_t)(value & 0xFF), upper);
        return text;
    }

    std::string to_hex(void* ptr, bool upper, bool auto_fill_prefix)
    {

        std::stringstream ss;
        if (auto_fill_prefix)
        {
            auto byte = sizeof(void*);
            ss << std::setfill('0') << std::setw(2 * byte);
        }
        ss << ptr;
        // return ss.str();
        auto result = ss.str();
        if (upper)
        {
            std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        }
        return result;
    }

    std::string to_bin(uint8_t value, char separator)
    {
        std::string text;
        std::bitset<4> v1((value >> 4) & 0xF);
        std::bitset<4> v2(value & 0xF);
        text += v1.to_string();
        text += separator;
        text += v2.to_string();
        return text;
    }

    std::string to_bin(uint16_t value, char separator)
    {
        std::string text;
        text += to_bin((uint8_t)((value >> 8) & 0xFF)) + separator;
        text += to_bin((uint8_t)(value & 0xFF));
        return text;
    }

    std::string to_bin(uint32_t value, char separator)
    {
        std::string text;
        text += to_bin((uint8_t)((value >> 24) & 0xFF)) + separator;
        text += to_bin((uint8_t)((value >> 16) & 0xFF)) + separator;
        text += to_bin((uint8_t)((value >> 8) & 0xFF)) + separator;
        text += to_bin((uint8_t)(value & 0xFF));
        return text;
    }

    std::string to_bin(uint64_t value, char separator)
    {
        std::string text;
        text += to_bin((uint8_t)((value >> 56) & 0xFF)) + separator;
        text += to_bin((uint8_t)((value >> 48) & 0xFF)) + separator;
        text += to_bin((uint8_t)((value >> 40) & 0xFF)) + separator;
        text += to_bin((uint8_t)((value >> 32) & 0xFF)) + separator;
        text += to_bin((uint8_t)((value >> 24) & 0xFF)) + separator;
        text += to_bin((uint8_t)((value >> 16) & 0xFF)) + separator;
        text += to_bin((uint8_t)((value >> 8) & 0xFF)) + separator;
        text += to_bin((uint8_t)(value & 0xFF));
        return text;
    }

} // namespace