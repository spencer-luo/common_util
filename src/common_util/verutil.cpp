#include <regex>
#include "verutil.h"
#include "inner/logger.h"

namespace cutl
{

    bool is_version(const std::string &text)
    {
        try
        {
            std::regex versionRule(R"(^\d{1,2}([.]\d{1,2}){0,2}[.]\d{1,3}$)");
            return regex_match(text, versionRule);
        }
        catch (const std::exception &e)
        {
            CUTL_ERROR(e.what());
            return false;
        }
    }

    std::string get_version(const std::string &text)
    {
        try
        {
            std::regex versionRule(R"(\d{1,2}(\.\d{1,2}){0,2}\.\d{1,3})");
            std::smatch result;
            if (regex_search(text, result, versionRule))
            {
                return result.str();
            }
            return "";
        }
        catch (const std::regex_error &e)
        {
            CUTL_ERROR(e.what());
            return "";
        }
    }

    int compare_version(const std::string &v1, const std::string &v2)
    {
        // todo
        return 0;
    }

} // namespace cutl