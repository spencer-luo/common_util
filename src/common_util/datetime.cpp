#include <regex>
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
        // timestamp_ms_ = cutl::timestamp(cutl::time_unit::millisecond);
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
        return datetime(cutl::timestamp(cutl::time_unit::millisecond));
    }

    datetime datetime::get(const std::string &time_text)
    {
        // todo
        // time_text
        std::regex fmt(R"(^\d{1,2}([.]\d{1,2}){0,2}[.]\d{1,3}$)");
        std::regex versionRule(R"(^\d{1,2}([.]\d{1,2}){0,2}[.]\d{1,3}$)");
        // regex_match(text, versionRule);

        std::regex fmta(R"(^(\d{4})-(\d{2})-(\d{2})[ ](\d{2}):(\d{2}):(\d{2})$)"); // YYYY-MM-DD HH:MM:SS
        std::regex fmta_ms("");                                                    // YYYY-MM-DD HH:MM:SS
        std::regex fmtb("");                                                       // YYYY.MM.DD HH:MM:SS
        std::regex fmtb_ms("");                                                    // YYYY.MM.DD HH:MM:SS
        std::regex fmtc("");                                                       // YYYY/MM/DD HH:MM:SS
        std::regex fmtc_ms("");                                                    // YYYY/MM/DD HH:MM:SS
        std::regex fmtd("");                                                       // YYYYMMDD HH:MM:SS
        std::regex fmtd_ms("");                                                    // YYYYMMDD HH:MM:SS

        std::smatch matchRes;
        bool result = false;
        static std::vector<std::regex> fmt_list = {fmta, fmta_ms, fmtb, fmtb_ms, fmtc, fmtc_ms, fmtd, fmtd_ms};
        for (size_t i = 0; i < fmt_list.size(); i++)
        {
            result = std::regex_search(time_text, matchRes, fmt_list[i]);
            if (result)
            {
                CUTL_DEBUG("matched regex:" + std::to_string(i));
                break;
            }
        }

        if (!result)
        {
            CUTL_ERROR("datetime::get() not match fmta");
            return datetime();
        }

        // todo
        std::cout << matchRes.size() << std::endl;
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
                // todo
                // std::cout << matchRes[i] << std::endl;
                time.tm_year = std::stoi(matchRes[1]);
                time.tm_mon = std::stoi(matchRes[2]);
                time.tm_mday = std::stoi(matchRes[3]);
                time.tm_hour = std::stoi(matchRes[4]);
                time.tm_min = std::stoi(matchRes[5]);
                time.tm_sec = std::stoi(matchRes[6]);
                // TODO: 不考虑夏令时
                time.tm_isdst = 0;
            }
        }
        // todo
        std::cout << "time1:" << std::endl;
        print_time(time);
        if (!verify_time(time))
        {
            return datetime();
        }

        time.tm_year -= 1900;
        time.tm_mon -= 1;
        auto s = static_cast<uint64_t>(mktime(&time));
        // todo
        std::cout << "time2:" << std::endl;
        print_time(time);
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
            // TODO: 这里暂时不考虑闰年的场景
            CUTL_ERROR("the day should be between 1 and 29 for " + std::to_string(time.tm_mon) + " month");
            return false;
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
        // bool leap_year = time.tm_year % 4 == 0 && (time.tm_year % 100 != 0 || time.tm_year % 400 == 0);
        // if (time.tm_mday < 1 || time.tm_mday > 31)
        // return false;
    }

    void datetime::print_time(const struct tm &time)
    {
        std::cout << "year: " << time.tm_year << std::endl;
        std::cout << "month: " << time.tm_mon << std::endl;
        std::cout << "day: " << time.tm_mday << std::endl;
        std::cout << "hour: " << time.tm_hour << std::endl;
        std::cout << "minute: " << time.tm_min << std::endl;
        std::cout << "second: " << time.tm_sec << std::endl;
        std::cout << "isdst: " << time.tm_isdst << std::endl;
        std::cout << "wday: " << time.tm_wday << std::endl;
        std::cout << "yday: " << time.tm_yday << std::endl;
        std::cout << "gmtoff: " << time.tm_gmtoff << std::endl;
        // std::cout << "zone: " << std::(time.tm_zone) << std::endl;
    }

} // namespace
