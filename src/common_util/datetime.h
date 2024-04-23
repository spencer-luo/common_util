/**
 * @file datetime.h
 * @brief A simple, feature-rich modern C++ date-time class
 * @author spencer.luo
 * @date 2024-04-22
 */

#pragma once

#include <cstdint>
#include <string>
#include <iostream>
#include <regex>
#include <vector>
#include <utility>

namespace cutl
{
    /**
     * @brief the string datetime format for parsing and formatting
     *
     */
    enum class datetime_format
    {
        /** YYYY-MM-DD HH:MM:SS.sss */
        datetime_format_a,
        /** YYYY.MM.DD HH:MM:SS */
        datetime_format_b,
        /** YYYY/MM/DD HH:MM:SS */
        datetime_format_c,
        /** YYYYMMDD HH:MM:SS */
        datetime_format_d,
    };

    /**
     * @brief A simple, feature-rich modern C++ date-time class
     *
     */
    class datetime
    {
    public:
        /**
         * @brief Constants value: second, expressed in milliseconds.
         *
         */
        static constexpr int second = 1000;
        /**
         * @brief Constants value: min, expressed in milliseconds.
         *
         */
        static constexpr int min = 60 * second;
        /**
         * @brief Constants value: hour, expressed in milliseconds.
         *
         */
        static constexpr int hour = 60 * min;
        /**
         * @brief Constants value: day, expressed in milliseconds.
         *
         */
        static constexpr int day = 24 * hour;

    public:
        /**
         * @brief Construct a new datetime object
         *
         * @param ms a timestamp in milliseconds for initialize the datetime object
         */
        datetime(uint64_t ms);

        /**
         * @brief Construct a new datetime object
         *
         * @param other other datetime object to copy
         */
        datetime(const datetime &other);

        /**
         * @brief Destroy the datetime object
         *
         */
        ~datetime();

    private:
        datetime();

    public:
        /**
         * @brief Get a datetime object for the current system time
         *
         * @return datetime object for the current system time
         */
        static datetime now();

        /**
         * @brief Constructs a datetime object from a local time string.
         *
         * Only the following time formats are supported:
         * - YYYY-MM-DD HH:MM:SS.sss
         * - YYYY-MM-DD HH:MM:SS
         * - YYYY.MM.DD HH:MM:SS.sss
         * - YYYY.MM.DD HH:MM:SS
         * - YYYY/MM/DD HH:MM:SS.sss
         * - YYYY/MM/DD HH:MM:SS
         * - YYYYMMDD HH:MM:SS.sss
         * - YYYYMMDD HH:MM:SS
         * @param time_text local time string, use the below formats to construct a datetime object.
         * @param isdst the setting of daylight saving time, -1 means system automatically determine, 0 means not in daylight saving time, 1 means in daylight saving time
         * @return datetime object constructed from the local time string
         */
        static datetime get(const std::string &time_text, int isdst = -1);

    public:
        /**
         * @brief Get the timestamp in milliseconds
         *
         * @return the timestamp in milliseconds
         */
        uint64_t timestamp() const;
        /**
         * @brief format the datetime object to a string
         *
         * @param dfmt datetime format, a value of datetime_format enum, default is datetime_format_a
         * @param local whether to use local time, default is true. if true means output in local time, otherwise, output in UTC time.
         * @param show_milliseconds whether to show milliseconds, default is true. if true means show milliseconds, otherwise, not show milliseconds.
         * @return formatted datetime string described by std::string
         */
        std::string format(datetime_format dfmt = datetime_format::datetime_format_a, bool local = true, bool show_milliseconds = true) const;
        // fmt, usages like std::put_time

        /**
         * @brief Format the datetime object to a string with a custom format string
         *
         * @param fmt datetime format string, default is "%Y-%m-%d %H:%M:%S.%f". usages like std::put_time, reference to https://en.cppreference.com/w/cpp/io/manip/put_time
         * @param local whether to use local time, default is true. if true means output in local time, otherwise, output in UTC time.
         * @param show_milliseconds whether to show milliseconds, default is true. if true means show milliseconds, otherwise, not show milliseconds.
         * @return formatted datetime string described by std::string
         */
        std::string format(const std::string &fmt, bool local = true, bool show_milliseconds = true) const;

        /**
         * @brief Get the string of UTC time described by datetime object
         *
         * @return the string of UTC time in format "YYYY-MM-DD HH:MM:SS.sss"
         */
        std::string utctime() const
        {
            return format(datetime_format::datetime_format_a, false);
        }

        /**
         * @brief Define the assignment operator
         *
         * @param other other datetime object to copy
         * @return datetime& the reference of the current datetime object
         */
        datetime &operator=(const datetime &other);

        /**
         * @brief Define the addition operator
         *
         * @param ms milliseconds to add
         * @return datetime object after adding milliseconds
         */
        datetime operator+(uint64_t ms);

        /**
         * @brief Define the subtraction operator
         *
         * @param ms milliseconds to subtract
         * @return datetime object after subtracting milliseconds
         */
        datetime operator-(uint64_t ms);

        /**
         * @brief Define the addition and assignment operator
         *
         * @param ms milliseconds to add
         * @return datetime& the reference of the current datetime object after adding milliseconds
         */
        datetime &operator+=(uint64_t ms);

        /**
         * @brief Define the subtraction and assignment operator
         *
         * @param ms milliseconds to subtract
         * @return datetime& the reference of the current datetime object after subtracting milliseconds
         */
        datetime &operator-=(uint64_t ms);

        /**
         * @brief Define the subtraction operator between two datetime objects
         *
         * @param other datetime object to subtract
         * @return the duration in milliseconds between current and other datetime objects
         */
        int64_t operator-(const datetime &other) const;

    private:
        using time_regex_type = std::pair<std::string, std::regex>;
        using time_regex_vec_type = std::vector<time_regex_type>;
        static std::string supported_time_formats(const time_regex_vec_type &fmtlist);
        static bool verify_time(const struct tm &time);

    private:
        uint64_t timestamp_ms_;
    };

    /**
     * @brief Define the output stream operator for datetime object
     *
     * @param os the std::ostream object
     * @param dt the datetime object to be output
     * @return std::ostream& the reference of the std::ostream object after outputting the datetime object
     */
    std::ostream &operator<<(std::ostream &os, const datetime &dt);

} // namespace
