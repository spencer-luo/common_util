#if defined(_WIN32)
// do nothing
#else

#include "string_util.h"
#include <cstdlib>
#include <string.h>
#include <string>

namespace cutl
{

std::string wstring2string(const std::wstring& ws)
{
    const wchar_t* wchSrc = ws.c_str();
    size_t nDestSize = wcstombs(NULL, wchSrc, 0) + 1;
    char* chDest = new char[nDestSize];
    memset(chDest, 0, nDestSize);
    wcstombs(chDest, wchSrc, nDestSize);
    std::string strResult = chDest;
    delete[] chDest;
    return strResult;
}

std::wstring string2wstring(const std::string& s)
{
    const char* chSrc = s.c_str();
    size_t nDestSize = mbstowcs(NULL, chSrc, 0) + 1;
    wchar_t* wchDest = new wchar_t[nDestSize];
    wmemset(wchDest, 0, nDestSize);
    mbstowcs(wchDest, chSrc, nDestSize);
    std::wstring wstrResult = wchDest;
    delete[] wchDest;
    return wstrResult;
}

} // namespace cutl

#endif // defined(_WIN32)
