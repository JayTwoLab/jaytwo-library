#include <gtest/gtest.h>

#include "j2_library/schedule/weekly/scheduler.hpp"
#include "j2_library/schedule/weekly/schedule_types.hpp"
#include "j2_library/schedule/weekly/schedule_json.hpp"
#include "j2_library/schedule/weekly/schedule_xml.hpp"
#include "j2_library/schedule/weekly/schedule_dump.hpp"
#include "j2_library/schedule/weekly/schedule_normalizer.hpp"
#include "j2_library/datetime/datetime_convert.hpp"

#include <string>
#include <algorithm>
#include <iostream>

using namespace j2::schedule::weekly;
using namespace j2::datetime;

// 전날부터 다음날로 넘어가는 스케쥴 영역 테스트
TEST(SchedulerCases, Tue23_To_Wed10_Wed09_ActiveAndJsonXml) {

    scheduler s;

    // 화요일 23:00 부터 수요일 10:00 까지
    // 매주마다 반복되는 스케쥴 범위 설정
    weekly_range r{
        weekday::tue,   time_hm{23, 0},
        weekday::wed,   time_hm{10, 0}
    };
    s.add_range(r); // 스케쥴 범위 추가

    // 변경: time_point 프로바이더 사용 (로컬 기준으로 tm -> time_point 변환)
    auto provider_tp = []() {
        std::tm tm = make_tm_with_wday_hour_min(weekday::wed, 9, 0);
        return j2::datetime::to_timepoint(tm, time_zone_mode::local_time);
    };
    s.set_now_provider(provider_tp); // 현재 시각 제공자 설정
    EXPECT_TRUE(s.is_active_now()); // 현재 시간이 스케쥴 범위 내인지 확인 (수요일 09:00 이므로 범위 내부임)   

    // JSON 출력 검사
    auto j = to_json(s.ranges()); // 현재 설정된 스케쥴 범위들을 JSON으로 변환
    nlohmann::ordered_json oj = j;
    std::cout << oj.dump(4) << std::endl << std::endl; // JSON을 보기 좋게 출력

    ASSERT_TRUE(j.is_array());
    ASSERT_EQ(j.size(), 1u);
    EXPECT_EQ(j[0]["start_day"].get<std::string>(), std::string("Tue")); // 변경: 숫자 -> 문자열 기대
    EXPECT_EQ(j[0]["start_h"].get<int>(), 23);
    EXPECT_EQ(j[0]["end_day"].get<std::string>(), std::string("Wed"));   // 변경: 숫자 -> 문자열 기대
    EXPECT_EQ(j[0]["end_h"].get<int>(), 10);

    // XML 출력 검사 (간단히 태그와 속성 포함 여부 확인)
    auto xml = to_xml(s.ranges()); // 현재 설정된 스케쥴 범위들을 XML(문자열)로 변환
    std::cout << xml << std::endl;
    EXPECT_NE(xml.find("WeeklySchedule"), std::string::npos);
    EXPECT_NE(xml.find("start_day=\"Tue\""), std::string::npos); // 변경: 숫자 -> 문자열
    EXPECT_NE(xml.find("end_day=\"Wed\""), std::string::npos);   // 변경: 숫자 -> 문자열
}

TEST(SchedulerCases, Sat23_To_Mon08_Mon07_ActiveAndJsonXml) {
        
    scheduler s;

    // 토요일 23:00 부터 월요일 08:00 까지
    weekly_range r{
        weekday::sat,   time_hm{23, 0},
        weekday::mon,   time_hm{8, 0}
    };
    s.add_range(r); // 스케쥴 범위 추가

    // 변경: time_point 프로바이더 사용
    auto provider_tp = []() {
        std::tm tm = make_tm_with_wday_hour_min(weekday::mon, 7, 0);
        return j2::datetime::to_timepoint(tm, time_zone_mode::local_time);
    };
    s.set_now_provider(provider_tp); // 현재 시각 제공자 설정

    EXPECT_TRUE(s.is_active_now()); // 현재 시간이 스케쥴 범위 내인지 확인

    auto j = to_json(s.ranges()); // 스케쥴 범위들을 JSON으로 변환
    nlohmann::ordered_json oj = j;
    std::cout << oj.dump(4) << std::endl; // JSON을 보기 좋게 출력

    ASSERT_TRUE(j.is_array());
    ASSERT_EQ(j.size(), 1u);
    EXPECT_EQ(j[0]["start_day"].get<std::string>(), std::string("Sat")); // 변경
    EXPECT_EQ(j[0]["start_h"].get<int>(), 23);
    EXPECT_EQ(j[0]["end_day"].get<std::string>(), std::string("Mon"));   // 변경
    EXPECT_EQ(j[0]["end_h"].get<int>(), 8);

    auto xml = to_xml(s.ranges()); // 스케쥴 범위들을 XML(문자열)로 변환
    std::cout << xml << std::endl;
    EXPECT_NE(xml.find("WeeklySchedule"), std::string::npos);
    EXPECT_NE(xml.find("start_day=\"Sat\""), std::string::npos); // 변경
    EXPECT_NE(xml.find("end_day=\"Mon\""), std::string::npos);   // 변경
}

