#pragma once

#include "j2_library/schedule/weekly/schedule_types.hpp"

namespace j2::schedule::weekly {

    // 내부 표현: 요일(0=Mon..6=Sun), 시작분, 종료분 (0..1440)
    struct interval {
        int day;
        int start; // inclusive minutes
        int end;   // inclusive minutes
    };

    class schedule_normalizer {
    public:
        static weekly_ranges normalize(const weekly_ranges& input);
    };

}
