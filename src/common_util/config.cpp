#include "config.h"
#include "inner/logger.h"

namespace cutl
{

    std::string library_version()
    {
        // todo
        return "0.1.0";
    }

    void library_init(LogFuncType log_func)
    {
        CUTL_LOGGER.set_log_func(log_func);
        CUTL_INFO("common_util library initialized, version:" + library_version());
    }

} // namespace cutl