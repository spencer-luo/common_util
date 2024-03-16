#pragma once

#include <string>

namespace cutl
{
    enum LogLevel
    {
        DEBUG,
        INFO,
        WARN,
        ERROR
    };

    // log function type
    using LogFunc = std::function<void(LogLevel, const std::string &)>;

} // namespace cutl