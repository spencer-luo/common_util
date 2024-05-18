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
 * @file singleton.h
 * @brief Define some macro definitions for singleton pattern implementations.
 * For convenience to use, this file defined some macro definitions about singleton pattern implementations.
 * @author spencer.luo
 * @date 2024-05-13
 */
#pragma once

#include <mutex>

/**
 * @brief Macro definition for constructor and copy-assignment operator.
 *
 */
#undef CUTL_COPY_AND_ASSIGN
#define CUTL_COPY_AND_ASSIGN(classname)    \
    classname(const classname &) = delete; \
    classname &operator=(const classname &) = delete;

/**
 * @brief Macro definition for get singleton instance pointer implaementation.
 *
 */
#undef CUTL_SINGLETON_PTR
#define CUTL_SINGLETON_PTR(classname)                                            \
public:                                                                          \
    static classname *get_instance(bool create_if_needed = true)                 \
    {                                                                            \
        static classname *obj = nullptr;                                         \
        if (!obj && create_if_needed)                                            \
        {                                                                        \
            static std::once_flag flag;                                          \
            std::call_once(flag, [&] { obj = new (std::nothrow) classname(); }); \
        }                                                                        \
        return obj;                                                              \
    }                                                                            \
    ~classname();                                                                \
                                                                                 \
private:                                                                         \
    classname();                                                                 \
    CUTL_COPY_AND_ASSIGN(classname)

/**
 * @brief Macro definition for get singleton instance pointer implementation with default constructor.
 *
 */
#undef CUTL_SINGLETON_PTR_DEFAULT_CTOR
#define CUTL_SINGLETON_PTR_DEFAULT_CTOR(classname)                               \
public:                                                                          \
    static classname *get_instance(bool create_if_needed = true)                 \
    {                                                                            \
        static classname *obj = nullptr;                                         \
        if (!obj && create_if_needed)                                            \
        {                                                                        \
            static std::once_flag flag;                                          \
            std::call_once(flag, [&] { obj = new (std::nothrow) classname(); }); \
        }                                                                        \
        return obj;                                                              \
    }                                                                            \
    ~classname();                                                                \
                                                                                 \
private:                                                                         \
    classname() = default;                                                       \
    CUTL_COPY_AND_ASSIGN(classname)

/**
 * @brief Macro definition for get singleton instance reference implaementation.
 *
 */
#undef CUTL_SINGLETON_REF
#define CUTL_SINGLETON_REF(classname)                            \
public:                                                          \
    static classname &get_instance(bool create_if_needed = true) \
    {                                                            \
        static classname obj;                                    \
        return obj;                                              \
    }                                                            \
    ~classname();                                                \
                                                                 \
private:                                                         \
    classname();                                                 \
    CUTL_COPY_AND_ASSIGN(classname)

/**
 * @brief Macro definition for get singleton instance reference implementation with default constructor.
 *
 */
#undef CUTL_SINGLETON_REF_DEFAULT_CTOR
#define CUTL_SINGLETON_REF_DEFAULT_CTOR(classname)               \
public:                                                          \
    static classname &get_instance(bool create_if_needed = true) \
    {                                                            \
        static classname obj;                                    \
        return obj;                                              \
    }                                                            \
    ~classname();                                                \
                                                                 \
private:                                                         \
    classname() = default;                                       \
    CUTL_COPY_AND_ASSIGN(classname)
