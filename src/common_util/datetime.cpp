#include "datetime.h"
#include "timeutil.h"
#include "strfmt.h"

namespace cutl
{
    datetime::datetime(const datetime &other)
    {
        timestamp_ms_ = other.timestamp_ms_;
    }

    datetime::datetime()
    {
        timestamp_ms_ = cutl::timestamp(cutl::time_unit::millisecond);
    }

    datetime::datetime(uint64_t ms) : timestamp_ms_(ms)
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

    std::string datetime::format(datetime_format fmt, bool local, bool show_milliseconds) const
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

    std::string datetime::format(const std::string &fmt, bool local, bool show_milliseconds) const
    {
        auto s = timestamp_ms_ / 1000;
        auto ms = timestamp_ms_ % 1000;
        auto text = fmt_timestamp(s, local, fmt);
        if (show_milliseconds)
        {
            text += "." + fmt_uint(ms, 3);
        }
        return text;
    }

    datetime &datetime::operator=(const datetime &other)
    {
        if (this == &other)
        {
            return *this;
        }

        timestamp_ms_ = other.timestamp_ms_;
        return *this;
    }

    datetime datetime::operator+(uint64_t ms)
    {
        datetime dt(*this);
        dt.timestamp_ms_ += ms;
        return dt;
    }

    datetime datetime::operator-(uint64_t ms)
    {
        datetime dt(*this);
        dt.timestamp_ms_ -= ms;
        return dt;
    }

    datetime &datetime::operator+=(uint64_t ms)
    {
        timestamp_ms_ += ms;
        return *this;
    }

    datetime &datetime::operator-=(uint64_t ms)
    {
        timestamp_ms_ -= ms;
        return *this;
    }

    int64_t datetime::operator-(const datetime &other) const
    {
        int64_t diff = timestamp_ms_ - other.timestamp_ms_;
        return diff;
    }

    std::ostream &operator<<(std::ostream &os, const datetime &dt)
    {
        os << dt.format();
        return os;
    }

} // namespace
