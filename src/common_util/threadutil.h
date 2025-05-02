#pragma once

#include <cstdint>
#include <string>

namespace cutl
{

/**
 * @brief Set thread name for current thread.
 *
 * @param name The name to set
 */
void set_current_thread_name(const std::string& name);

/**
 * @brief Get thread name of current thread.
 *
 * @return std::string
 */
std::string get_current_thread_name();

/**
 * @brief Get thread id of current thread.
 * short-id for local system
 * @return int32_t
 */
int32_t get_current_thread_tid();

} // namespace cutl
