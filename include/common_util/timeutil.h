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
 * @file timeutil.h
 * @brief Common time utilities.
 * @author spencer.luo
 * @date 2024-05-13
 */

#pragma once

#include <cstdint>

namespace cutl
{

    /**
     * @brief Time unit enum.
     *
     */
    enum class timeunit
    {
        /** second */
        s,
        /** millisecond */
        ms,
        /** microsecond */
        us,
    };

    /**
     * @brief Get current timestamp.
     *
     * @param unit time unit
     * @return uint64_t timestamp
     */
    uint64_t timestamp(timeunit unit);
    /**
     * @brief Get current clock time for monotone increment time.
     *
     * @param unit time unit
     * @return uint64_t clock time
     */
    uint64_t clocktime(timeunit unit);

    /**
     * @brief Convert time from milliseconds to seconds.
     *
     * @param time time in milliseconds
     * @return uint64_t time in seconds
     */
    uint64_t ms2s(uint64_t time);
    /**
     * @brief Convert time from microseconds to seconds.
     *
     * @param time time in microseconds
     * @return uint64_t time in seconds
     */
    uint64_t us2s(uint64_t time);
    /**
     * @brief Convert time from seconds to milliseconds.
     *
     * @param time time in seconds
     * @return uint64_t time in milliseconds
     */
    uint64_t us2ms(uint64_t time);
    /**
     * @brief Convert time from seconds to microseconds.
     *
     * @param time time in seconds
     * @return uint64_t time in microseconds
     */
    uint64_t s2ms(uint64_t time);
    /**
     * @brief Convert time from seconds to microseconds.
     *
     * @param time time in seconds
     * @return uint64_t time in microseconds
     */
    uint64_t s2us(uint64_t time);
    /**
     * @brief Convert time from milliseconds to microseconds.
     *
     * @param time time in milliseconds
     * @return uint64_t time in microseconds
     */
    uint64_t ms2us(uint64_t time);

} // namespace