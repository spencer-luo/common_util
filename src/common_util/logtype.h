/**
 * @copyright Copyright (c) 2024, Spencer.Luo. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations.
 *
 * @file logtype.h
 * @brief Defines the log level and log function type.
 * @author spencer.luo
 * @date 2024-05-13
 */

#pragma once

#include <string>
#include <functional>

namespace cutl
{
    /**
     * @brief The type of log level.
     *
     */
    enum class loglevel
    {
        /** debug level */
        debug_level,
        /** info level */
        info_level,
        /** warn level */
        warn_level,
        /** error level */
        error_level,
    };

    /**
     * @brief The type of log function
     *
     * @param level the type of log level
     * @param msg the message to log
     * @return void
     */
    using LogFuncType = std::function<void(loglevel, const std::string &)>;

} // namespace cutl