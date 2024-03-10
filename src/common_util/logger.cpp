#include <iostream>
#include <map>
#include <thread>
#include "logger.h"
#include "timeutil.h"
#include "strfmt.h"

namespace cutl
{

    logger::logger()
    {
        log_func_ = std::bind(&logger::default_log, this, std::placeholders::_1, std::placeholders::_2);
    }

    logger::~logger()
    {
        log_func_ = nullptr;
    }

    void logger::set_log_func(LogFunc func)
    {
        log_func_ = func;
    }

    void logger::debug(const std::string &msg)
    {
        if (log_func_)
        {
            log_func_(LogLevel::DEBUG, msg);
        }
    }

    void logger::info(const std::string &msg)
    {
        if (log_func_)
        {
            log_func_(LogLevel::INFO, msg);
        }
    }

    void logger::warn(const std::string &msg)
    {
        if (log_func_)
        {
            log_func_(LogLevel::WARN, msg);
        }
    }

    void logger::error(const std::string &msg)
    {
        if (log_func_)
        {
            log_func_(LogLevel::ERROR, msg);
        }
    }

    std::string logger::loglevel_flag(LogLevel level)
    {
        static std::map<LogLevel, std::string> levelMap = {
            {LogLevel::DEBUG, "[D]"},
            {LogLevel::INFO, "[I]"},
            {LogLevel::WARN, "[W]"},
            {LogLevel::ERROR, "[E]"},
        };
        auto itr = levelMap.find(level);
        if (itr != levelMap.end())
        {
            return itr->second;
        }
        return "[?]";
    }

    void logger::default_log(LogLevel level, const std::string &msg)
    {
        std::lock_guard<std::mutex> lock(log_mtx_);
        auto timetext = fmt_timestamp_ms(timestamp_ms());
        auto threadId = std::this_thread::get_id();
        if (LogLevel::ERROR == level)
        {
            std::cerr << "[" << timetext << "]" << loglevel_flag(level) << "]" << threadId << "] " << msg << std::endl;
        }
        else
        {
            std::cout << "[" << timetext << "]" << loglevel_flag(level) << "]" << threadId << "] " << msg << std::endl;
        }
    }
} // namespace
