#pragma once

#include <string>
#include <mutex>
#include <functional>
#include "singleton.h"
#include "logtype.h"

namespace cutl
{

    class logger
    {
        DECLARE_SINGLETON_REF(logger)

    public:
        void set_log_func(LogFuncType func);
        void debug(const std::string &fn_name, const std::string &msg);
        void info(const std::string &fn_name, const std::string &msg);
        void warn(const std::string &fn_name, const std::string &msg);
        void error(const std::string &fn_name, const std::string &msg);
        void log(loglevel level, const std::string &fn_name, const std::string &msg);

    private:
        std::string loglevel_flag(loglevel level);
        void default_log(loglevel level, const std::string &msg);

    private:
        LogFuncType log_func_;
        std::mutex log_mtx_;
    };

// define a logger instance for convenience
#define CUTL_LOGGER cutl::logger::get_instance()

#define CUTL_DEBUG(msg) CUTL_LOGGER.debug(__FUNCTION__, msg)
#define CUTL_INFO(msg) CUTL_LOGGER.info(__FUNCTION__, msg)
#define CUTL_WARN(msg) CUTL_LOGGER.warn(__FUNCTION__, msg)
#define CUTL_ERROR(msg) CUTL_LOGGER.error(__FUNCTION__, msg)

} // namespace
