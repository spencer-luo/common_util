#pragma once

#include <cstdint>
#include <string>
#include <iostream>
#include <regex>
#include <vector>
#include <utility>

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
        datetime(uint64_t ms);
        datetime(const datetime &other);
        ~datetime();

    private:
        datetime();

    public:
        static datetime now();
        // 根据(本地时间)时间字符串构造datetime对象
        // isdst: 夏令时设置，-1: 系统根据时区自动判断夏令时，0: 非夏令时，1: 夏令时
        static datetime get(const std::string &time_text, int isdst = -1);

    public:
        uint64_t timestamp() const;
        std::string format(datetime_format dfmt = datetime_format::datetime_format_a, bool local = true, bool show_milliseconds = true) const;
        // fmt, usages like std::put_time
        std::string format(const std::string &fmt, bool local = true, bool show_milliseconds = true) const;
        std::string utctime() const
        {
            return format(datetime_format::datetime_format_a, false);
        }

        datetime &operator=(const datetime &other);
        datetime operator+(uint64_t ms);
        datetime operator-(uint64_t ms);
        datetime &operator+=(uint64_t ms);
        datetime &operator-=(uint64_t ms);
        int64_t operator-(const datetime &other) const;

    private:
        using time_regex_type = std::pair<std::string, std::regex>;
        using time_regex_vec_type = std::vector<time_regex_type>;
        static std::string supported_time_formats(const time_regex_vec_type &fmtlist);
        static bool verify_time(const struct tm &time);

    private:
        uint64_t timestamp_ms_;
    };

    std::ostream &operator<<(std::ostream &os, const datetime &dt);

} // namespace
