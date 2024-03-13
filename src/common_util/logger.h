#pragma once

#include <string>
#include <mutex>
#include <functional>
#include "singleton.h"

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

    class logger
    {
        DECLARE_SINGLETON_REF(logger)

    public:
        void set_log_func(LogFunc func);
        void debug(const std::string &fn_name, const std::string &msg);
        void info(const std::string &fn_name, const std::string &msg);
        void warn(const std::string &fn_name, const std::string &msg);
        void error(const std::string &fn_name, const std::string &msg);
        void log(LogLevel level, const std::string &fn_name, const std::string &msg);

    private:
        std::string loglevel_flag(LogLevel level);
        void default_log(LogLevel level, const std::string &msg);

    private:
        LogFunc log_func_;
        std::mutex log_mtx_;
    };

// define a logger instance for convenience
#define CUTL_LOG cutl::logger::get_instance()

#define CUTL_DEBUG(msg) CUTL_LOG.debug(__FUNCTION__, msg)
#define CUTL_INFO(msg) CUTL_LOG.info(__FUNCTION__, msg)
#define CUTL_WARN(msg) CUTL_LOG.warn(__FUNCTION__, msg)
#define CUTL_ERROR(msg) CUTL_LOG.error(__FUNCTION__, msg)

} // namespace
