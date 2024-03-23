#pragma once

#include <string>
#include <vector>

namespace cutl
{
    using strvec = std::vector<std::string>;
    std::string &to_upper(std::string &text);
    std::string &to_lower(std::string &text);

    bool starts_with(const std::string &str, const std::string &start, bool ignoreCase = false);
    bool ends_with(const std::string &str, const std::string &end, bool ignoreCase = false);

    strvec split(const std::string &str, const std::string &pattern);
    std::string join(const strvec &strlist);
    std::string join(const char *__restrict, ...);

} // namespace cutl