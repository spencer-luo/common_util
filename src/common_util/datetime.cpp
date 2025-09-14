/**
 * @copyright Copyright (c) 2024, Spencer.Luo. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations.
 *
 * @file datetime.cpp
 * @brief
 * @author spencer.luo
 * @date 2024-05-13
 */

#include "datetime.h"
#include "timeutil.h"
#include "strfmt.h"
#include "inner/logger.h"

namespace cutl
{
    datetime::datetime(const datetime &other)
    {
        timestamp_ms_ = other.timestamp_ms_;
    }

    datetime::datetime()
    {
        timestamp_ms_ = 0;
    }

    datetime::datetime(uint64_t ms) : timestamp_ms_(ms)
    {
    }

    datetime::~datetime()
    {
    }

    datetime datetime::now()
    {
        return datetime(cutl::timestamp(timeunit::ms));
    }

    datetime datetime::get(const std::string &time_text, int isdst)
    {
        std::smatch matchRes;
        bool result = false;
        static time_regex_vec_type fmt_list = {
            // 0/1, 2/3, 4/5, 6/7的顺序不能反，因为不含毫秒数的时间会被优先匹配到
            std::make_pair("YYYY-MM-DD HH:MM:SS.sss", std::regex(R"((\d{4})-(\d{2})-(\d{2})[ ](\d{2}):(\d{2}):(\d{2}).(\d{3}))")),
            std::make_pair("YYYY-MM-DD HH:MM:SS", std::regex(R"((\d{4})-(\d{2})-(\d{2})[ ](\d{2}):(\d{2}):(\d{2}))")),
            std::make_pair("YYYY.MM.DD HH:MM:SS.sss", std::regex(R"((\d{4}).(\d{2}).(\d{2})[ ](\d{2}):(\d{2}):(\d{2}).(\d{3}))")),
            std::make_pair("YYYY.MM.DD HH:MM:SS", std::regex(R"((\d{4}).(\d{2}).(\d{2})[ ](\d{2}):(\d{2}):(\d{2}))")),
            std::make_pair("YYYY/MM/DD HH:MM:SS.sss", std::regex(R"((\d{4})/(\d{2})/(\d{2})[ ](\d{2}):(\d{2}):(\d{2}).(\d{3}))")),
            std::make_pair("YYYY/MM/DD HH:MM:SS", std::regex(R"((\d{4})/(\d{2})/(\d{2})[ ](\d{2}):(\d{2}):(\d{2}))")),
            std::make_pair("YYYYMMDD HH:MM:SS.sss", std::regex(R"((\d{4})(\d{2})(\d{2})[ ](\d{2}):(\d{2}):(\d{2}).(\d{3}))")),
            std::make_pair("YYYYMMDD HH:MM:SS", std::regex(R"((\d{4})(\d{2})(\d{2})[ ](\d{2}):(\d{2}):(\d{2}))")),
        };
        for (size_t i = 0; i < fmt_list.size(); i++)
        {
            auto &fmt_text = fmt_list[i].first;
            auto &fmt_pattern = fmt_list[i].second;
            result = std::regex_search(time_text, matchRes, fmt_pattern);
            if (result)
            {
                CUTL_DEBUG("matched regex: " + fmt_text);
                break;
            }
        }

        if (!result || matchRes.size() < 7)
        {
            auto time_fmts = supported_time_formats(fmt_list);
            CUTL_ERROR("Only the following time formats are supported:\n" + time_fmts);
            return datetime();
        }

        CUTL_DEBUG("matchRes size:" + std::to_string(matchRes.size()) + ", res:" + matchRes[0].str());
        // 解析毫秒值
        int ms = 0;
        if (matchRes.size() == 8)
        {
            ms = std::stoi(matchRes[7].str());
        }
        // 解析tm结构的时间
        struct tm time = {};
        if (matchRes.size() >= 7)
        {
            for (size_t i = 1; i < 7; i++)
            {
                time.tm_year = std::stoi(matchRes[1]);
                time.tm_mon = std::stoi(matchRes[2]);
                time.tm_mday = std::stoi(matchRes[3]);
                time.tm_hour = std::stoi(matchRes[4]);
                time.tm_min = std::stoi(matchRes[5]);
                time.tm_sec = std::stoi(matchRes[6]);
                time.tm_isdst = isdst;
            }
        }
        if (!verify_time(time))
        {
            return datetime();
        }

        // 转换为时间戳
        time.tm_year -= 1900;
        time.tm_mon -= 1;
        auto ret = mktime(&time);
        if (ret == -1)
        {
            CUTL_ERROR("mktime() failed");
            return datetime();
        }
        auto s = static_cast<uint64_t>(ret);
        return datetime(s2ms(s) + ms);
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
        if (timestamp_ms_ == 0)
        {
            CUTL_ERROR("datetime is not initialized");
            return {};
        }

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

    std::string datetime::supported_time_formats(const time_regex_vec_type &fmtlist)
    {
        std::string time_fmts;
        for (size_t i = 0; i < fmtlist.size(); i++)
        {
            time_fmts += fmtlist[i].first + "\n";
        }
        return time_fmts;
    }

    bool datetime::verify_time(const struct tm &time)
    {
        // 校验年
        if (time.tm_year < 1900)
        {
            CUTL_ERROR("the year should be >= 1900");
            return false;
        }
        // 校验月
        if (time.tm_mon < 1 || time.tm_mon > 12)
        {
            CUTL_ERROR("the month should be between 1 and 12");
            return false;
        }
        // 校验日
        std::vector<int> large_month = {1, 3, 5, 7, 8, 10, 12};
        if (std::find(large_month.begin(), large_month.end(), time.tm_mon) != large_month.end() && (time.tm_mday < 1 || time.tm_mday > 31))
        {
            CUTL_ERROR("the day should be between 1 and 31 for " + std::to_string(time.tm_mon) + " month");
            return false;
        }
        std::vector<int> small_month = {4, 6, 9, 11};
        if (std::find(small_month.begin(), small_month.end(), time.tm_mon) != small_month.end() && (time.tm_mday < 1 || time.tm_mday > 30))
        {
            CUTL_ERROR("the day should be between 1 and 30 for " + std::to_string(time.tm_mon) + " month");
            return false;
        }
        if (time.tm_mon == 2)
        {
            auto is_leap_year = [](int year)
            { return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0); };
            if (is_leap_year(time.tm_year) && (time.tm_mday < 1 || time.tm_mday > 29))
            {
                CUTL_ERROR("the day should be between 1 and 29 for " + std::to_string(time.tm_year) + "-" + fmt_uint(time.tm_mon, 2));
                return false;
            }
            if (!is_leap_year(time.tm_year) && (time.tm_mday < 1 || time.tm_mday > 28))
            {
                CUTL_ERROR("the day should be between 1 and 28 for " + std::to_string(time.tm_year) + "-" + fmt_uint(time.tm_mon, 2));
                return false;
            }
        }

        // 校验时分秒
        if (time.tm_hour < 0 || time.tm_hour > 23)
        {
            CUTL_ERROR("the hour should be between 0 and 23");
            return false;
        }
        if (time.tm_min < 0 || time.tm_min > 59)
        {
            CUTL_ERROR("the minute should be between 0 and 59");
            return false;
        }
        if (time.tm_sec < 0 || time.tm_sec > 59)
        {
            CUTL_ERROR("the second should be between 0 and 59");
            return false;
        }

        return true;
    }

    std::ostream &operator<<(std::ostream &os, const datetime &dt)
    {
        os << dt.format();
        return os;
    }

} // namespace
