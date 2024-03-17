#pragma once

#include <stdint.h>
#include <string>

namespace cutl
{
    std::string fmt_uint(uint64_t val, uint8_t width = 0, char fill = '0');
    std::string fmt_timeduration(uint64_t seconds);
    std::string fmt_timeduration_ms(uint64_t microseconds);
    std::string fmt_timeduration_us(uint64_t nanoseconds);
    std::string fmt_timestamp(uint64_t second, bool local, const std::string &fmt);
    std::string fmt_timestamp(uint64_t second, bool local = false);
    std::string fmt_timestamp_ms(uint64_t microsecond);
    std::string fmt_timestamp_us(uint64_t nanosecond);
} // namespace