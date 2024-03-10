#pragma once

#include <stdint.h>
#include <string>

namespace cutl
{
    enum date_format
    {
        DATE_FORMAT_YYYY_MM_DD_A, // YYYY-MM-DD
        DATE_FORMAT_YYYY_MM_DD_B, // YYYY.MM.DD
        DATE_FORMAT_YYYY_MM_DD_C, // YYYY/MM/DD
        DATE_FORMAT_YYYYMMDD,     // YYYYMMDD
    };

    enum time_format
    {
        TIME_FORMAT_HH_MM_SS_SSS, // HH:MM:SS.SSS
        TIME_FORMAT_HH_MM_SS,     // HH:MM:SS
    };

    class datetime
    {
    public:
        datetime(uint64_t timestamp_ms);
        ~datetime();

    private:
        datetime();

    public:
        static datetime now();
        static datetime get(const std::string &time_text);

    public:
        std::string format(date_format dfmt = date_format::DATE_FORMAT_YYYY_MM_DD_A,
                           time_format tfmt = time_format::TIME_FORMAT_HH_MM_SS_SSS) const;
        std::string format(std::string &reggex_pattern) const;

    private:
        uint64_t timestamp_ms_;
    };

} // namespace
