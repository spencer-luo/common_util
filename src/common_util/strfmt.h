#pragma once

#include <string>
#include <cstdint>
#include <sstream>
#include <iomanip>
#include "timeutil.h"

namespace cutl
{
    std::string fmt_uint(uint64_t val, uint8_t width = 0, char fill = '0');
    std::string fmt_double(double val, int precision = 2);

    std::string fmt_timeduration_s(uint64_t seconds);
    std::string fmt_timeduration_ms(uint64_t microseconds);
    std::string fmt_timeduration_us(uint64_t nanoseconds);

    std::string fmt_timestamp(uint64_t second, bool local, const std::string &fmt);
    std::string fmt_timestamp_s(uint64_t second, bool local = true);
    std::string fmt_timestamp_ms(uint64_t ms, bool local = true);
    std::string fmt_timestamp_us(uint64_t us, bool local = true);

    std::string to_hex(const uint8_t *data, size_t len, bool upper = true, char split = ' ');
    std::string to_hex(uint8_t value, bool upper = true, const std::string &prefix = "");
    std::string to_hex(uint16_t value, bool upper = true, const std::string &prefix = "");
    std::string to_hex(uint32_t value, bool upper = true, const std::string &prefix = "");
    std::string to_hex(uint64_t value, bool upper = true, const std::string &prefix = "");

    std::string to_bin(uint8_t value, char split = ',');
    std::string to_bin(uint16_t value, char split = ' ');
    std::string to_bin(uint32_t value, char split = ' ');
    std::string to_bin(uint64_t value, char split = ' ');

} // namespace