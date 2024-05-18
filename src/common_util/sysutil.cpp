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
 * @file sysutil.cpp
 * @brief
 * @author spencer.luo
 * @date 2024-05-13
 */

#include <map>
#include <iostream>
#include <strutil.h>
#include <cstdlib>
#include "sysutil.h"
#include "inner/logger.h"
#include "inner/system_util.h"
#include "inner/filesystem.h"

namespace cutl
{

    // https://www.cnblogs.com/Forgenvueory/p/12757271.html
    // https://blog.csdn.net/n5/article/details/70143942
    // https://blog.csdn.net/qq_40340448/article/details/122117270
    os_platform platform_type()
    {
#if defined(_WIN32) || defined(__WIN32__)
        // #ifdef _WIN64
        //         std::cout << "64-bit Windows" << std::endl;
        // #else
        //         std::cout << "32-bit Windows" << std::endl;
        // #endif
        return os_platform::os_windows;
#elif defined(__APPLE__) || defined(__MACH__)
        return os_platform::os_macos;
#elif defined(__linux__)
        return os_platform::os_linux;
#elif defined(__unix__)
        return os_platform::os_unix;
#else
        return os_platform::os_unknown;
#endif
    }

    std::string architecture()
    {
#if defined(_WIN32) || defined(__WIN32__)
        return cutl::getenv("PROCESSOR_ARCHITECTURE", "");
#else
        static std::string arch;
        if (arch.empty())
        {
            std::string cmd = "uname -m";
            callcmd(cmd, arch);
            CUTL_DEBUG("cmd: " + cmd + ", result: " + arch);
        }
        return arch;
#endif
    }

    std::string platform_name(os_platform type)
    {
        static std::map<os_platform, std::string> platform_map = {
            {os_platform::os_windows, "Windows"},
            {os_platform::os_macos, "macOS"},
            {os_platform::os_linux, "Linux"},
            {os_platform::os_unix, "Unix"},
            {os_platform::os_unknown, "Unknown"},
        };

        auto iter = platform_map.find(type);
        if (iter != platform_map.end())
        {
            return iter->second;
        }

        return "unknown";
    }

    std::string cpp_stl_version()
    {
        static std::map<long, std::string> version_map = {
            {199711L, "C++98"},
            {201103L, "C++11"},
            {201402L, "C++14"},
            {201703L, "C++17"},
            {202002L, "C++20"},
        };

        std::string stlVersion;
        auto iter = version_map.find(__cplusplus);
        if (iter != version_map.end())
        {
            stlVersion = iter->second;
        }

        return std::to_string(__cplusplus) + " (" + stlVersion + ")";
    }

    uint16_t program_bit()
    {
        int a = 0;
        auto byte = sizeof(&a);
        return byte * 8;
    }

    // https://blog.csdn.net/Frederick_Fung/article/details/115333125?utm_source=miniapp_weixin
    endian endian_type()
    {
        int a = 1;
        int *p = &a;
        uint8_t *pBtye = (uint8_t *)p;
        if (*pBtye == 1)
        {
            return endian::little;
        }
        else
        {
            return endian::big;
        }
    }

    uint16_t byteswap(uint16_t value)
    {
        auto data = (uint8_t *)(&value);
        byteswap(data, 2);
        return value;
    }

    uint32_t byteswap(uint32_t value)
    {
        auto data = (uint8_t *)(&value);
        byteswap(data, 4);
        return value;
    }

    uint64_t byteswap(uint64_t value)
    {
        auto data = (uint8_t *)(&value);
        byteswap(data, 8);
        return value;
    }

    void byteswap(uint8_t *data, uint32_t size)
    {
        uint32_t n = size / 2;
        for (uint32_t i = 0; i < n; i++)
        {
            uint8_t temp = data[i];
            data[i] = data[size - i - 1];
            data[size - i - 1] = temp;
        }
    }

    bool system(const std::string &cmd)
    {
        return call_system(cmd);
    }

    bool callcmd(const std::string &cmd, std::string &result)
    {
        // 读取命令执行结果的最大Buffer长度
        constexpr int MAX_CMD_BUF_LEN = 1024;
        FILE *fp = pipline_open(cmd);
        if (fp == NULL)
        {
            CUTL_ERROR("pipline_open error for cmd:" + cmd);
            return false;
        }

        // 　读取命令执行结果
        char buffer[MAX_CMD_BUF_LEN] = {0};
        char *res = fgets(buffer, sizeof(buffer), fp);
        if (res == NULL)
        {
            CUTL_ERROR("read result error for cmd:" + cmd);
            if (pipline_close(fp) != 0)
            {
                CUTL_ERROR("pipline_close error for cmd:" + cmd);
            }
            return false;
        }

        if (pipline_close(fp) != 0)
        {
            CUTL_ERROR("pipline_close error for cmd:" + cmd);
        }

        result = strip(std::string(buffer));

        return true;
    }

    std::string getenv(const std::string &name, const std::string &default_value)
    {
        const char *text = std::getenv(name.c_str());
        if (text == nullptr)
        {
            CUTL_ERROR("variable [" + name + "] not set, fallback to " + default_value);
            return default_value;
        }

        return std::string(text);
    }

    std::string getcwd()
    {
        return current_program_dir();
    }

    std::string homedir()
    {
#if defined(_WIN32) || defined(__WIN32__)
        return cutl::getenv("USERPROFILE", "");
#else
        return cutl::getenv("HOME", "");
#endif
    }

} // namespace