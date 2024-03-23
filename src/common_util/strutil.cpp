#include "strutil.h"

namespace cutl
{
    std::string &to_upper(std::string &text)
    {
        std::transform(text.begin(), text.end(), text.begin(), ::toupper);
        return text;
    }

    std::string &to_lower(std::string &text)
    {
        std::transform(text.begin(), text.end(), text.begin(), ::tolower);
        return text;
    }

    bool starts_with(const std::string &str, const std::string &start, bool ignoreCase)
    {
        int srclen = str.size();
        int startlen = start.size();
        if (srclen < startlen)
        {
            return false;
        }

        std::string temp = str.substr(0, startlen);
        if (ignoreCase)
        {
            std::string startStr = start;
            return to_lower(temp) == to_lower(startStr);
        }
        else
        {
            return temp == start;
        }
    }

    bool ends_with(const std::string &str, const std::string &end, bool ignoreCase)
    {
        int srclen = str.size();
        int endlen = end.size();
        if (srclen < endlen)
        {
            return false;
        }

        std::string temp = str.substr(srclen - endlen, endlen);
        if (ignoreCase)
        {
            std::string endStr = end;
            return to_lower(temp) == to_lower(endStr);
        }
        else
        {
            return temp == end;
        }
    }

    strvec split(const std::string &str, const std::string &pattern)
    {
        strvec res;
        if (str == "")
            return res;
        // 在字符串末尾也加入分隔符，方便截取最后一段
        std::string strs = str + pattern;
        size_t pos = strs.find(pattern);
        int startIndex = 0;

        while (pos != strs.npos)
        {
            std::string temp = strs.substr(startIndex, pos - startIndex);
            res.emplace_back(temp);
            startIndex = pos + 1;
            pos = strs.find(pattern, startIndex);
        }

        return res;
    }

    std::string join(const strvec &strlist)
    {
        // todo
        return "";
    }
    std::string join(const char *__restrict, ...)
    {
        // todo
        return "";
    }

} // namespace cutl