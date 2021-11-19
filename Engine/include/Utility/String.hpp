#ifndef SD_STRING_HPP
#define SD_STRING_HPP

#include "Utility/Base.hpp"

#include <string>
#include <codecvt>
#include <locale>

namespace SD {

namespace String {

inline SD_API void TrimNewline(std::string &s) {
    s.erase(
        std::find_if(s.rbegin(), s.rend(),
                     [](unsigned char ch) { return ch != '\r' || ch != '\n'; })
            .base(),
        s.end());
}

inline SD_API void TrimLeftWhitespace(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                    [](char ch) { return !std::isspace(ch); }));
}

inline SD_API void TrimRightWhitespace(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         [](unsigned char ch) { return !std::isspace(ch); })
                .base(),
            s.end());
}

inline SD_API void TrimWhitespace(std::string &s) {
    TrimLeftWhitespace(s);
    TrimRightWhitespace(s);
}

}  // namespace String

inline SD_API std::wstring StringToWstring(const std::string &t_str) {
    // setup converter
    typedef std::codecvt_utf8<wchar_t> convert_type;
    std::wstring_convert<convert_type, wchar_t> converter;

    // use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
    return converter.from_bytes(t_str);
}

inline SD_API std::string WstringToString(const std::wstring &wstr) {
    // setup converter
    typedef std::codecvt_utf8<wchar_t> convert_type;
    std::wstring_convert<convert_type, wchar_t> converter;

    // use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
    return converter.to_bytes(wstr);
}

}  // namespace SD

#endif
