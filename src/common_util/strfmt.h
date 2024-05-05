/**
 * @file strfmt.h
 * @brief String format functions. this file defines a series of string-formatted functions.
 * @author spencer.luo
 * @date 2024-04-24
 */

#pragma once

#include <string>
#include <cstdint>
#include <sstream>
#include <iomanip>
#include "timeutil.h"

namespace cutl
{
    /**
     * @brief Format uint64 value to a string with a given width and fill character.
     *
     * @param val the value to be formatted.
     * @param width the width of the formatted string.
     * @param fill the fill character of the formatted string, default is '0'.
     * @return std::string the formatted string.
     */
    std::string fmt_uint(uint64_t val, uint8_t width = 0, char fill = '0');
    /**
     * @brief Format double value to a string with a given precision.
     *
     * @param val the value to be formatted.
     * @param precision the precision of the formatted string, default is 2.
     * @return std::string the formatted string.
     */
    std::string fmt_double(double val, int precision = 2);

    /**
     * @brief Format a file size to a human-readable string with a given precision.
     *
     * @param size the size to be formatted.
     * @param simplify whether to use a simplify unit.
     * @param precision the precision of the formatted string, default is 1.
     * @return std::string the formatted string.
     */
    std::string fmt_filesize(uint64_t size, bool simplify = true, int precision = 1);

    /**
     * @brief Format a time duration to a human-readable string.
     *
     * @param seconds the duration in seconds.
     * @return std::string the formatted string.
     */
    std::string fmt_timeduration_s(uint64_t seconds);
    /**
     * @brief Format a time duration to a human-readable string.
     *
     * @param microseconds the duration in microseconds.
     * @return std::string the formatted string.
     */
    std::string fmt_timeduration_ms(uint64_t microseconds);
    /**
     * @brief Format a time duration to a human-readable string.
     *
     * @param nanoseconds the duration in nanoseconds.
     * @return std::string the formatted string.
     */
    std::string fmt_timeduration_us(uint64_t nanoseconds);

    /**
     * @brief Format a timestamp to a human-readable string with a given format.
     *
     * @param second the timestamp in seconds.
     * @param local whether to use local time or UTC time, default is local time.
     * If local is true, the function will format the timestamp to local time, otherwise, it will format the timestamp to UTC time.
     * @param fmt the format of the formatted string. useage like std::put_time, see https://en.cppreference.com/w/cpp/io/manip/put_time
     * @return std::string the formatted string.
     */
    std::string fmt_timestamp(uint64_t second, bool local, const std::string &fmt);
    /**
     * @brief Format a timestamp to a human-readable string.
     *
     * @param second the timestamp in seconds.
     * @param local whether to use local time or UTC time, default is local time.
     * If local is true, the function will format the timestamp to local time, otherwise, it will format the timestamp to UTC time.
     * @return std::string the formatted string.
     */
    std::string fmt_timestamp_s(uint64_t second, bool local = true);
    /**
     * @brief Format a timestamp to a human-readable string.
     *
     * @param ms the timestamp in milliseconds.
     * @param local whether to use local time or UTC time, default is local time.
     * If local is true, the function will format the timestamp to local time, otherwise, it will format the timestamp to UTC time.
     * @return std::string the formatted string.
     */
    std::string fmt_timestamp_ms(uint64_t ms, bool local = true);
    /**
     * @brief Format a timestamp to a human-readable string.
     *
     * @param us the timestamp in microseconds.
     * @param local whether to use local time or UTC time, default is local time.
     * If local is true, the function will format the timestamp to local time, otherwise, it will format the timestamp to UTC time.
     * @return std::string the formatted string.
     */
    std::string fmt_timestamp_us(uint64_t us, bool local = true);

    /**
     * @brief Format data to a hex string.
     *
     * @param data the data to be formatted.
     * @param len the length of the data.
     * @param upper whether to use upper case or lower case for hex characters, default is upper case.
     * @param separator the separator between each pair of hex characters, default is space.
     * @return std::string the formatted string.
     */
    std::string to_hex(const uint8_t *data, size_t len, bool upper = true, char separator = ' ');
    /**
     * @brief Format a uint8_t value to a hex string.
     *
     * @param value the value to be formatted.
     * @param upper whether to use upper case or lower case for hex characters, default is upper case.
     * @param prefix the prefix of the formatted string, default is empty.
     * @return std::string the formatted string.
     */
    std::string to_hex(uint8_t value, bool upper = true, const std::string &prefix = "");
    /**
     * @brief Format a uint16_t value to a hex string.
     *
     * @param value the value to be formatted.
     * @param upper whether to use upper case or lower case for hex characters, default is upper case.
     * @param prefix the prefix of the formatted string, default is empty.
     * @return std::string the formatted string.
     */
    std::string to_hex(uint16_t value, bool upper = true, const std::string &prefix = "");
    /**
     * @brief Format a uint32_t value to a hex string.
     *
     * @param value the value to be formatted.
     * @param upper whether to use upper case or lower case for hex characters, default is upper case.
     * @param prefix the prefix of the formatted string, default is empty.
     * @return std::string the formatted string.
     */
    std::string to_hex(uint32_t value, bool upper = true, const std::string &prefix = "");
    /**
     * @brief Format a uint64_t value to a hex string.
     *
     * @param value the value to be formatted.
     * @param upper whether to use upper case or lower case for hex characters, default is upper case.
     * @param prefix the prefix of the formatted string, default is empty.
     * @return std::string the formatted string.
     */
    std::string to_hex(uint64_t value, bool upper = true, const std::string &prefix = "");

    /**
     * @brief Format uint8_t value to a binary string.
     *
     * @param value the value to be formatted.
     * @param separator the separator between each pair of binary characters, default is comma.
     * @return std::string the formatted string.
     */
    std::string to_bin(uint8_t value, char separator = ',');
    /**
     * @brief Format uint16_t value to a binary string.
     *
     * @param value the value to be formatted.
     * @param separator the separator between each pair of binary characters, default is space.
     * @return std::string the formatted string.
     */
    std::string to_bin(uint16_t value, char separator = ' ');
    /**
     * @brief Format uint32_t value to a binary string.
     *
     * @param value the value to be formatted.
     * @param separator the separator between each pair of binary characters, default is space.
     * @return std::string the formatted string.
     */
    std::string to_bin(uint32_t value, char separator = ' ');
    /**
     * @brief Format uint64_t value to a binary string.
     *
     * @param value the value to be formatted.
     * @param separator the separator between each pair of binary characters, default is space.
     * @return std::string the formatted string.
     */
    std::string to_bin(uint64_t value, char separator = ' ');

} // namespace