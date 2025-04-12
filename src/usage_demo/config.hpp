#pragma once

#include "common.hpp"
#include "config.h"
#include "strfmt.h"
#include "threadutil.h"
#include "timeutil.h"
#include <iostream>
#include <map>
#include <mutex>
#include <thread>

std::string loglevel2flag(cutl::loglevel level)
{
    static std::map<cutl::loglevel, std::string> levelMap = {
        { cutl::loglevel::debug_level, "[D]" },
        { cutl::loglevel::info_level, "[I]" },
        { cutl::loglevel::warn_level, "[W]" },
        { cutl::loglevel::error_level, "[E]" },
    };
    auto itr = levelMap.find(level);
    if (itr != levelMap.end())
    {
        return itr->second;
    }
    return "[?]";
}

static std::mutex g_log_mtx_;
void static library_log_func(cutl::loglevel level, const std::string& msg)
{
    std::lock_guard<std::mutex> lock(g_log_mtx_);
    auto curTime = cutl::fmt_timestamp_ms(cutl::timestamp(cutl::timeunit::ms));
    // auto threadId = std::this_thread::get_id();
    auto threadId = cutl::get_current_thread_tid();
    if (cutl::loglevel::error_level == level)
    {
        std::cerr << "[" << curTime << "]" << loglevel2flag(level) << "]" << threadId << "](cutl) "
                  << msg << std::endl;
    }
    else if (cutl::loglevel::debug_level == level)
    {
        //  print debug log
    }
    else
    {
        std::cout << "[" << curTime << "]" << loglevel2flag(level) << "]" << threadId << "](cutl) "
                  << msg << std::endl;
    }
}

void LibraryInit()
{
    PrintTitle("library_init");
    cutl::library_init(library_log_func);
}
