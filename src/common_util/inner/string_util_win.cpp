#if defined(_WIN32) || defined(__WIN32__)

#include "string_util.h"

#include <cstdlib>
// #include <string.h>
#include <string>
namespace cutl
{

std::string wstring2string(const std::wstring& ws)
{
    size_t convertedChars = 0;
    size_t nDestSize = ws.size() * 4;
    char* chDest = new char[nDestSize];
    memset(chDest, 0, nDestSize);
    errno_t ret = wcstombs_s(&convertedChars, chDest, nDestSize, ws.c_str(), _TRUNCATE);
    if (ret != 0)
    {
        std::cerr << "wcstombs_s error, ret:" << ret << std::endl;
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
        std::cerr << "mbstowcs_s error, ret:" << ret << std::endl;
    }

    std::wstring wstrResult = wchDest;
    delete[] wchDest;
    return wstrResult;
}

} // namespace cutl

#endif // defined(_WIN32) || defined(__WIN32__)
