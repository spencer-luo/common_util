#pragma once

#include <stdint.h>
#include <atomic>
#include <string>

namespace cutl
{
    class timecount
    {
    public:
        timecount(const std::string &func_name);
        ~timecount();

    private:
        std::string func_name_;
        std::atomic<uint64_t> start_time_;
    };
} // namespace