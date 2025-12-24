
#pragma once
#include "j2_library/export.hpp"

namespace j2
{
    // Library Version (manually maintained)
    // Update these values manually when releasing a new version.
    inline constexpr int VersionMajor = 0;
    inline constexpr int VersionMinor = 1;
    inline constexpr int VersionPatch = 0;

    // Keep a header-only string to avoid dllimport/dllexport issues on Windows.
    inline constexpr const char* VersionString = "0.1.0";
}
