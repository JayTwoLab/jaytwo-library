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
#include "j2_library/bit/bit.hpp" 

// --  core components --
#include "j2_library/core/core.hpp" 

// -- date/time components --
#include "j2_library/datetime/datetime.hpp" 

// -- directory components --
#include "j2_library/directory/directory.hpp" 

// -- encoding components --
#include "j2_library/encoding/encoding.hpp"

// -- expected components --
#include "j2_library/expected/expected.hpp"

// -- file components --
#include "j2_library/file/file.hpp"

// -- ini components --
#include "j2_library/ini/ini.hpp" 

// -- json components --
#include "j2_library/json/json.hpp"

// -- logging components --
#include "j2_library/log/log.hpp" 

// -- macro components --
#include "j2_library/macro/macro.hpp"

// -- network components --
#include "j2_library/network/network.hpp"

// -- overload components --
#include "j2_library/overload/overload.hpp" 

// -- queue components --
#include "j2_library/queue/queue.hpp"

// -- schedule components --
#include "j2_library/schedule/schedule.hpp"

// -- string components --
#include "j2_library/string/string.hpp"

// -- thread components --
#include "j2_library/thread/thread.hpp"

// -- uuid components --
#include "j2_library/uuid/uuid.hpp" 

// -- xml components --
#include "j2_library/xml/xml.hpp"



//----------------------------------------------------------------
namespace j2
{
} // namespace j2
