// test_datetime.cpp
// - Google Test를 사용한 단위 테스트
// - 다양한 성공/실패/경계/타임존/분수초/누락 필드 보정 케이스 포함

#include <gtest/gtest.h>
#include "j2_library/datetime/datetime.hpp"

using namespace j2::datetime;

// 테스트 보조: UTC tm 생성 헬퍼
static std::time_t make_utc_time_t(int Y, int M, int D, int h, int m, int s) {
    std::tm tm{}; tm.tm_year = Y - 1900; tm.tm_mon = M - 1; tm.tm_mday = D;
    tm.tm_hour = h; tm.tm_min = m; tm.tm_sec = s; tm.tm_isdst = -1;
#if defined(_WIN32)
    return _mkgmtime(&tm);
#else
#if defined(__USE_MISC) || defined(_GNU_SOURCE) || defined(__APPLE__)
    return timegm(&tm);
#else
    // 최후 수단: 로컬을 UTC로 가정(테스트 환경에 따라 오차 가능)
    return std::mktime(&tm);
#endif
#endif
}

TEST(DateTime_Strict, FullMatch_Localtime) {

    // 시간 문자열을 시간 형식에 맞춰 파싱
    auto r = parse_strict_datetime("2025-10-17 12:34:56.123",
        "YYYY-MM-DD hh:mm:ss.SSS",
        TimeZoneMode::Localtime);

    EXPECT_TRUE(r.ok) << r.error; // 파싱 결과  
    EXPECT_EQ(r.millisecond, 123); // 밀리초

    // 역변환(로컬 tm)은 환경 의존성이 있으므로 존재만 검증
    std::tm loc{}; // C++ 표준 시간(time) 구조체
    // struct std::tm {
    //    int tm_sec;   // 초 (0 ~ 60, 윤초 때문에 60까지 가능)
    //    int tm_min;   // 분 (0 ~ 59)
    //    int tm_hour;  // 시 (0 ~ 23)
    //    int tm_mday;  // 일 (1 ~ 31)
    //    int tm_mon;   // 월 (0 ~ 11, 0=1월)
    //    int tm_year;  // 연도 (1900년 기준, 예: 2025년이면 125)
    //    int tm_wday;  // 요일 (0=일요일, 1=월요일, ...)
    //    int tm_yday;  // 1월 1일부터 지난 일 수 (0 ~ 365)
    //    int tm_isdst; // 서머타임 적용 여부 (양수=적용, 0=미적용, 음수=정보 없음)
    // };

    EXPECT_TRUE(get_local_tm(r, loc)); // 로컬 tm 얻기
    EXPECT_EQ(loc.tm_sec, 56);
}

TEST(DateTime_Strict, MissingTimeFieldsUseBase) {
    // base_tm을 고정하여 결정론적 테스트 수행:
    // 2030-01-02 09:08:07 (UTC 기준)
    std::tm base{};
    base.tm_year = 2030 - 1900;
    base.tm_mon = 0;   // 1월
    base.tm_mday = 2;   // 2일
    base.tm_hour = 9;
    base.tm_min = 8;
    base.tm_sec = 7;
    base.tm_isdst = -1;

    // 형식에 분:초만 제공 → 누락된 연/월/일/시를 base로 보정
    auto r = parse_strict_datetime_with_base("01:02", "mm:ss",
        TimeZoneMode::UTC, base);

    // 파싱 성공 및 broken 존재 여부 확인
    ASSERT_TRUE(r.ok) << r.error;
    ASSERT_TRUE(r.broken.has_value()) << "base 사용 시 broken 은 반드시 존재해야 합니다.";

    // broken 값 확인 (입력 토큰이 아닌 필드는 base 값으로 보정되어야 합니다)
    const std::tm& br = *r.broken;
    EXPECT_EQ(br.tm_year, 2030 - 1900); // 연도: base
    EXPECT_EQ(br.tm_mon, 0);           // 월:   base (1월 → 0)
    EXPECT_EQ(br.tm_mday, 2);           // 일:   base
    EXPECT_EQ(br.tm_hour, 9);           // 시:   base
    EXPECT_EQ(br.tm_min, 1);           // 분:   입력값
    EXPECT_EQ(br.tm_sec, 2);           // 초:   입력값

    // present 플래그 확인: mm, ss만 true, 나머지는 false 여야 함
    EXPECT_FALSE(r.present.Y);
    EXPECT_FALSE(r.present.M);
    EXPECT_FALSE(r.present.D);
    EXPECT_FALSE(r.present.h);
    EXPECT_TRUE(r.present.m);
    EXPECT_TRUE(r.present.s);
    EXPECT_FALSE(r.present.SSS);
}
TEST(DateTime_Strict, LiteralMismatchFails) {

    // 형식과 맞지 않는 문자열 파싱 시도
    auto r = parse_strict_datetime("2025/10-17", "YYYY-MM-DD", TimeZoneMode::UTC);

    EXPECT_FALSE(r.ok);
    EXPECT_FALSE(r.error.empty());
}

