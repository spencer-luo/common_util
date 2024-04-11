#include <sstream>
#include <iomanip>
#include <bitset>
#include "strfmt.h"
#include "inner/logger.h"

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

    std::string fmt_timestamp(uint64_t second, bool local, const std::string &fmt)
    {
        std::time_t t(second);
        struct tm datetime;
        struct tm *pDatetime = NULL;
        if (local)
        {
            // localtime 线程不安全
            // pDatetime = std::localtime(&t);
            // localtime_r 线程安全
            pDatetime = localtime_r(&t, &datetime);
        }
        else
        {
            // gmtime 线程不安全
            pDatetime = std::gmtime(&t);
            // gmtime_r 线程安全
            pDatetime = gmtime_r(&t, &datetime);
        }
        if (!pDatetime)
        {
            CUTL_ERROR("pDatetime is null");
            return "";
        }

        std::stringstream ss;
        ss << std::put_time(pDatetime, fmt.c_str());
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

    std::string to_hex(const uint8_t *data, size_t len, bool upper, char split)
    {
        const char *hex_chars = upper ? HEX_CHARS_UPPER : HEX_CHARS_LOWER;

        std::string output;
        output.reserve(3 * len);
        for (size_t i = 0; i < len; i++)
        {
            const char temp = data[i];
            output.push_back(hex_chars[temp / 16]);
            output.push_back(hex_chars[temp % 16]);
            output.push_back(split);
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

    std::string to_bin(uint8_t value, char split)
    {
        std::string text;
        std::bitset<4> v1((value >> 4) & 0xF);
        std::bitset<4> v2(value & 0xF);
        text += v1.to_string();
        text += split;
        text += v2.to_string();
        return text;
    }

    std::string to_bin(uint16_t value, char split)
    {
        std::string text;
        text += to_bin((uint8_t)((value >> 8) & 0xFF)) + split;
        text += to_bin((uint8_t)(value & 0xFF));
        return text;
    }

    std::string to_bin(uint32_t value, char split)
    {
        std::string text;
        text += to_bin((uint8_t)((value >> 24) & 0xFF)) + split;
        text += to_bin((uint8_t)((value >> 16) & 0xFF)) + split;
        text += to_bin((uint8_t)((value >> 8) & 0xFF)) + split;
        text += to_bin((uint8_t)(value & 0xFF));
        return text;
    }

    std::string to_bin(uint64_t value, char split)
    {
        std::string text;
        text += to_bin((uint8_t)((value >> 56) & 0xFF)) + split;
        text += to_bin((uint8_t)((value >> 48) & 0xFF)) + split;
        text += to_bin((uint8_t)((value >> 40) & 0xFF)) + split;
        text += to_bin((uint8_t)((value >> 32) & 0xFF)) + split;
        text += to_bin((uint8_t)((value >> 24) & 0xFF)) + split;
        text += to_bin((uint8_t)((value >> 16) & 0xFF)) + split;
        text += to_bin((uint8_t)((value >> 8) & 0xFF)) + split;
        text += to_bin((uint8_t)(value & 0xFF));
        return text;
    }

} // namespace