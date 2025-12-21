#pragma once

#include <string>

#include "j2_library/schedule/weekly/schedule_types.hpp"

namespace j2::schedule::weekly {

    std::string dump_schedule(const weekly_ranges& ranges);

}
