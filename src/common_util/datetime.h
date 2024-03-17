#pragma once

#include <stdint.h>
#include <string>
#include <iostream>

namespace cutl
{
    enum class datetime_format
    {
        datetime_format_a, // YYYY-MM-DD HH:MM:SS
        datetime_format_b, // YYYY.MM.DD HH:MM:SS
        datetime_format_c, // YYYY/MM/DD HH:MM:SS
        datetime_format_d, // YYYYMMDD HH:MM:SS
    };

    class datetime
    {
    public:
        static constexpr int second = 1000;
        static constexpr int min = 60 * second;
        static constexpr int hour = 60 * min;
        static constexpr int day = 24 * hour;

    public:
        datetime(uint64_t timestamp_ms);
        datetime(const datetime &other);
        ~datetime();

    private:
        datetime();

    public:
        static datetime now();
        // todo
        static datetime get(const std::string &time_text);

    public:
        uint64_t timestamp() const;
        std::string format(datetime_format dfmt = datetime_format::datetime_format_a, bool local = false, bool show_milliseconds = true) const;
        // fmt, usages like std::put_time
        std::string format(const std::string &fmt, bool local = false, bool show_milliseconds = true) const;
        std::string localtime() const
        {
            return format(datetime_format::datetime_format_a, true);
        }

        datetime &operator=(const datetime &other);
        datetime operator+(uint64_t ms);
        datetime operator-(uint64_t ms);
        datetime &operator+=(uint64_t ms);
        datetime &operator-=(uint64_t ms);
        int64_t operator-(const datetime &other) const;

    private:
        uint64_t timestamp_ms_;
    };

    std::ostream &operator<<(std::ostream &os, const datetime &dt);

} // namespace
