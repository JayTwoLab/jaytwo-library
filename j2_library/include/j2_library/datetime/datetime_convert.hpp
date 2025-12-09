#pragma once

#include <string>
#include <ctime>
#include <cstdint>
#include <optional>
#include <chrono>

#include "j2_library/export.hpp"
#include "j2_library/datetime/datetime_common.hpp"

// -------------------------------------------------------------
// C/C++ 시간 자료형 
//   std::tm : 년월일 시분초 요일 1년중날자 서머타임 정보. 타임존 정보 없음. 디촐트 값은 1900-01-01 00:00:00. 최소 시간 단위 : 초(second).
//   std::time_t : 1970-01-01 00:00:00 UTC 이후 초(second) 단위 시간. UNIX epoch 시간이라고도 함.
//   std::chrono::system_clock::time_point : 1970-01-01 00:00:00 UTC 기준. 단위 : 나노초(nanoseconds).
// 
// -------------------------------------------------------------
// j2::datetime 변환 유틸리티 사용 예시
// -------------------------------------------------------------
//
// #include "j2_library/datetime/datetime_convert.hpp"
// using namespace j2::datetime;
// using clock = std::chrono::system_clock;
//
// -------------------------------------------------------------
// 1) time_point ↔ time_t
// -------------------------------------------------------------
//
// auto now_tp = clock::now();
// std::time_t t1 = clock::to_time_t(now_tp);        // time_point → time_t
// auto tp1 = clock::from_time_t(t1);                // time_t → time_point
//
// -------------------------------------------------------------
// 2) time_point ↔ std::tm (UTC/Localtime)
//    time_point 와 tm 간 변환하는 표준 함수는 없음. (C++17)
//    c++20 부터는 std::chrono::zoned_time 등을 사용 가능.
// -------------------------------------------------------------
//
// auto now = clock::now();
// std::tm tm_utc{};
// std::tm tm_loc{};
//
// to_tm(now, time_zone_mode::UTC, tm_utc);            // time_point → tm(UTC)
// to_tm(now, time_zone_mode::Local, tm_loc);          // time_point → tm(Local)
//
// std::tm tmv{};
// tmv.tm_year = 2025 - 1900;
// tmv.tm_mon  = 9;  // 10월
// tmv.tm_mday = 19;
// tmv.tm_hour = 15;
// tmv.tm_min  = 30;
// tmv.tm_sec  = 0;
//
// auto tp2 = to_timepoint(tmv, time_zone_mode::Local); // tm(Local) → time_point
//
// std::tm utm{};
// utm.tm_year = 2025 - 1900;
// utm.tm_mon  = 9;
// utm.tm_mday = 19;
// utm.tm_hour = 6;
// utm.tm_min  = 30;
// utm.tm_sec  = 0;
//
// auto tp3 = to_timepoint(utm, time_zone_mode::UTC);   // tm(UTC) → time_point
//
// -------------------------------------------------------------
// 3) std::tm (Localtime) ↔ std::time_t
// -------------------------------------------------------------
//
// std::tm local_tm{};
// local_tm.tm_year = 2025 - 1900;
// local_tm.tm_mon  = 9;
// local_tm.tm_mday = 19;
// local_tm.tm_hour = 15;
// local_tm.tm_min  = 30;
// local_tm.tm_sec  = 0;
//
// auto t_opt1 = tm_to_time_local(local_tm);          // tm(Local) → time_t
// if (t_opt1) {
//     std::tm out{};
//     time_t_to_local_tm(*t_opt1, out);              // time_t → tm(Local)
// }
//
// -------------------------------------------------------------
// 4) std::tm (UTC) ↔ std::time_t
// -------------------------------------------------------------
// std::tm utc_tm{};
// utc_tm.tm_year = 2025 - 1900;
// utc_tm.tm_mon  = 9;
// utc_tm.tm_mday = 19;
// utc_tm.tm_hour = 6;
// utc_tm.tm_min  = 30;
// utc_tm.tm_sec  = 0;
//
// auto t_opt2 = tm_to_time_utc(utc_tm);              // tm(UTC) → time_t
// if (t_opt2) {
//     std::tm out{};
//     time_t_to_utc_tm(*t_opt2, out);                // time_t → tm(UTC)
// }
//
// -------------------------------------------------------------
// NOTE: 멀티스레드 안전성 (Thread-Safety)
//   - 시간 변환을 하는 localtime/gmtime 함수들은 스레드-세이프하지 않음.
// -------------------------------------------------------------
// - Windows: localtime_s / gmtime_s / _mkgmtime 사용
// - POSIX  : localtime_r / gmtime_r / timegm 사용
// - 따라서 tm ↔ time_t ↔ time_point 변환을 멀티스레드 환경에서도 안전하게 사용 가능

namespace j2::datetime {

    // ---------------- 변환 ----------------

