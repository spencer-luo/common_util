#include <iostream>
#include <map>
#include <thread>
#include "logger.h"
#include "timeutil.h"

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

    void logger::debug(const std::string &fn_name, const std::string &msg)
    {
        log(LogLevel::DEBUG, fn_name, msg);
    }

    void logger::info(const std::string &fn_name, const std::string &msg)
    {
        log(LogLevel::INFO, fn_name, msg);
    }

    void logger::warn(const std::string &fn_name, const std::string &msg)
    {
        log(LogLevel::WARN, fn_name, msg);
    }

    void logger::error(const std::string &fn_name, const std::string &msg)
    {
        log(LogLevel::ERROR, fn_name, msg);
    }

    void logger::log(LogLevel level, const std::string &fn_name, const std::string &msg)
    {
        if (log_func_)
        {
            log_func_(level, fn_name + " " + msg);
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
        auto threadId = std::this_thread::get_id();
        if (LogLevel::ERROR == level)
        {
            std::cerr << "[" << timestamp_ms() << "]" << loglevel_flag(level) << "]" << threadId << "] " << msg << std::endl;
        }
        else
        {
            std::cout << "[" << timestamp_ms() << "]" << loglevel_flag(level) << "]" << threadId << "] " << msg << std::endl;
        }
    }
} // namespace
