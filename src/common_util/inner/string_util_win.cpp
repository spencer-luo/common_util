#if defined(_WIN32)

#include "inner/logger.h"
#include "string_util.h"
#include <cstdlib>
#include <string>

namespace cutl
{

std::string wstring2string(const std::wstring& ws)
{
    if (ws.empty())
    {
        return std::string();
    }

    size_t convertedChars = 0;
    size_t nDestSize = ws.size() * 4;
    char* chDest = new char[nDestSize];
    memset(chDest, 0, nDestSize);
    errno_t ret = wcstombs_s(&convertedChars, chDest, nDestSize, ws.c_str(), _TRUNCATE);
    if (ret != 0)
    {
        CUTL_ERROR("wcstombs_s error, ret:" + ret);
    }

    std::string strResult = chDest;
    delete[] chDest;
    return strResult;
}

std::wstring string2wstring(const std::string& s)
{
    size_t convertedChars = 0;
    size_t nDestSize = s.size() * 4;
    wchar_t* wchDest = new wchar_t[nDestSize];
    wmemset(wchDest, 0, nDestSize);
    errno_t ret = mbstowcs_s(&convertedChars, wchDest, nDestSize, s.c_str(), _TRUNCATE);
    if (ret != 0)
    {
        CUTL_ERROR("mbstowcs_s error, ret:" + ret);
    }

    std::wstring wstrResult = wchDest;
    delete[] wchDest;
    return wstrResult;
}

} // namespace cutl

#endif // defined(_WIN32)
