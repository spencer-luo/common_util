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
 * @file timecount.cpp
 * @brief
 * @author spencer.luo
 * @date 2024-05-13
 */

#include "timecount.h"
#include "timeutil.h"
#include "strfmt.h"
#include "inner/logger.h"

namespace cutl
{
    timecount::timecount(const std::string &func_name)
        : func_name_(func_name)
    {
        start_time_ = clocktime(timeunit::us);
    }

    timecount::~timecount()
    {
        auto end_time = clocktime(timeunit::us);
        auto duration = end_time - start_time_;
        auto text = "[timecount] " + func_name_ + " used " + fmt_timeduration_us(duration);
        CUTL_LOGGER.info("", text);
    }
} // namespace