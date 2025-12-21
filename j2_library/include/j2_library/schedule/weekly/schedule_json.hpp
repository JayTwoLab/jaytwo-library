#pragma once

#include <nlohmann/json.hpp>

#include "j2_library/schedule/weekly/schedule_types.hpp"

namespace j2::schedule::weekly {

    // weekly_ranges -> JSON 변환기
    nlohmann::json to_json(const weekly_ranges& ranges);

    // JSON -> weekly_ranges 파서
    // 입력 형식(배열):
    // [
    //   { "start_day": "Mon" | 0, "start_h": 8, "start_m": 0,
    //     "end_day": "Mon" | 0, "end_h": 9, "end_m": 0 },
    //   ...
    // ]
    weekly_ranges from_json(const nlohmann::json& j);

}
