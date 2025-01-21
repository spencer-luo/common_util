/**
 * @copyright Copyright (c) 2025, Spencer.Luo. All Rights Reserved.
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
 * @file dlloader.h
 * @brief Dynamic(Shared) library loader
 * @author Spencer
 * @date 2025-01-21
 */

#pragma once

#include <string>

namespace cutl
{

#if defined(_WIN32)
#include <windows.h>
using dl_handle_t = HMODULE; /* WIN32 handle type */
#else
using dl_handle_t = void*; /* UNIX handle type */
#endif

/**
 * @brief Dynamic(Shared) library loader
 * 
 */
class dlloader
{
public:
    /**
     * @brief Construct a new dlloader object
     * 
     * @param lib_path library path
     */
    dlloader(const std::string& lib_path);
    /**
     * @brief Destroy the dlloader object
     * 
     */
    ~dlloader();

public:
    /**
     * @brief Get the symbol object
     * 
     * @param symbol_name symbol name
     * @return dl_handle_t 
     */
    dl_handle_t get_symbol(const std::string& symbol_name);

private:
    dl_handle_t library_handle_;
};

} // namespace cutl