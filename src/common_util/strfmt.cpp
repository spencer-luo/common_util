#include <sstream>
#include <iomanip>
#include "strfmt.h"
#include "logger.h"

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

    std::string fmt_timeduration(uint64_t seconds)
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
        auto text = fmt_timeduration(s);
        text += "." + fmt_uint(ms, 3) + "ms";
        return text;
    }

    std::string fmt_timeduration_us(uint64_t nanoseconds)
    {
        auto s = nanoseconds / MILLION;
        auto ms = nanoseconds % MILLION;
        auto text = fmt_timeduration(s);
        text += "." + fmt_uint(ms, 6) + "us";
        return text;
    }

    // 格式化时间戳，second单位：秒
    std::string fmt_timestamp(uint64_t second, bool local)
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
        ss << std::put_time(pDatetime, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    // 格式化时间戳，microseconds单位：毫秒
    std::string fmt_timestamp_ms(uint64_t microseconds)
    {
        auto s = microseconds / THOUSAND;
        auto ms = microseconds % THOUSAND;
        auto text = fmt_timestamp(s);
        text += "." + fmt_uint(ms, 3);
        return text;
    }

    // 格式化时间戳，microseconds单位：毫秒
    std::string fmt_timestamp_us(uint64_t nanoseconds)
    {
        auto s = nanoseconds / MILLION;
        auto ms = nanoseconds % MILLION;
        auto text = fmt_timestamp(s);
        text += "." + fmt_uint(ms, 6);
        return text;
    }

} // namespace