    // std::tm → time_t 변환 (UTC 기준)
    // 인자:
    //  tmv: 변환할 std::tm 구조체
    // 반환: 성공 시 time_t 값, 실패 시 std::nullopt
    // 주의: tmv.tm_isdst 필드는 무시됨
    //      내부적으로 timegm(리눅스/맥)/_mkgmtime(윈도우)을 사용
    //      지원하지 않는 플랫폼에서는 실패할 수 있음
    //      변환 실패 가능성이 있는 경우 반환값을 반드시 확인할 것
    // 예제:
    //  tmv={tm_year=125, tm_mon=9, tm_mday=17, tm_hour=0, tm_min=30, tm_sec=5}
    //   → 1749983405 (2025-10-17 00:30:05 UTC)
    J2LIB_API std::optional<std::time_t> tm_to_time_utc(std::tm tmv);

    // std::tm → time_t 변환 (Localtime 기준)
    // 인자:
    //  tmv: 변환할 std::tm 구조체
    // 반환: 성공 시 time_t 값, 실패 시 std::nullopt
    // 주의: tmv.tm_isdst 필드는 무시됨
    //      내부적으로 mktime을 사용
    //      변환 실패 가능성이 있는 경우 반환값을 반드시 확인할 것
    // 예제:
    //  tmv={tm_year=125, tm_mon=9, tm_mday
    //        tm_hour=9, tm_min=30, tm_sec=5}
    //   → 1749952205 (2025-10-17 09:30:05 Localtime, 예: KST) 
    J2LIB_API std::optional<std::time_t> tm_to_time_local(std::tm tmv);

    // time_t → std::tm 변환 (UTC 기준)
    // 인자:
    //  t: 변환할 time_t 값
    //  out: 변환된 std::tm 구조체를 받을 참조
    // 반환: 성공 시 true, 실패 시 false
    // 예제:
    //  t=1749983405 → out={tm_year=125, tm_mon
    //                     tm_mday=17, tm_hour=0, tm_min=30, tm_sec=5}
    J2LIB_API bool time_t_to_utc_tm(std::time_t t, std::tm& out);

    // time_t → std::tm 변환 (Localtime 기준)
    // 인자:
    //  t: 변환할 time_t 값
    //  out: 변환된 std::tm 구조체를 받을 참조
    // 반환: 성공 시 true, 실패 시 false
    // 예제:
    //  t=1749952205 → out={tm_year=125, tm_mon=9,
    //                     tm_mday=17, tm_hour=9, tm_min=30,
    //                     tm_sec=5}
    J2LIB_API bool time_t_to_local_tm(std::time_t t, std::tm& out);

    // time_point → std::tm 변환 (UTC 기준)
    // 인자:
    //  tp: 변환할 time_point 값
    //  out: 변환된 std::tm 구조체를 받을 참조
    // 반환: 성공 시 true, 실패 시 false
    // 예제:
    //  tp=system_clock::time_point corresponding to
    //     1749983405 (2025-10-17 00:30 UTC)
    //   → out={tm_year=125, tm_mon=9,
    //          tm_mday=17, tm_hour=0, tm_min=30,
    //          tm_sec=5}
    J2LIB_API bool get_utc_tm(
        const std::chrono::system_clock::time_point& tp,
        std::tm& out);

    // time_point → std::tm 변환 (Localtime 기준)
    // 인자:
    //  tp: 변환할 time_point 값
    //  out: 변환된 std::tm 구조체를 받을 참조
    // 반환: 성공 시 true, 실패 시 false
    // 예제:
    //  tp=system_clock::time_point corresponding to
    //     1749952205 (2025-10-17 09:30
    //     Localtime, e.g., KST)
    //   → out={tm_year=125, tm_mon=9,
    //          tm_mday=17, tm_hour=9, tm_min=30,
    //          tm_sec=5}
    J2LIB_API bool get_local_tm(
        const std::chrono::system_clock::time_point& tp,
        std::tm& out);

    // ---------------- 변환 API ----------------
    // (4) std::tm + time_zone_mode → time_point
    // 인자:
    //  tmv: 변환할 std::tm 구조체
    //  tzmode: tmv가 UTC/Localtime 중 어느 타임존인지
    // 반환: 변환된 time_point
    // 
    J2LIB_API std::chrono::system_clock::time_point to_timepoint(
        const std::tm& tmv,
        time_zone_mode tzmode);

    // (5) time_point + time_zone_mode → std::tm
    // 인자:
    //  tp: 변환할 time_point 값
    //  tzmode: 변환할 타임존 모드(UTC/Localtime)
    //  out: 변환된 std::tm 구조체를 받을 참조
    // 반환: 성공 시 true, 실패 시 false
    J2LIB_API bool to_tm(
        const std::chrono::system_clock::time_point& tp,
        time_zone_mode tzmode,
        std::tm& out);



} // namespace j2::datetime

