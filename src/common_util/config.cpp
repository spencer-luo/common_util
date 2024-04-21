#include "config.h"
#include "inner/logger.h"
#include "version.h"

namespace cutl
{

    std::string library_name()
    {
        return CUTL_PROJECT_NAME;
    }

    std::string library_version()
    {
        return CUTL_VERSION;
    }

    void library_init(LogFuncType log_func)
    {
        CUTL_LOGGER.set_log_func(log_func);
        CUTL_INFO(library_name() + " library initialized, version:" + library_version());
    }

} // namespace cutl