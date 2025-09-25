#pragma once
#include <string>
#include <string_view>
#include <vector>
#include "export.hpp"

namespace j2
{
    // 공개 API: 정적/공유에 맞게 J2LIB_API가 설정됩니다.

    J2LIB_API int add(int a, int b);

    J2LIB_API std::string trim(std::string_view s);

    J2LIB_API std::string to_upper(std::string_view s);

    J2LIB_API std::vector<std::string> split(std::string_view s, char delim);

} // namespace j2
