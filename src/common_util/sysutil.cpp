
#include "sysutil.h"
#include <map>

namespace cutl
{

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

} // namespace