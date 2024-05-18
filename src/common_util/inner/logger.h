#pragma once

#include <string>
#include <mutex>
#include "singleton.h"
#include "logtype.h"

namespace cutl
{

    class logger
    {
        CUTL_SINGLETON_REF(logger)

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

    std::string loginfo(const std::string &filename, int line, const std::string &funcname);

// define a logger instance for convenience
#define CUTL_LOGGER cutl::logger::get_instance()

#define LOG_INFO() loginfo(__FILE__, __LINE__, __FUNCTION__)

#define CUTL_DEBUG(msg) CUTL_LOGGER.debug(LOG_INFO(), msg)
#define CUTL_INFO(msg) CUTL_LOGGER.info(LOG_INFO(), msg)
#define CUTL_WARN(msg) CUTL_LOGGER.warn(LOG_INFO(), msg)
#define CUTL_ERROR(msg) CUTL_LOGGER.error(LOG_INFO(), msg)

} // namespace
