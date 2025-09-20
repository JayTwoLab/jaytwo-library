#pragma once
#include <string>
#include <string_view>
#include <vector>
#include "export.hpp"

namespace j2
{
    J2LIB_API int add(int a, int b);
    J2LIB_API std::string trim(std::string_view s);
    J2LIB_API std::string to_upper(std::string_view s);
    J2LIB_API std::vector<std::string> split(std::string_view s, char delim);
}
