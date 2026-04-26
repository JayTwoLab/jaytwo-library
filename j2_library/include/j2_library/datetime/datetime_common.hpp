#pragma once

#include <string>
#include <ctime>
#include <cstdint>
#include <optional>
#include <chrono>
#include <cctype>
#include <limits>

#include "j2_library/export.hpp"

namespace j2::datetime {
    enum class time_zone_mode { utc, local_time }; // 타임존 모드
} // namespace j2::datetime

