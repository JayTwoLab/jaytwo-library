#include "j2_library/schedule/weekly/schedule_json.hpp"

namespace j2::schedule::weekly {

    nlohmann::json to_json(const weekly_ranges& ranges) {
        nlohmann::json j = nlohmann::json::array();
        for (const auto& r : ranges) {
            j.push_back({
              {"start_day", (int)r.start_day},
              {"start_h", r.start_time.hour},
              {"start_m", r.start_time.minute},
              {"end_day", (int)r.end_day},
              {"end_h", r.end_time.hour},
              {"end_m", r.end_time.minute}
                });
        }
        return j;
    }

}
