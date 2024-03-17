#pragma once

#include "logtype.h"

namespace cutl
{

    std::string library_version();
    void library_init(LogFuncType log_func);

} // namespace cutl