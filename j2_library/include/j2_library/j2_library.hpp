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

// -- bit components --

#include "j2_library/bit/bit_array.hpp" // For bit_array class definition 

// --  core components --

#include "j2_library/core/single_function.hpp" // For Single Function definitions

// -- directory components --

#include "j2_library/directory/directory.hpp" // For directory manipulation functions

// -- encoding components --

#include "j2_library/encoding/encoding.hpp" // For encoding related functions

// -- ini components --

#include "j2_library/ini/ini.hpp" // For INI parser class definition

// -- macro components --

#include "j2_library/macro/setter_getter.hpp" // For DEFINE_MEMBER_AND_SETTER macro

#include "j2_library/macro/try_opt.hpp" // For TRY_OPT and related macros

// -- overload components --

#include "j2_library/overload/overload.hpp" // For overload utility

// -- string components --

#include "j2_library/string/mutex_string.hpp" // For mutex_string class definition

#include "j2_library/string/string.hpp" // For string manipulation functions

#include "j2_library/string/to_console_encoding.hpp" // For to_console_encoding function

#include "j2_library/string/to_string.hpp" // For to_string and is_equal functions

// -- thread components --

#include "j2_library/thread/dynamic_thread.hpp" // For DynamicThread class definition

// TODO: Add more core components as needed

//----------------------------------------------------------------
namespace j2
{
} // namespace j2
