#pragma once

#include <stdint.h>
#include <string>

namespace cutl
{
    std::string fmt_time_duration(uint64_t seconds);
    std::string fmt_time_duration_ms(uint64_t microseconds);
    std::string fmt_time_duration_us(uint64_t nanoseconds);
} // namespace