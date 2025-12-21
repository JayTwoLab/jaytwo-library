#pragma once

#include "j2_library/export.hpp"

#include "j2_library/schedule/weekly/schedule_types.hpp"
#include "j2_library/schedule/weekly/scheduler_time_base.hpp"

namespace j2::schedule::weekly {

    class J2LIB_API scheduler {
    public:
        explicit scheduler(time_base base = time_base::localtime);

        void add_range(const weekly_range& r);
        bool is_active_now() const;

        const weekly_ranges& ranges() const;

    private:
        scheduler_time_base time_base_;
        weekly_ranges ranges_;
    };

}
