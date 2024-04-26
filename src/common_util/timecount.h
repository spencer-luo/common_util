/**
 * @file timecount.h
 * @brief A simple time counter class to measure the execution time of a function.
 *
 * Measure the execution time of a function by using the feature that a class instance object automatically destructs when it exits it's scope.
 *
 * The time precision is microsecond，use monotone increment time, which presents the duration time between the system start time and the current time.
 *
 * @author spencer.luo
 * @date 2024-04-24
 */

#pragma once

#include <cstdint>
#include <atomic>
#include <string>

namespace cutl
{
    /**
     * @brief A simple time counter class to measure the execution time of a function.
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
        timecount(const std::string &func_name);
        /**
         * @brief Destroy the timecount object
         * The desctructor will record the end time of the function calling and calculate the execution time.
         */
        ~timecount();

    private:
        std::string func_name_;
        std::atomic<uint64_t> start_time_;
    };

} // namespace