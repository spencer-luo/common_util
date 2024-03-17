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

    void logger::set_log_func(LogFuncType func)
    {
        log_func_ = func;
    }

    void logger::debug(const std::string &fn_name, const std::string &msg)
    {
        log(loglevel::debug_level, fn_name, msg);
    }

    void logger::info(const std::string &fn_name, const std::string &msg)
    {
        log(loglevel::info_level, fn_name, msg);
    }

    void logger::warn(const std::string &fn_name, const std::string &msg)
    {
        log(loglevel::warn_level, fn_name, msg);
    }

    void logger::error(const std::string &fn_name, const std::string &msg)
    {
        log(loglevel::error_level, fn_name, msg);
    }

    void logger::log(loglevel level, const std::string &fn_name, const std::string &msg)
    {
        if (log_func_)
        {
            log_func_(level, fn_name + " " + msg);
        }
    }

    std::string logger::loglevel_flag(loglevel level)
    {
        static std::map<loglevel, std::string> levelMap = {
            {loglevel::debug_level, "[D]"},
            {loglevel::info_level, "[I]"},
            {loglevel::warn_level, "[W]"},
            {loglevel::error_level, "[E]"},
        };
        auto itr = levelMap.find(level);
        if (itr != levelMap.end())
        {
            return itr->second;
        }
        return "[?]";
    }

    void logger::default_log(loglevel level, const std::string &msg)
    {
        std::lock_guard<std::mutex> lock(log_mtx_);
        auto threadId = std::this_thread::get_id();
        if (loglevel::error_level == level)
        {
            std::cerr << "[" << timestamp_ms() << "]" << loglevel_flag(level) << "]" << threadId << "] " << msg << std::endl;
        }
        else
        {
            std::cout << "[" << timestamp_ms() << "]" << loglevel_flag(level) << "]" << threadId << "] " << msg << std::endl;
        }
    }
} // namespace
