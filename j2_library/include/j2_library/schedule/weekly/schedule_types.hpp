#pragma once

#include <cstdint>
#include <vector>

#include "j2_library/export.hpp"

namespace j2::schedule::weekly {

    enum class weekday : uint8_t {
        mon = 0, tue, wed, thu, fri, sat, sun
    };

    struct J2LIB_API time_hm {
        int hour;
        int minute;
    };

    struct J2LIB_API weekly_range {
        weekday start_day;
        time_hm start_time;
        weekday end_day;
        time_hm end_time;
    };

    using weekly_ranges = std::vector<weekly_range>;

}
