
#include <algorithm>
#include <cctype>

#include "j2_library/core/single_function.hpp"

namespace j2
{
    // 두 정수의 합을 반환합니다.
    int add(int a, int b) {
        return a + b;
    }

    // 좌우 공백 제거
    std::string trim(std::string_view s) {
        auto begin = s.begin();
        auto end = s.end();
        while (begin != end && std::isspace(static_cast<unsigned char>(*begin))) ++begin;
        while (begin != end && std::isspace(static_cast<unsigned char>(*(end - 1)))) --end;
        return std::string(begin, end);
    }

    // 대문자 변환
    std::string to_upper(std::string_view s) {
        std::string out(s);
        std::transform(out.begin(), out.end(), out.begin(),
            [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
        return out;
    }

    // 구분자 분할
    std::vector<std::string> split(std::string_view s, char delim) {
        std::vector<std::string> parts;
        std::string current;
        for (char c : s)
        {
            if (c == delim)
            {
                parts.emplace_back(std::move(current));
                current.clear();
            }
            else
            {
                current.push_back(c);
            }
        }
        parts.emplace_back(std::move(current));
        return parts;
    }



} // namespace j2  

