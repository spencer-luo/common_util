/**
 * @copyright Copyright (c) 2024, Spencer.Luo. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the
 * License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing permissions and
 * limitations.
 *
 * @file timecount.h
 * @brief A simple time counter class to measure the execution time of a function.
 *
 * Measure the execution time of a function by using the feature that a class instance object
 * automatically destructs when it exits it's scope.
 *
 * The time precision is microsecond，use monotone increment time, which presents the duration time
 * between the system start time and the current time.
 * @author spencer.luo
 * @date 2024-05-13
 */

#pragma once

#include <atomic>
#include <cstdint>
#include <string>

namespace cutl
{
/**
 * @brief A simple time counter class for measure the execution time of a function.
 *
 */
class timecount
{
public:
    /**
     * @brief Construct a new timecount object
     * The constructor will record the begin time of the function calling.
     * @param func_name
     */
    timecount(const std::string& func_name);
    /**
     * @brief Destroy the timecount object
     * The desctructor will record the end time of the function calling and calculate the execution
     * time.
     */
    ~timecount();

private:
    std::string func_name_;
    std::atomic<uint64_t> start_time_;
};

// Rename `timecount` to `steady_timecounter`, In order to be compatible with older versions,
// retain the original name.
using steady_timecounter = timecount;

/**
 * @brief A time counter class for counting the CPU usage time of a function
 *
 */
class cpu_timecounter
{
public:
    /**
     * @brief Construct a new cpu timecounter object
     * The constructor will record the begin time of the function calling.
     * @param func_name
     */
    explicit cpu_timecounter(const std::string& func_name);

    /**
     * @brief Destroy the cpu timecounter object
     * The desctructor will record the end time of the function calling and calculate the execution
     * time.
     */
    ~cpu_timecounter();

    // Disable copy and assignment
    cpu_timecounter(const cpu_timecounter&) = delete;
    cpu_timecounter& operator=(const cpu_timecounter&) = delete;

private:
    std::string func_name_;
    std::atomic<uint64_t> start_time_;
};

} // namespace