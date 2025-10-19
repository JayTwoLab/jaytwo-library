#pragma once

#include <string>
#include <ctime>
#include <cstdint>
#include <optional>
#include <chrono>

#include "j2_library/export.hpp"
#include "j2_library/datetime/datetime_common.hpp"
#include "j2_library/datetime/datetime_convert.hpp"

namespace j2::datetime {

    // ---------------- 헬퍼 ----------------
    //

    // 문자열에서 ndig 자리수 정수 읽기
    // 인자:
    //  s: 입력 문자열
    //  pos: 읽기 시작 위치(성공 시 ndig만큼 증가)
    //  ndig: 읽을 자리수
    //  out: 읽은 정수 값을 받을 참조
    // 반환: 성공 시 true, 실패 시 false
    // 예제:
    //  s="2025-10-17", pos=0, ndig=4 → out=2025, pos=4, 반환 true
    //  s="2025-10-17", pos=5, ndig=2 → out=10, pos=7, 반환 true
    J2LIB_API bool read_ndigits(const std::string& s, size_t& pos, int ndig, int& out);

    // format의 pos 위치에서 tok 토큰과 일치하는지 검사
    // 인자:
    //  fmt: 형식 문자열
    //  pos: 검사 시작 위치
    //  tok: 검사할 토큰 문자열(예: "YYYY", "MM" 등)
    // 반환: 일치하면 true, 아니면 false
    // 예제:
    //  fmt="YYYY-MM-DD", pos=0, tok="YYYY" → 반환
    J2LIB_API bool match_token(const std::string& fmt, size_t pos, const char* tok);

    // 윤년 여부 반환
    // 인자:
    //  y: 연도(예: 2024)
    // 반환: 윤년이면 true, 아니면 false
    J2LIB_API bool is_leap(int y);

    // 유효한 연월일(YYYY-MM-DD)인지 검사
    // 인자:
    //  Y: 연도(예: 2025)
    //  M: 월(1~12)
    //  D: 일(1~31)
    // 반환: 유효하면 true, 아니면 false
    // 예제:
    //  valid_ymd(2025, 2, 29) → false
    J2LIB_API bool valid_ymd(int Y, int M, int D);

    // 현재 시각을 지정된 타임존 모드(UTC/Localtime)로 얻기
    // 인자:
    //  tz: 타임존 모드
    // 반환: 현재 시각의 std::tm 구조체
    J2LIB_API std::tm now_in_zone(TimeZoneMode tz);

    // 오늘 날짜(00:00:00)를 지정된 타임존 오프셋(초)으로 얻기
    // 인자:
    //  offset_sec: UTC로부터의 오프셋(초)
    // 반환: 오늘 날짜의 std::tm 구조체
    // 예제:
    //  offset_sec=0 → UTC 오늘 날짜
    //  offset_sec=32400 → UTC+9 오늘 날짜
    J2LIB_API std::tm today_in_fixed_offset_seconds(int offset_sec);

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
    J2LIB_API bool get_utc_tm(const std::chrono::system_clock::time_point& tp, std::tm& out);

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
    J2LIB_API bool get_local_tm(const std::chrono::system_clock::time_point& tp, std::tm& out);

    // ---------------- 포맷터 유틸 ----------------

    // 내부 유틸: 0 채움 숫자 쓰기
    // 인자:
    //  out: 결과 문자열 버퍼
    //  value: 쓸 정수 값
    J2LIB_API void append_ndigits(std::string& out, int value, int width);

    // 내부 유틸: format의 pos 위치에서 tok 토큰과 일치하는지 검사
    // 인자:
    //  fmt: 형식 문자열 
    //  pos: 검사 시작 위치
    //  tok: 검사할 토큰 문자열(예: "YYYY", "MM" 등)
    // 반환: 일치하면 true, 아니면 false
    J2LIB_API bool fmt_match(const std::string& fmt, size_t pos, const char* tok);

    // 내부 유틸: std::tm 구조체를 format에 따라 포맷
    // 인자:
    //  tmv: 포맷할 std::tm 구조체
    //  format: 형식 문자열. 예: "YYYY-MM-DD hh:mm:ss.SSS"
    // 반환: 포맷된 문자열
    J2LIB_API std::string format_from_tm_core(const std::tm& tmv,
        const std::string& format);

    // ---------------- 포맷터 API ----------------

    // (1) std::tm + format
    // 인자:
    //  tmv: 포맷할 std::tm 구조체
    //  format: 형식 문자열. 예: "YYYY-MM-DD hh:mm:ss.SSS"
    // 반환: 포맷된 문자열
    J2LIB_API
        std::string format_datetime(const std::tm& tmv,
            const std::string& format);

    // (2) time_t + tzmode + format
    // 인자:
    //  t: 포맷할 time_t 값
    //  tzmode: t를 UTC/Localtime 중 어느 타임존으로 변환할지 지정
    //  format: 형식 문자열. 예: "YYYY-MM-DD hh:mm:ss.SSS"
    // 반환: 포맷된 문자열
    J2LIB_API
        std::string format_datetime(std::time_t t,
            TimeZoneMode tzmode,
            const std::string& format);

    // (3) time_point + tzmode + format
    // 인자:
    //  tp: 포맷할 time_point 값
    //  tzmode: tp를 UTC/Localtime 중 어느 타임존으로 변환할지 지정
    //  format: 형식 문자열. 예: "YYYY-MM-DD hh:mm:ss.SSS"
    // 반환: 포맷된 문자열    
    J2LIB_API
        std::string format_datetime(const std::chrono::system_clock::time_point& tp,
            TimeZoneMode tzmode,
            const std::string& format);

    // ---------------- 변환 API ----------------
    // (4) std::tm + TimeZoneMode → time_point
    // 인자:
    //  tmv: 변환할 std::tm 구조체
    //  tzmode: tmv가 UTC/Localtime 중 어느 타임존인지
    // 반환: 변환된 time_point
    J2LIB_API
        std::chrono::system_clock::time_point
        to_timepoint(const std::tm& tmv, TimeZoneMode tzmode);

    // (5) time_point + TimeZoneMode → std::tm
    // 인자:
    //  tp: 변환할 time_point 값
    //  tzmode: 변환할 타임존 모드(UTC/Localtime)
    //  out: 변환된 std::tm 구조체를 받을 참조
    // 반환: 성공 시 true, 실패 시 false
    J2LIB_API
        bool to_tm(const std::chrono::system_clock::time_point& tp,
            TimeZoneMode tzmode,
            std::tm& out);



} // namespace j2::datetime

