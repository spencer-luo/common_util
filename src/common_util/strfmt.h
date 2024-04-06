#pragma once

#include <string>
#include <cstdint>
#include <sstream>
#include <iomanip>
#include "timeutil.h"

namespace cutl
{
    std::string fmt_uint(uint64_t val, uint8_t width = 0, char fill = '0');
    std::string fmt_timeduration_s(uint64_t seconds);
    std::string fmt_timeduration_ms(uint64_t microseconds);
    std::string fmt_timeduration_us(uint64_t nanoseconds);
    std::string fmt_timestamp(uint64_t second, bool local, const std::string &fmt);
    std::string fmt_timestamp_s(uint64_t second, bool local = true);
    std::string fmt_timestamp_ms(uint64_t ms, bool local = true);
    std::string fmt_timestamp_us(uint64_t us, bool local = true);

    std::string to_hex(const uint8_t *data, size_t len, bool upper = true, char split = ' ');
    std::string to_hex(uint8_t value, bool upper = true, const std::string &prefix = "");

    // 整数转换成16进制
    // T 要转换的数据类型，可以是int、long int、short等
    template <typename T>
    std::string to_hex(T value, bool upper = true, const std::string &prefix = "")
    {
        std::stringstream ss;
        // 以十六制(大写)形式输出，长度不足时补０
        ss << prefix << std::setfill('0') << std::setw(sizeof(T) * 2) << std::setiosflags(std::ios::uppercase) << std::hex;
        ss << value;
        return ss.str();
    }

} // namespace