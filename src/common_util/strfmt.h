#pragma once

#include <stdint.h>
#include <string>

namespace cutl
{
    std::string fmt_timeduration(uint64_t seconds);
    std::string fmt_timeduration_ms(uint64_t microseconds);
    std::string fmt_timeduration_us(uint64_t nanoseconds);
    std::string fmt_timestamp(uint64_t second);
    std::string fmt_timestamp_ms(uint64_t microsecond);
    std::string fmt_timestamp_us(uint64_t nanosecond);
} // namespace