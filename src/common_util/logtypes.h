#pragma once

#include <string>

namespace cutl
{
    enum loglevel
    {
        debug_level,
        info_level,
        warn_level,
        error_level
    };

    // log function type
    using LogFuncType = std::function<void(loglevel, const std::string &)>;

} // namespace cutl