TEST(DateTime_ISO, ZuluMilliseconds) {

    // ISO 8601 형식 문자열 파싱 (Z: UTC) 
    auto r = parse_iso8601_datetime("2025-10-17T00:30:05.987Z", TimeZoneMode::Localtime);

    EXPECT_TRUE(r.ok) << r.error;
    EXPECT_EQ(r.millisecond, 987);

    // 예상 epoch 시간 계산 (UTC 기준)
    auto exp = make_utc_time_t(2025, 10, 17, 0, 30, 5);

    ASSERT_NE(exp, (std::time_t)-1);
    EXPECT_EQ(r.epoch, exp);
}

TEST(DateTime_ISO, OffsetPlus0900) {

    // ISO 8601 형식 문자열 파싱 (로컬 타임)
    auto r = parse_iso8601_datetime("2025-10-17 09:30:05+09:00", TimeZoneMode::Localtime); // KST는 +09:00
    EXPECT_TRUE(r.ok) << r.error;

    auto exp = make_utc_time_t(2025, 10, 17, 0, 30, 5); // 오프셋 적용된 UTC 시간
    EXPECT_EQ(r.epoch, exp);     // 2025-10-17 09:30:05+09:00 == 2025-10-17 00:30:05Z
}

TEST(DateTime_ISO, TimeOnlyZ_TodayApplied) {

    // 날짜 없음 + Z → UTC 기준 오늘 날짜 보정
    auto r = parse_iso8601_datetime("12:34:56.001Z");
    EXPECT_TRUE(r.ok) << r.error;
    EXPECT_EQ(r.millisecond, 1);

    // UTC tm에서 시/분/초만 확인
    std::tm utc{};
    ASSERT_TRUE(get_utc_tm(r, utc));
    EXPECT_EQ(utc.tm_hour, 12);
    EXPECT_EQ(utc.tm_min, 34);
    EXPECT_EQ(utc.tm_sec, 56);
}

TEST(DateTime_ISO, TimeOnlyNoOffset_UsesFallbackLocalDate) {

    // 날짜 없음 + 오프셋 없음 → fallback(Localtime) 오늘 날짜 보정
    auto r = parse_iso8601_datetime("23:15", TimeZoneMode::Localtime);
    EXPECT_TRUE(r.ok) << r.error;

    std::tm loc{};
    ASSERT_TRUE(get_local_tm(r, loc));
    EXPECT_EQ(loc.tm_min, 15);
}

TEST(DateTime_Auto, SwitchByLiteral) {

    // ISO 8601 형식으로 파싱
    auto r1 = parse_datetime_auto("2025-10-17T00:00:00Z", "ISO8601", TimeZoneMode::UTC);
    EXPECT_TRUE(r1.ok) << r1.error;

    // 지정된 시간 형식으로 파싱
    auto r2 = parse_datetime_auto("2025-10-17 00:00:00", "YYYY-MM-DD hh:mm:ss", TimeZoneMode::UTC);
    EXPECT_TRUE(r2.ok) << r2.error;
}

