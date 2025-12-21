#pragma once

#include "j2_library/schedule/weekly/schedule_types.hpp"

namespace j2::schedule::weekly {

    class schedule_normalizer {
    public:
        static weekly_ranges normalize(const weekly_ranges& input);
    };

}
