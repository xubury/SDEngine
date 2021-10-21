#ifndef SD_STRING_HPP
#define SD_STRING_HPP

#include <string>
#include <codecvt>
#include <locale>

namespace sd {

inline std::wstring stringToWstring(const std::string &t_str) {
    // setup converter
    typedef std::codecvt_utf8<wchar_t> convert_type;
    std::wstring_convert<convert_type, wchar_t> converter;

    // use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
    return converter.from_bytes(t_str);
}

inline std::string wstringToString(const std::wstring &wstr) {
    // setup converter
    typedef std::codecvt_utf8<wchar_t> convert_type;
    std::wstring_convert<convert_type, wchar_t> converter;

    // use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
    return converter.to_bytes(wstr);
}

}  // namespace sd

#endif
