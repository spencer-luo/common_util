#pragma once

#include <cstdint>
#include <string>

namespace cutl
{

// set current thread name
void set_current_thread_name(const std::string& name);

std::string get_current_thread_name();

// get current thread id
int32_t get_current_thread_tid();

} // namespace cutl
