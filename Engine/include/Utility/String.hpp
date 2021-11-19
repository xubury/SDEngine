#ifndef SD_STRING_HPP
#define SD_STRING_HPP

#include "Utility/Base.hpp"

#include <string>
#include <codecvt>
#include <locale>

namespace SD {

namespace String {

inline SD_API void TrimLeft(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                    [](char ch) { return !std::isspace(ch); }));
}

inline SD_API void TrimRight(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         [](char ch) { return !std::isspace(ch); })
                .base(),
            s.end());
}

inline SD_API void Trim(std::string &s) {
    TrimLeft(s);
    TrimRight(s);
}

inline SD_API std::u32string ConvertToUTF32(const std::string &t_str) {
    typedef std::codecvt_utf8<char32_t> convert_type;
    std::wstring_convert<convert_type, char32_t> converter;

    return converter.from_bytes(t_str);
}

inline SD_API std::string ConvertToUTF8(const std::u32string &wstr) {
    // setup converter
    typedef std::codecvt_utf8<char32_t> convert_type;
    std::wstring_convert<convert_type, char32_t> converter;

    return converter.to_bytes(wstr);
}

}  // namespace String

}  // namespace SD

#endif
