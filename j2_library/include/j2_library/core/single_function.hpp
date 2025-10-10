#pragma once

#include <algorithm>
#include <cctype>
#include <string>
#include <string_view>
#include <vector>

#include "j2_library/export.hpp" // For J2LIB_API macro definition

namespace j2::core {

// NOTE: Public API: J2LIB_API is set for static/sharing.

// Adds two integers and returns the result.
J2LIB_API int add(int a, int b);
 

// TODO: Add more single-function utilities as needed.






} // namespace j2::core
