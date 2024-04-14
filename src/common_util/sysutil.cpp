
#include <map>
#include <strutil.h>
#include <cstdlib>
#include "sysutil.h"
#include "inner/logger.h"

namespace cutl
{

    // TODO: 未在各个平台下验证
    // https://www.cnblogs.com/Forgenvueory/p/12757271.html
    // https : // blog.csdn.net/n5/article/details/70143942
    platform platform_type()
    {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        return platform::windows;
#elif defined(__APPLE__) || defined(__MACH__)
        return platform::macos;
#elif defined(__linux__)
        return platform::linux;
#elif defined(__unix__)
        return platform::unix;
#else
        return platform::unknown;
#endif
    }

    std::string platform_name()
    {
        auto type = platform_type();
        static std::map<platform, std::string> platform_map = {
            {platform::windows, "windows"},
            {platform::macos, "macos"},
            {platform::linux, "linux"},
            {platform::unix, "unix"},
            {platform::unknown, "unknown"},
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

    // TODO: windows下未验证，可能会有问题
    bool system(const std::string &cmd)
    {
        if (cmd.empty())
        {
            CUTL_ERROR("cmd is empty!");
            return false;
        }

        pid_t status;
        status = std::system(cmd.c_str());

        if (-1 == status)
        {
            CUTL_ERROR("system error!");
            return false;
        }

        if (!WIFEXITED(status))
        {
            CUTL_ERROR("exit status:" + std::to_string(WEXITSTATUS(status)));
            return false;
        }

        if (0 != WEXITSTATUS(status))
        {
            CUTL_ERROR("run shell script fail, script exit code:" + std::to_string(WEXITSTATUS(status)));
            return false;
        }

        return true;
    }

    bool callcmd(const std::string &cmd, std::string &result)
    {
        // 读取命令执行结果的最大Buffer长度
        constexpr int MAX_CMD_BUF_LEN = 1024;
        FILE *fp = popen(cmd.c_str(), "r");
        if (fp == NULL)
        {
            CUTL_ERROR("popen error for cmd:" + cmd);
            return false;
        }

        // 　读取命令执行结果
        char buffer[MAX_CMD_BUF_LEN] = {0};
        char *res = fgets(buffer, sizeof(buffer), fp);
        if (res == NULL)
        {
            CUTL_ERROR("read result error for cmd:" + cmd);
            if (pclose(fp) != 0)
            {
                CUTL_ERROR("pclose error for cmd:" + cmd);
            }
            return false;
        }

        if (pclose(fp) != 0)
        {
            CUTL_ERROR("pclose error for cmd:" + cmd);
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

} // namespace