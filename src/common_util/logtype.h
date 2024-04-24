/**
 * @file logtype.h
 * @brief Defines the log level and log function type.
 * @author spencer.luo
 * @date 2024-04-24
 */

#pragma once

#include <string>
#include <functional>

namespace cutl
{
    /**
     * @brief The type of log level.
     *
     */
    enum class loglevel
    {
        /** debug level */
        debug_level,
        /** info level */
        info_level,
        /** warn level */
        warn_level,
        /** error level */
        error_level,
    };

    /**
     * @brief The type of log function
     *
     * @param level the type of log level
     * @param msg the message to log
     * @return void
     */
    using LogFuncType = std::function<void(loglevel, const std::string &)>;

} // namespace cutl