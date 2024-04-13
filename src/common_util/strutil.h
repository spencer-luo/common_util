#pragma once

#include <string>
#include <vector>

namespace cutl
{
    using strvec = std::vector<std::string>;

    std::string &to_upper(std::string &str);
    std::string &to_lower(std::string &str);

    std::string lstrip(const std::string str);
    std::string rstrip(const std::string str);
    std::string strip(const std::string str);

    bool starts_with(const std::string &str, const std::string &start, bool ignoreCase = false);
    bool ends_with(const std::string &str, const std::string &end, bool ignoreCase = false);

    strvec split(const std::string &str, const std::string &pattern);
    std::string join(const strvec &strlist, const std::string &separator = "");

    std::string desensitizing(const std::string &str);

} // namespace cutl