TEST(SchedulerCases, MergeOverlappingRanges_Mon10_13_and_Mon11_12_MergedDump) {

    // 월요일 10:00~13:00     
    weekly_range a{
        weekday::mon,   time_hm{10, 0},
        weekday::mon,   time_hm{13, 0}
    };

    // 월요일 11:00~12:00
    weekly_range b{
        weekday::mon,   time_hm{11, 0},
        weekday::mon,   time_hm{12, 0}
    };

    weekly_ranges in{a, b};
    auto normalized = schedule_normalizer::normalize(in);

    // 병합되면 하나의 항목으로 합쳐져야 함
    ASSERT_EQ(normalized.size(), 1u); // 겹치지 않으면 2

    auto dump = dump_schedule(normalized);

    // dump 형식은 "Mon 10:0 -> Mon 13:0\n"
    EXPECT_NE(dump.find("Mon 10:0 -> Mon 13:0"), std::string::npos);
}

// New test: set_now() updates the internal provider and affects is_active_now()
TEST(SchedulerCases, SetNow_UpdatesTimeAndIsActive) {

    scheduler s;

    // 월요일 08:00 ~ 09:00 스케줄을 준비
    weekly_range r{
        weekday::mon,   time_hm{8, 0},
        weekday::mon,   time_hm{9, 0}
    };
    s.add_range(r); // 스케쥴 범위 추가

    // 시각을 월요일 07:30로 설정하면 비활성
    s.set_now(weekday::mon, 7, 30);
    EXPECT_FALSE(s.is_active_now());

    // 같은 인스턴스에서 시각을 월요일 08:30로 변경하면 활성
    s.set_now(weekday::mon, 8, 30);
    EXPECT_TRUE(s.is_active_now());

    // 다시 화요일로 변경하면 비활성
    s.set_now(weekday::tue, 8, 30);
    EXPECT_FALSE(s.is_active_now());
    
}

// New test: add overlapping ranges via add_range, then call set_now twice and verify
TEST(SchedulerCases, AddRange_MergedThenSetNowSequence) {

    scheduler s(time_base::localtime);

    // overlapping ranges: Mon 10:00-13:00 and Mon 11:00-12:00
    weekly_range a{
        weekday::mon,   time_hm{10, 0},
        weekday::mon,   time_hm{13, 0}
    };
    weekly_range b{
        weekday::mon,   time_hm{11, 0},
        weekday::mon,   time_hm{12, 0}
    };

    // add ranges using add_range (scheduler merges overlaps on insert)
    s.add_range(a);
    s.add_range(b);

    // confirm internal storage merged to single range
    const auto& rs = s.ranges();
    ASSERT_EQ(rs.size(), 1u); // 1 means merged schedule

    // set_now to a time outside the merged interval -> should be inactive
    s.set_now(weekday::mon, 9, 0);
    EXPECT_FALSE(s.is_active_now());

    // then set_now to a time inside merged interval -> should be active
    s.set_now(weekday::mon, 11, 30);
    EXPECT_TRUE(s.is_active_now());
}

// New test: load_from_json should parse JSON and populate schedule (and merge)
TEST(SchedulerCases, LoadFromJson_PopulatesAndMerges) {
    // Build JSON array with two overlapping ranges (string day form)
    nlohmann::json j = nlohmann::json::array();
    j.push_back({
        {"start_day",   "Mon"}, {"start_h", 10},    {"start_m", 0}, // Mon 10:00
        {"end_day",     "Mon"}, {"end_h",   13},    {"end_m",   0}  // Mon 13:00
        });
    j.push_back({
        {"start_day",   "Mon"}, {"start_h", 11},    {"start_m", 0}, // Mon 11:00
        {"end_day",     "Mon"}, {"end_h",   12},    {"end_m",   0}  // Mon 12:00
        });

    scheduler s(time_base::localtime);

    // load JSON into scheduler (should add ranges and merge)
    s.load_from_json(j);

    const auto& rs = s.ranges();
    ASSERT_EQ(rs.size(), 1u); // merged

    // merged interval should cover 10:00..13:00
    EXPECT_EQ(rs[0].start_time.hour, 10);
    EXPECT_EQ(rs[0].end_time.hour, 13);

    // verify is_active_now respects the merged interval
    s.set_now(weekday::mon, 9, 30);
    EXPECT_FALSE(s.is_active_now());

    s.set_now(weekday::mon, 11, 0);
    EXPECT_TRUE(s.is_active_now());
}

// New test: set_now(time_point) 동작 검사
TEST(SchedulerCases, SetNowWithTimePoint_Works) {

    scheduler s(time_base::localtime); // Localtime 기준 스케쥴러 생성

    // 월요일 10:00~12:00 스케줄 추가
    weekly_range r{
        weekday::mon,   time_hm{10, 0},
        weekday::mon,   time_hm{12, 0}
    };
    s.add_range(r); // 스케쥴 범위 추가

    // time_point 생성: 월요일 11:00 (Local 기준)
    auto tp_inside = j2::datetime::to_timepoint(
        j2::schedule::weekly::make_tm_with_wday_hour_min(weekday::mon, 11, 0), 
        j2::datetime::time_zone_mode::local_time
    );
    s.set_now(tp_inside);

    EXPECT_TRUE(s.is_active_now()); // 스케쥴 범위 내인지 검사 

    // time_point 생성: 월요일 09:00 (Local 기준)
    auto tp_outside = j2::datetime::to_timepoint(
        j2::schedule::weekly::make_tm_with_wday_hour_min(weekday::mon, 9, 0), 
        j2::datetime::time_zone_mode::local_time
    );
    s.set_now(tp_outside);

    EXPECT_FALSE(s.is_active_now()); // 스케쥴 범위 내인지 검사

    // 실제 현재 시각으로 설정하는 테스트
    scheduler s_now(time_base::localtime);  
    s_now.set_now(std::chrono::system_clock::now());
    std::cout
        << "Current time in schedule: "
        << std::boolalpha << s_now.is_active_now()
        << std::endl; // 현재 시각이 스케쥴 범위 내인지 출력


}