TEST(DateTime_Helpers, GetUtcLocalTm) {

    // ISO 8601 형식 문자열 파싱
    auto r = parse_iso8601_datetime("2025-01-01T00:00:00Z");
    ASSERT_TRUE(r.ok);

    std::tm utc{}, loc{};
    EXPECT_TRUE(get_utc_tm(r.timepoint, utc)); // UTC tm 얻기
    EXPECT_TRUE(get_local_tm(r.timepoint, loc)); // 로컬타임 tm 얻기

    EXPECT_EQ(utc.tm_hour, 0); // UTC 시(hour)
    EXPECT_EQ(loc.tm_hour, 9); // 로컬 시(hour)

    EXPECT_EQ(utc.tm_min, 0);
    EXPECT_EQ(utc.tm_sec, 0);
}

TEST(DateTime_Helpers, TimepointOnlyWrapper) {

    // ISO 8601 형식 문자열 파싱
    auto tp_opt = parse_datetime_timepoint("2025-10-17T00:00:00Z", "ISO8601", TimeZoneMode::Localtime);
    EXPECT_TRUE(tp_opt.has_value()); // 값이 있는지 확인

    std::tm utc{};
    EXPECT_TRUE(get_utc_tm(*tp_opt, utc)); // 값이 있을 때만 UTC tm 얻기
    EXPECT_EQ(utc.tm_mday, 17);
}

TEST(DateTime_Strict, MillisecondsOptional) {

    // 밀리초 포함/미포함 모두 파싱 가능
    auto r1 = parse_strict_datetime("2025-01-02 12:34:56.250", "YYYY-MM-DD hh:mm:ss.SSS", TimeZoneMode::UTC);
    EXPECT_TRUE(r1.ok) << r1.error;
    EXPECT_EQ(r1.millisecond, 250);

    // 밀리초 미포함
    auto r2 = parse_strict_datetime("2025-01-02 12:34:56", "YYYY-MM-DD hh:mm:ss", TimeZoneMode::UTC);
    EXPECT_TRUE(r2.ok) << r2.error;
    EXPECT_EQ(r2.millisecond, 0);
}

TEST(DateTime_Errors, RangeChecks) {
    // 범위를 벗어난 값들에 대한 파싱 실패 검증 

    // ISO 8601의 특례: 24:00:00 은 “그 날의 끝(다음 날 00:00:00)”을 의미하는 표기 허용 특례입니다.
    // 이 경우에도 분·초가 반드시 00이어야 하며, 보통은 날짜와 함께 써야 모호하지 않습니다
    // (예: 2025-10-17T24:00:00 = 2025-10-18T00:00:00).
    //  날짜 없이 "hh:mm:ss"만 있는 시간 단독 문자열에서는 24:00:00 을 다음 날로 넘길 수 없으므로,
    // 엄격 모드(strict) 라면 거부하는 편이 일관됩니다.
    EXPECT_FALSE(parse_strict_datetime("24:00:00", "hh:mm:ss", TimeZoneMode::UTC).ok); // 시 범위
    EXPECT_FALSE(parse_strict_datetime("25:00:00", "hh:mm:ss", TimeZoneMode::UTC).ok); // 시 범위

    EXPECT_FALSE(parse_strict_datetime("12:60:00", "hh:mm:ss", TimeZoneMode::UTC).ok); // 분 범위

    // 윤초(leap second)는 지구 자전 속도의 불규칙성을 보정하기 위해
    // 국제원자시(TAI)와 협정세계시(UTC)의 차이를 맞추기 위해 추가하거나 빼는 1초
    // 1972년부터 2016년까지 총 27번 윤초가 추가되었습니다.
    // 가장 최근 윤초는 2016년 12월 31일에 삽입되었습니다.
    // 2025년 현재, 더 이상 윤초를 추가하지 않기로 결정되었으며(국제전기통신연합, 2022년 결정),
    // 2035년까지 윤초는 폐지될 예정입니다.
    EXPECT_FALSE(parse_strict_datetime("12:00:60", "hh:mm:ss", TimeZoneMode::UTC).ok); // 초 범위(윤초 60 처리 주의)
    EXPECT_FALSE(parse_strict_datetime("12:00:61", "hh:mm:ss", TimeZoneMode::UTC).ok); // 초 범위
}

