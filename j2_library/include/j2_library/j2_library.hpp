#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "j2_library/export.hpp" // For J2LIB_API macro definition

// For version information
#if __has_include("j2_library/version.hpp")
#include "j2_library/version.hpp"
#else
// If not, take the alternative action
#endif

#if __has_include("version.hpp")
#include "version.hpp"
#else
// If not, take the alternative action
#endif

// --  core components --

#include "j2_library/core/single_function.hpp" // For Single Function definitions

#include "j2_library/core/mutex_string.hpp" // For mutex_string class definition

#include "j2_library/core/to_string.hpp" // For to_string and is_equal functions

#include "j2_library/core/to_console_encoding.hpp" // For to_console_encoding function

// -- directory components --

#include "j2_library/directory/directory.hpp" // For directory manipulation functions

// TODO: Add more core components as needed

//----------------------------------------------------------------
namespace j2
{
} // namespace j2
