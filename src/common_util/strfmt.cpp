#include "strfmt.h"

namespace cutl
{
    constexpr static int ONE_MIN = 60;
    constexpr static int ONE_HOUR = 60 * ONE_MIN;
    constexpr static int ONE_DAY = 24 * ONE_HOUR;
    constexpr static int THOUSAND = 1000;
    constexpr static int MILLION = 1000000;

    std::string fmt_time_duration(uint64_t seconds)
    {
        std::string text;
        if (seconds > ONE_DAY)
        {
            int day = seconds / ONE_DAY;
            text += std::to_string(day) + "d:";
        }

        if (seconds > ONE_HOUR)
        {
            uint64_t hour = (seconds % ONE_DAY) / ONE_HOUR;
            text += std::to_string(hour) + "h:";
        }

        if (seconds > ONE_MIN)
        {
            uint64_t min = (seconds % ONE_HOUR) / ONE_MIN;
            text += std::to_string(min) + "m:";
        }

        uint64_t sec = (seconds % ONE_MIN);
        text += std::to_string(sec) + "s";

        return text;
    }

    std::string fmt_time_duration_ms(uint64_t microseconds)
    {
        auto seconds = microseconds / THOUSAND;
        auto ms = microseconds % THOUSAND;
        auto text = fmt_time_duration(seconds);
        text += "." + std::to_string(ms) + "ms";
        return text;
    }

    std::string fmt_time_duration_us(uint64_t nanoseconds)
    {
        auto seconds = nanoseconds / MILLION;
        auto ms = nanoseconds % MILLION;
        auto text = fmt_time_duration(seconds);
        text += "." + std::to_string(ms) + "us";
        return text;
    }

} // namespace