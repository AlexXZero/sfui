#ifndef UTILS_UNICODE_H
#define UTILS_UNICODE_H

#include <string>   // for std::string
#include <codecvt>  // for std::codecvt_utf8
#include <locale>   // for std::wstring_convert

namespace utils {

// from https://stackoverflow.com/questions/4804298/how-to-convert-wstring-into-string
static std::wstring s2ws(const std::string& str)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    static std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.from_bytes(str);
}

static std::string ws2s(const std::wstring& wstr)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    static std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.to_bytes(wstr);
}

}

#endif // UTILS_UNICODE_H
