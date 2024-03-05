
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

} // namespace