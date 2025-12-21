
#include "j2_library/schedule/weekly/scheduler.hpp"

namespace j2::schedule::weekly {

    scheduler::scheduler(time_base base)
        : time_base_(base) {
    }

    void scheduler::add_range(const weekly_range& r) {
        ranges_.push_back(r);
    }

    const weekly_ranges& scheduler::ranges() const {
        return ranges_;
    }

    bool scheduler::is_active_now() const {
        auto tm = time_base_.now_tm();
        int now_day = (tm.tm_wday + 6) % 7; // mon=0
        int now_min = tm.tm_hour * 60 + tm.tm_min;

        for (const auto& r : ranges_) {
            int sd = static_cast<int>(r.start_day);
            int ed = static_cast<int>(r.end_day);

            int sm = r.start_time.hour * 60 + r.start_time.minute;
            int em = r.end_time.hour * 60 + r.end_time.minute;

            bool day_match =
                (sd <= ed && now_day >= sd && now_day <= ed) ||
                (sd > ed && (now_day >= sd || now_day <= ed));

            if (!day_match) continue;

            if (sd == ed) {
                if (now_min >= sm && now_min <= em) return true;
            }
            else if (now_day == sd) {
                if (now_min >= sm) return true;
            }
            else if (now_day == ed) {
                if (now_min <= em) return true;
            }
            else {
                return true;
            }
        }
        return false;
    }

}
