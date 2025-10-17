#pragma once
// datetime.hpp
// - C++17 전용 날짜/시간 파서 유틸
// - 두 가지 모드:
//   (A) 형식 엄격 일치: "YYYY, MM, DD, hh, mm, ss, SSS" 토큰과 리터럴을 정확히 매칭
//       · 형식에 없는 토큰은 tzmode 기준의 현재 날짜/시간으로 보정
//       · 예: "mm:ss" → 시/년월일은 현재, 분/초는 입력값
//   (B) ISO-8601: YYYY-MM-DDThh:mm[:ss][.frac][Z|±HH:MM|±HHMM|±HH]
//       · 날짜가 없고 시간만 → 현재 날짜 보정
//       · 분수초는 1~9자리 허용, 밀리초는 앞 3자리 사용(부족 시 0 패딩)
// - 타임존 처리:
//   · (A) 형식 파서: 인자의 TimeZoneMode(UTC/Localtime) 기준의 현재를 사용
//   · (B) ISO 파서: 문자열의 Z/±오프셋이 우선, 없으면 TimeZoneMode 기준
// - 결과: time_t(초), epoch_ms(밀리초), time_point(system_clock) 제공
//
// 주의:
// - UTC 변환은 timegm(리눅스/맥)/_mkgmtime(윈도우)에 의존합니다.

#include <string>
#include <ctime>
#include <cstdint>
#include <optional>
#include <chrono>

#include "j2_library/export.hpp"

namespace j2::datetime {

    // 타임존 모드
    enum class TimeZoneMode { UTC, Localtime };

    // 형식 파서에서 실제 등장한 토큰 표기
    struct J2LIB_API PresentFlags {
        bool Y = false, M = false, D = false, h = false, m = false, s = false, SSS = false;
    };

    // 결과 구조체
    struct J2LIB_API DateTimeParseResult {
        bool ok = false;                           // 성공 여부
        std::time_t epoch = 0;                     // UNIX epoch (초)
        std::int64_t epoch_ms = 0;                 // UNIX epoch (밀리초)
        std::tm broken{};                          // 변환 직전 tm(보정 포함)
        int millisecond = 0;                       // 밀리초(0~999)
        PresentFlags present{};                    // 형식 파서 전용: 등장 토큰
        std::string error;                         // 실패 사유
        std::chrono::system_clock::time_point timepoint{}; // 결과 시각
    };

    // ---------------- (A) 형식 엄격 일치 파서 ----------------
    //
    // 예: "YYYY-MM-DD hh:mm:ss.SSS", "hh:mm", "mm:ss", "YYYY/MM/DD"
    J2LIB_API
        DateTimeParseResult
        parse_strict_datetime(const std::string& datetime,
            const std::string& format,
            TimeZoneMode tzmode);

    J2LIB_API
        DateTimeParseResult
        parse_strict_datetime_with_base(const std::string& datetime,
            const std::string& format,
            TimeZoneMode tzmode,
            const std::tm& base_tm);

    J2LIB_API
        std::optional<std::int64_t>
        parse_strict_datetime_epoch_ms(const std::string& datetime,
            const std::string& format,
            TimeZoneMode tzmode);

    // ---------------- (B) ISO-8601 파서 ----------------
    //
    // 예: "2025-10-17T00:30:05.123Z"
    //     "2025-10-17 09:30:05+09:00"
    //     "20251017T093005.5+0900"
    //     "12:34:56.789Z"   (날짜 없음 → UTC 오늘)
    //     "23:15"           (날짜 없음/오프셋 없음 → tzmode 기준 오늘)
    J2LIB_API
        DateTimeParseResult
        parse_iso8601_datetime(const std::string& iso8601,
            TimeZoneMode fallback_tzmode = TimeZoneMode::Localtime);

    // ---------------- 자동 판별 ----------------
    //
    // format_or_literal == "ISO8601" 이면 ISO 파서, 아니면 형식 파서
    J2LIB_API
        DateTimeParseResult
        parse_datetime_auto(const std::string& text,
            const std::string& format_or_literal,
            TimeZoneMode tzmode = TimeZoneMode::Localtime);

    // ---------------- 변환/헬퍼 ----------------
    //
    // r.timepoint(또는 임의의 system_clock::time_point)을 UTC/Local tm으로 변환
    J2LIB_API bool get_utc_tm(const DateTimeParseResult& r, std::tm& out);
    J2LIB_API bool get_local_tm(const DateTimeParseResult& r, std::tm& out);
    J2LIB_API bool get_utc_tm(const std::chrono::system_clock::time_point& tp, std::tm& out);
    J2LIB_API bool get_local_tm(const std::chrono::system_clock::time_point& tp, std::tm& out);

    // 자주 쓰는 경우: time_point만 바로 받고 싶을 때
    inline std::optional<std::chrono::system_clock::time_point>
        parse_datetime_timepoint(const std::string& text,
            const std::string& format_or_literal,
            TimeZoneMode tzmode = TimeZoneMode::Localtime)
    {
        auto r = parse_datetime_auto(text, format_or_literal, tzmode);
        if (!r.ok) return std::nullopt;
        return r.timepoint;
    }

} // namespace j2::datetime
