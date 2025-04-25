#pragma once

namespace cutl
{

/**
 * @brief limit the value to the range [min_val, max_val]
 * @note The function has the same functionality as std::clamp() in C++17.
 * @tparam T Type of the variable
 * @param value Variable that needs to be restricted
 * @param min_val The minimum value of the range
 * @param max_val The maximum value of the range
 * @return T The restricted value
 */
template<typename T>
T clamp(const T& value, const T& min_val, const T& max_val) noexcept
{
    // 先限制最大值，再限制最小值
    return std::max(min_val, std::min(value, max_val));
}

} // namespace cutl
