#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>

#include "j2_library/export.hpp" // For J2LIB_API macro definition

namespace j2::core
{
    // Trim from start (in place)
    J2LIB_API void ltrim(std::string& s);

    // Trim from end (in place)
    J2LIB_API void rtrim(std::string& s);

    // Trim from both ends (in place)
    J2LIB_API void trim(std::string& s);

    // Trim from start (copying)
    J2LIB_API std::string ltrim_copy(std::string s);

    // Trim from end (copying)
    J2LIB_API std::string rtrim_copy(std::string s);

    // Trim from both ends (copying)
    J2LIB_API std::string trim_copy(std::string s);

    // Replace all occurrences of find_token in text with replace_token
    J2LIB_API std::string replace(const std::string& text, const std::string& find_token, const std::string& replace_token);

    // 문자열을 소문자로 변환
    J2LIB_API std::string to_lower(const std::string& s);

    // 문자열을 대문자로 변환
    J2LIB_API std::string to_upper(const std::string& s);

    // 문자열이 특정 접두사로 시작하는지 확인
    J2LIB_API bool starts_with(const std::string& s, const std::string& prefix);

    // 문자열이 특정 접미사로 끝나는지 확인
    J2LIB_API bool ends_with(const std::string& s, const std::string& suffix);

    // 문자열 분할 (delimiter 기준)
    J2LIB_API std::vector<std::string> split(const std::string& s, char delimiter);

    // 문자열 합치기 (구분자 삽입)
    J2LIB_API std::string join(const std::vector<std::string>& tokens, const std::string& delimiter);

    // 문자열이 비어있거나 공백만 있는지 확인
    J2LIB_API bool is_blank(const std::string& s);

    // 특정 문자 제거
    J2LIB_API std::string remove_chars(const std::string& s, const std::string& chars);

    // 문자열 반복
    std::string repeat(const std::string& s, size_t count);

} // namespace j2::core
