#pragma once

#include <algorithm>
#include <cctype>
#include <string>
#include <string_view>
#include <vector>

#include "j2_library/export.hpp" // For J2LIB_API macro definition

namespace j2 
{
    // NOTE: Public API: J2LIB_API is set for static/sharing.

    // Adds two integers and returns the result.
    J2LIB_API int add(int a, int b);

    // Trims leading and trailing whitespace from the input string.
    J2LIB_API std::string trim(std::string_view s);

    // Converts the input string to uppercase.
    J2LIB_API std::string to_upper(std::string_view s);

    // Splits the input string by the given delimiter and returns a vector of substrings.
    J2LIB_API std::vector<std::string> split(std::string_view s, char delim);






} // namespace j2


