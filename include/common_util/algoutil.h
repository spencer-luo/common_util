/**
 * @copyright Copyright (c) 2025, Spencer.Luo. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the
 * License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing permissions and
 * limitations.
 *
 * @file algoutil.h
 * @brief A supplement to `<algorithm>`, providing some commonly used algorithm functions, such as
 * those that were not available in C++11 but have been added in later versions.
 * @author Spencer
 * @date 2025-05-02
 */
#pragma once

#include <algorithm>

namespace cutl
{

/**
 * @brief limit the value to the range [min_val, max_val], The function has the same functionality
 * as std::clamp() in C++17.
 * @note min_val <= return_value <= max_val.
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

/**
 * @brief Check if the value is within the specified range: [min_val, max_val)
 * @note if min_val <= value < max_val, return true. otherwise, return false.
 * @tparam T Type of the variable
 * @param value Variable to be checked
 * @param min_val The minimum value of the range
 * @param max_val The maximum value of the range
 * @return true
 * @return false
 */
template<typename T>
bool in_range(const T& value, const T& min_val, const T& max_val) noexcept
{
    return (value >= min_val) && (value < max_val);
}

} // namespace cutl
