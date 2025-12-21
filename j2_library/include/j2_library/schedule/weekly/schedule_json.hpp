#pragma once

#include <nlohmann/json.hpp>

#include "j2_library/schedule/weekly/schedule_types.hpp"

namespace j2::schedule::weekly {

    nlohmann::json to_json(const weekly_ranges& ranges);

}
