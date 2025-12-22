#pragma once

#include <chrono>

namespace j2::schedule::weekly {

    enum class time_base {
        localtime,
        utc
    };

    class scheduler_time_base {
    public:
        explicit scheduler_time_base(time_base base = time_base::localtime);

        std::tm now_tm() const;

        time_base get_base() const {
            return base_;
        }

    private:
        time_base base_;
    };

}
