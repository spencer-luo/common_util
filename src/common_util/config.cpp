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
 * @file config.cpp
 * @brief
 * @author spencer.luo
 * @date 2024-05-13
 */

#include "config.h"
#include "inner/logger.h"
#include "version.h"

namespace cutl
{

    std::string library_name()
    {
        return CUTL_NAME;
    }

    std::string library_version()
    {
        return CUTL_VERSION;
    }

    void library_init(LogFuncType log_func)
    {
        CUTL_LOGGER.set_log_func(log_func);
        CUTL_INFO(library_name() + " library initialized, version:" + library_version());
    }

} // namespace cutl