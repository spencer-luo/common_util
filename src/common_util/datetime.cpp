#include "datetime.h"
#include "timeutil.h"
#include "strfmt.h"

namespace cutl
{
    datetime::datetime()
    {
        timestamp_ms_ = timestamp_ms();
    }

    datetime::datetime(uint64_t timestamp_ms) : timestamp_ms_(timestamp_ms)
    {
    }

    datetime::~datetime()
    {
    }

    datetime datetime::now()
    {
        return datetime();
    }

    datetime datetime::get(const std::string &time_text)
    {
        // todo
        return datetime();
    }

    uint64_t datetime::timestamp() const
    {
        return timestamp_ms_;
    }

    std::string get_format_str(datetime_format fmt)
    {
        std::string text;
        switch (fmt)
        {
        case datetime_format::datetime_format_a: // YYYY-MM-DD HH:MM:SS
            text = "%Y-%m-%d %H:%M:%S";
            break;
        case datetime_format::datetime_format_b: // YYYY.MM.DD HH:MM:SS
            text = "%Y.%m.%d %H:%M:%S";
            break;
        case datetime_format::datetime_format_c: // YYYY/MM/DD HH:MM:SS
            text = "%Y/%m/%d %H:%M:%S";
            break;
        case datetime_format::datetime_format_d: // YYYYMMDD HH:MM:SS
            text = "%Y%m%d %H:%M:%S";
            break;
        default:
            break;
        }

        return text;
    }

    std::string datetime::format(bool local, bool show_milliseconds, datetime_format fmt) const
    {
        auto fmtstr = get_format_str(fmt);
        auto s = timestamp_ms_ / 1000;
        auto ms = timestamp_ms_ % 1000;

        auto text = fmt_timestamp(s, local, fmtstr);
        if (show_milliseconds)
        {
            text += "." + fmt_uint(ms, 3);
        }

        return text;
    }

    // std::string datetime::format(bool local, bool show_milliseconds, const std::string &reggex_pattern) const
    // {
    //     return "";
    // }

} // namespace
