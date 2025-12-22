#pragma once

#include "j2_library/export.hpp"

#include "j2_library/schedule/weekly/schedule_types.hpp"
#include "j2_library/schedule/weekly/scheduler_time_base.hpp"
#include "j2_library/schedule/weekly/schedule_json.hpp"
#include "j2_library/schedule/weekly/schedule_xml.hpp"
#include "j2_library/schedule/weekly/schedule_dump.hpp"
#include "j2_library/schedule/weekly/schedule_normalizer.hpp"

#include <functional>
#include <string>
#include <algorithm>
#include <chrono>

namespace j2::schedule::weekly {

    class J2LIB_API scheduler {
    public:
        explicit scheduler(time_base base = time_base::localtime);

        // 테스트용: 현재 시각을 주입할 수 있는 setter (time_point 기반)
        void set_now_provider(std::function<std::chrono::system_clock::time_point()> now_provider);

        // 호환: 기존 tm 프로바이더를 받아 내부에서 time_point 프로바이더로 래핑
        void set_now_provider(std::function<std::tm()> now_provider_tm);

        // 편의 오버로드: weekday enum 으로 호출 가능
        void set_now(weekday wd, int hour, int minute);

        // 신규: time_point 를 직접 주입하는 편의 오버로드
        void set_now(std::chrono::system_clock::time_point tp);

        // JSON으로부터 스케줄을 로드 (기존 범위는 추가됨)
        void load_from_json(const nlohmann::json& j);

        // 옵셔널: 호출 시점에 제공자를 넘겨서 체크 가능 (time_point 기반)
        bool is_active_now() const;
        bool is_active_now(std::function<std::chrono::system_clock::time_point()> now_provider_tp) const;

        void add_range(const weekly_range& r);

        const weekly_ranges& ranges() const;
         
    private:
        // 내부 헬퍼: 특정 tm 기준으로 활성 여부 판정
        bool is_active_at_tm(const std::tm& tm) const;

        scheduler_time_base time_base_;
        weekly_ranges ranges_;
        // now_provider_는 time_point을 반환하는 프로바이더로 변경
        std::function<std::chrono::system_clock::time_point()> now_provider_;
    };

     // 편의 오버로드: weekday enum 으로 호출 가능하도록 추가
    std::tm make_tm_with_wday_hour_min(
        weekday wd,
        int hour,
        int minute
    );

}
