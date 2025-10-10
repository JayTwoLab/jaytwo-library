
#include "j2_library/core/string.hpp"

namespace j2::core
{
    // Trim from start (in place)
    void ltrim(std::string& s)
    {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
            }));
    }

    // Trim from end (in place)
    void rtrim(std::string& s)
    {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
            }).base(),
                s.end());
    }

    // Trim from both ends (in place)
    void trim(std::string& s)
    {
        ltrim(s);
        rtrim(s);
    }

    // Trim from start (copying)
    std::string ltrim_copy(std::string s)
    {
        ltrim(s);
        return s;
    }

    // Trim from end (copying)
    std::string rtrim_copy(std::string s)
    {
        rtrim(s);
        return s;
    }

    // Trim from both ends (copying)
    std::string trim_copy(std::string s)
    {
        trim(s);
        return s;
    }

    // Replace all occurrences of find_token in text with replace_token
    std::string replace(const std::string& text, const std::string& find_token, const std::string& replace_token) {
        std::string ret = text;

        size_t i = 0;
        while ((i = ret.find(find_token)) != std::string::npos) {
            ret.replace(i, find_token.size(), replace_token);
        }

        return ret;
    }

    // 문자열을 소문자로 변환
    std::string to_lower(const std::string& s) {
        std::string ret = s;
        std::transform(ret.begin(), ret.end(), ret.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return ret;
    }

    // 문자열을 대문자로 변환
    std::string to_upper(const std::string& s) {
        std::string ret = s;
        std::transform(ret.begin(), ret.end(), ret.begin(),
            [](unsigned char c) { return std::toupper(c); });
        return ret;
    }

    // 문자열이 특정 접두사로 시작하는지 확인
    bool starts_with(const std::string& s, const std::string& prefix) {
        return s.size() >= prefix.size() &&
            std::equal(prefix.begin(), prefix.end(), s.begin());
    }

    // 문자열이 특정 접미사로 끝나는지 확인
    bool ends_with(const std::string& s, const std::string& suffix) {
        return s.size() >= suffix.size() &&
            std::equal(suffix.rbegin(), suffix.rend(), s.rbegin());
    }

    // 문자열 분할 (delimiter 기준)
    std::vector<std::string> split(const std::string& s, char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delimiter)) {
            tokens.push_back(item);
        }
        return tokens;
    }

    // 문자열 합치기 (구분자 삽입)
    std::string join(const std::vector<std::string>& tokens, const std::string& delimiter) {
        if (tokens.empty()) return "";
        std::ostringstream oss;
        for (size_t i = 0; i < tokens.size(); ++i) {
            if (i > 0) oss << delimiter;
            oss << tokens[i];
        }
        return oss.str();
    }

    // 문자열이 비어있거나 공백만 있는지 확인
    bool is_blank(const std::string& s) {
        return std::all_of(s.begin(), s.end(), [](unsigned char ch) {
            return std::isspace(ch);
            });
    }

    // 특정 문자 제거
    std::string remove_chars(const std::string& s, const std::string& chars) {
        std::string ret;
        for (char c : s) {
            if (chars.find(c) == std::string::npos) {
                ret.push_back(c);
            }
        }
        return ret;
    }

    // 문자열 반복
    std::string repeat(const std::string& s, size_t count) {
        std::string ret;
        ret.reserve(s.size() * count);
        for (size_t i = 0; i < count; ++i) {
            ret += s;
        }
        return ret;
    }

} // namespace j2::core
