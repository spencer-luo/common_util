/**
 * @file config.h
 * @brief Config for common_util library, such as initialization and destory for library.
 * @author spencer.luo
 * @date 2024-04-23
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