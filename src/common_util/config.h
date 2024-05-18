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
 * @file config.h
 * @brief Config for common_util library, such as initialization and destory for library.
 * @author spencer.luo
 * @date 2024-05-13
 */

#pragma once

#include "logtype.h"

namespace cutl
{
    /**
     * @brief Get the name of the library.
     *
     * @return library name.
     */
    std::string library_name();

    /**
     * @brief Get the version of the library.
     *
     * @return the version of the library
     */
    std::string library_version();

    /**
     * @brief Initialize the library.
     *
     * @param log_func The function pointer for LogFuncType, register a log function for the library.
     */
    void library_init(LogFuncType log_func);

} // namespace cutl