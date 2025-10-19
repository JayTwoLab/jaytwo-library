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

    J2LIB_API bool read_ndigits(const std::string& s, size_t& pos, int ndig, int& out);

    J2LIB_API bool match_token(const std::string& fmt, size_t pos, const char* tok);

    J2LIB_API bool is_leap(int y);

    J2LIB_API bool valid_ymd(int Y, int M, int D);

    J2LIB_API std::tm now_in_zone(TimeZoneMode tz);

    J2LIB_API std::tm today_in_fixed_offset_seconds(int offset_sec);

    // ---------------- 변환 ----------------

    J2LIB_API std::optional<std::time_t> tm_to_time_utc(std::tm tmv);

    J2LIB_API std::optional<std::time_t> tm_to_time_local(std::tm tmv);

    J2LIB_API bool time_t_to_utc_tm(std::time_t t, std::tm& out);

    J2LIB_API bool time_t_to_local_tm(std::time_t t, std::tm& out);

    J2LIB_API bool get_utc_tm(const std::chrono::system_clock::time_point& tp, std::tm& out);

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

