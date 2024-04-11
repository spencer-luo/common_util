#pragma once

#include <string>

namespace cutl
{

    bool is_version(const std::string &text);
    std::string get_version(const std::string &text);
    int compare_version(const std::string &v1, const std::string &v2);

} // namespace cutl