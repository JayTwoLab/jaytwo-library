
#include <chrono>
#include <ctime>
#include <cstdlib>

#include <gtest/gtest.h>

#include "j2_library/datetime/v1/date_time.hpp"

TEST(DateTimeV1Test, FormattingAndDayOfWeek) {

    using date_time_v1 = j2::datetime::v1::date_time;

    auto now = date_time_v1::current_date_time(); // 현재 시간

    GTEST_LOG_(INFO) << "Local: " << now.to_string() << " (" << now.day_name() << ")";
    GTEST_LOG_(INFO) << "UTC: " << now.to_utc().to_string() << " (" << now.to_utc().day_name() << ")";
    GTEST_LOG_(INFO) << "Offset: " << date_time_v1::utc_offset_string();
    EXPECT_FALSE(now.to_string().empty());
    // EXPECT_GE(now.day_of_week(), 0); // 0=Sunday, 6=Saturday
}

TEST(DateTimeV1Test, Arithmetic) {
    auto now = j2::datetime::v1::date_time::current_date_time(); // 현재 시간
    auto later = now.add_days(1).add_hours(2); // + 1일 2시간 후의 미래 시간

    auto span = now.secs_to(later);
    EXPECT_EQ(span.total_msecs(), (24 + 2) * 3600 * 1000LL);
}

TEST(DateTimeV1Test, SubtractDateTimesProducesTimeSpan) {
    // t1 - t2 -> time_span
    auto t1 = j2::datetime::v1::date_time::current_date_time();
    auto t2 = t1.add_hours(-5); // 5 hours earlier

    j2::datetime::v1::time_span span = t1 - t2;

    EXPECT_EQ(span.total_msecs(), 5 * 3600 * 1000LL);
    EXPECT_EQ(span.total_secs(), 5 * 3600LL);

    // reverse order yields negative span
    j2::datetime::v1::time_span neg = t2 - t1;

    EXPECT_EQ(neg.total_msecs(), -5 * 3600 * 1000LL);
    EXPECT_EQ(neg.total_secs(), -5 * 3600LL);
}

// utc vs local spec 간 span 
TEST(DateTimeV1Test, Subtract_SameEpoch_DifferentSpec_IsZero) {

    using date_time_v1 = j2::datetime::v1::date_time;
    using time_spec_v1 = j2::datetime::v1::time_spec;

    // 같은 epoch 밀리초로 생성했을 때 spec이 달라도 내부 tp_가 같으므로 차이는 0
    long long epoch_ms = 1'642'000'000'000LL; // 예시 epoch (밀리초)
    auto dt_utc = date_time_v1::from_epoch_msecs(epoch_ms, time_spec_v1::utc);
    auto dt_local = date_time_v1::from_epoch_msecs(epoch_ms, time_spec_v1::local_time);

    auto diff1 = dt_utc - dt_local; // UTC - Local
    auto diff2 = dt_local - dt_utc; // Local - UTC

    EXPECT_EQ(diff1.total_msecs(), 0LL);
    EXPECT_EQ(diff2.total_msecs(), 0LL);
}

TEST(DateTimeV1Test, Subtract_SameWallClock_DifferentSpec_EqualsMinusUtcOffset) {

    using date_time_v1 = j2::datetime::v1::date_time;
    using time_spec_v1 = j2::datetime::v1::time_spec;

    // 동일한 달력(년/월/일/시/분/초)을 UTC로 해석한 값과
    // 로컬로 해석한 값의 tp_ 차이는 타임존 오프셋의 부호 반대임
    std::tm tm1 = {};
    tm1.tm_year = 2026 - 1900;
    tm1.tm_mon = 4 - 1; // April
    tm1.tm_mday = 26;
    tm1.tm_hour = 12;
    tm1.tm_min = 0;
    tm1.tm_sec = 0;

    // from_tm 수정으로 tm이 변경될 수 있으니 복사본 사용
    std::tm tm_for_local = tm1;
    std::tm tm_for_utc = tm1;

    auto dt_local = date_time_v1::from_tm(&tm_for_local, time_spec_v1::local_time);
    auto dt_utc = date_time_v1::from_tm(&tm_for_utc, time_spec_v1::utc);

    // local - utc == -utc_offset_seconds() (초 단위)
    long long diff_secs = (dt_local - dt_utc).total_secs();
    long long utc_off_secs = date_time_v1::utc_offset_seconds();
    EXPECT_EQ(diff_secs, -utc_off_secs);

    // 밀리초 단위 검증
    long long diff_msecs = (dt_local - dt_utc).total_msecs();
    EXPECT_EQ(diff_msecs, -utc_off_secs * 1000LL);
}


static j2::datetime::v1::date_time make_known_utc_datetime() {

    using date_time_v1 = j2::datetime::v1::date_time;
    using time_spec_v1 = j2::datetime::v1::time_spec;

    // 2020-01-02 03:04:05 UTC + 123 ms
    std::time_t t = 1577934245; // epoch for 2020-01-02 03:04:05 UTC

    // time point 계산: time_t(t)를 time_point으로 변환 후, +123 ms 를 더함
    date_time_v1::time_point tp = std::chrono::time_point_cast<std::chrono::milliseconds>(date_time_v1::clock::from_time_t(t)) + std::chrono::milliseconds(123);

    return date_time_v1::from_time_point(tp, time_spec_v1::utc);
}

static j2::datetime::v1::date_time make_known_local_datetime() {

    using date_time_v1 = j2::datetime::v1::date_time;
    using time_spec_v1 = j2::datetime::v1::time_spec;

    // Construct local time 2020-01-02 12:34:56 + 789 ms (interpreted as local time)
    std::tm tm_local = {};
    tm_local.tm_year = 2020 - 1900;
    tm_local.tm_mon = 1 - 1; // January
    tm_local.tm_mday = 2;
    tm_local.tm_hour = 12;
    tm_local.tm_min = 34;
    tm_local.tm_sec = 56;

    std::tm tmp = tm_local;
    std::time_t t = std::mktime(&tmp);

    // time point 계산: time_t(t)를 time_point으로 변환 후, +789 ms 를 더함
    date_time_v1::time_point tp = std::chrono::time_point_cast<std::chrono::milliseconds>(date_time_v1::clock::from_time_t(t)) + std::chrono::milliseconds(789);

    return date_time_v1::from_time_point(tp, time_spec_v1::local_time);
}

TEST(DateTimeV1Test, FromEpochMsecsAndToEpochMsecs) {

    using date_time_v1 = j2::datetime::v1::date_time;
    using time_spec_v1 = j2::datetime::v1::time_spec;

    long long ms = 1577934245LL * 1000LL + 123LL;
    auto dt_utc = date_time_v1::from_epoch_msecs(ms, time_spec_v1::utc); // 2020-01-02 03:04:05.123 UTC

    EXPECT_EQ(dt_utc.to_epoch_msecs(), ms);
    EXPECT_EQ(dt_utc.spec(), time_spec_v1::utc);
    EXPECT_EQ(dt_utc.year(), 2020);
    EXPECT_EQ(dt_utc.msec(), 123);

    auto dt_local = date_time_v1::from_epoch_msecs(ms, time_spec_v1::local_time); // 2020-01-02 12:34:56.789 Local (같은 epoch 밀리초로 local time 해석)
    EXPECT_EQ(dt_local.to_epoch_msecs(), ms);
    EXPECT_EQ(dt_local.spec(), time_spec_v1::local_time);
}

TEST(DateTimeV1Test, ConstructionAndAccessors) {

    using date_time_v1 = j2::datetime::v1::date_time;
    using time_spec_v1 = j2::datetime::v1::time_spec;

    auto dt = make_known_utc_datetime(); // 2020-01-02 03:04:05.123 UTC

    EXPECT_EQ(dt.spec(), time_spec_v1::utc);
    EXPECT_TRUE(dt.is_utc());

    EXPECT_EQ(dt.year(), 2020);
    EXPECT_EQ(dt.month(), 1);
    EXPECT_EQ(dt.day(), 2);
    EXPECT_EQ(dt.hour(), 3);
    EXPECT_EQ(dt.minute(), 4);
    EXPECT_EQ(dt.second(), 5);
    EXPECT_EQ(dt.msec(), 123);

    auto tm = dt.to_tm();
    EXPECT_EQ(tm.tm_year + 1900, dt.year());
    EXPECT_EQ(tm.tm_mon + 1, dt.month()); // tm_mon은 0인 경우 1월
    EXPECT_EQ(tm.tm_mday, dt.day());

    EXPECT_EQ(dt.day_of_week(), tm.tm_wday);
    EXPECT_EQ(dt.day_of_week(), 4); // 2020-01-02는 목요일 (0=일요일, 4=목요일)
    EXPECT_FALSE(dt.day_name(true).empty());
    EXPECT_FALSE(dt.day_name(false).empty());

    EXPECT_EQ(dt.to_string("%Y-%m-%d %H:%M:%S"), std::string("2020-01-02 03:04:05"));
}

TEST(DateTimeV1Test, MakeKnownLocalDatetime) {

    using date_time_v1 = j2::datetime::v1::date_time;
    using time_spec_v1 = j2::datetime::v1::time_spec;

    auto dt = make_known_local_datetime(); // 2020-01-02 12:34:56.789 Local
    EXPECT_EQ(dt.spec(), time_spec_v1::local_time);
    EXPECT_FALSE(dt.is_utc());

    EXPECT_EQ(dt.year(), 2020);
    EXPECT_EQ(dt.month(), 1);
    EXPECT_EQ(dt.day(), 2);
    EXPECT_EQ(dt.hour(), 12);
    EXPECT_EQ(dt.minute(), 34);
    EXPECT_EQ(dt.second(), 56);
    EXPECT_EQ(dt.msec(), 789);

    auto tm = dt.to_tm();
    EXPECT_EQ(tm.tm_year + 1900, dt.year());
    EXPECT_EQ(tm.tm_mon + 1, dt.month());
    EXPECT_EQ(tm.tm_mday, dt.day());
}

TEST(DateTimeV1Test, TimePointAndTimeT) {

    using date_time_v1 = j2::datetime::v1::date_time;
    using time_spec_v1 = j2::datetime::v1::time_spec;

    auto dt = make_known_utc_datetime(); // 2020-01-02 03:04:05.123 UTC
    auto tp = dt.to_time_point(); // date_time에서 time_point으로 변환

    auto dt2 = date_time_v1::from_time_point(tp, dt.spec()); // 같은 time_point과 spec으로부터 date_time 생성
    EXPECT_EQ(dt2.year(), dt.year()); // 2020
    EXPECT_EQ(dt2.month(), dt.month()); // 1
    EXPECT_EQ(dt2.day(), dt.day()); // 2
    EXPECT_EQ(dt2.msec(), dt.msec()); // 123

    std::time_t t = dt.to_time_t(); // date_time에서 time_t로 변환 (초 단위)
    auto dt3 = date_time_v1::from_time_t(t, time_spec_v1::utc); // time_t에서 date_time으로 변환
    EXPECT_EQ(dt3.year(), 2020);
    EXPECT_EQ(dt3.hour(), 3);
    EXPECT_EQ(dt3.second(), 5);
    EXPECT_EQ(dt3.msec(), 0); // time_t는 초 단위이므로 msec는 0
}

TEST(DateTimeV1Test, ConversionsAndSpec) {

    using date_time_v1 = j2::datetime::v1::date_time;
    using time_spec_v1 = j2::datetime::v1::time_spec;

    auto dt_utc = make_known_utc_datetime(); // 2020-01-02 03:04:05.123 UTC

    auto dt_local = dt_utc.to_local_time(); // 같은 epoch 밀리초로 local time 해석
    EXPECT_EQ(dt_local.spec(), time_spec_v1::local_time);
    EXPECT_NE(dt_local.is_utc(), dt_utc.is_utc());

    auto back = dt_local.to_utc(); // 다시 UTC로 변환하면 원래의 UTC와 같은 epoch 밀리초로 해석되어야 함
    EXPECT_EQ(back.spec(), time_spec_v1::utc); 
}

TEST(DateTimeV1Test, UtcOffsetFormatting) {

    using date_time_v1 = j2::datetime::v1::date_time;
    using time_spec_v1 = j2::datetime::v1::time_spec;

    long long offs = date_time_v1::utc_offset_seconds(); // 예: +3600, -18000 등
    EXPECT_EQ(offs % 60, 0);

    auto s1 = date_time_v1::utc_offset_string(false); // long format (e.g. "+9", "-5")
    auto s2 = date_time_v1::utc_offset_string(true); // short format (e.g. "+09:00", "-05:00")
    EXPECT_FALSE(s1.empty());
    EXPECT_FALSE(s2.empty());
    EXPECT_NE(s2.find(':'), 0u == 0u ? std::string::npos : std::string::npos);
}

TEST(DateTimeV1Test, UtcOffsetHelpers) {

    using date_time_v1 = j2::datetime::v1::date_time;
    using time_spec_v1 = j2::datetime::v1::time_spec;

    long long secs = date_time_v1::utc_offset_seconds(); // 예: +3600, -18000 등
    long long mins = date_time_v1::utc_offset_minutes(); // 예: +60, -300 등
    int hrs = date_time_v1::utc_offset_hours(); // 예: +1, -5 등 (정수로 시간 단위)

    // minutes and seconds relationship
    EXPECT_EQ(mins, secs / 60LL);

    // hours is integer-truncated hours of the offset (e.g. +5:30 => 5)
    EXPECT_EQ(hrs, static_cast<int>(mins / 60LL));

    // short-format string matches numeric offset
    auto sshort = date_time_v1::utc_offset_string(true); // short format (e.g. "+09:00", "-05:00")
    auto colon = sshort.find(':'); // short format에는 ':'이 포함되어야 함
    EXPECT_NE(colon, std::string::npos);

    int parsed_h = std::stoi(sshort.substr(0, colon)); // 예: "+09:00"에서 "+09" 부분을 정수로 파싱
    int parsed_m = std::stoi(sshort.substr(colon + 1)); // 예: "+09:00"에서 "00" 부분을 정수로 파싱
    EXPECT_EQ(parsed_h, static_cast<int>(secs / 3600));
    EXPECT_EQ(parsed_m, static_cast<int>(std::abs(static_cast<int>((secs % 3600) / 60))));
}

TEST(DateTimeV1Test, TotalsAndEquality) {

    using date_time_v1 = j2::datetime::v1::date_time;
    using time_spec_v1 = j2::datetime::v1::time_spec;
    using time_span_v1 = j2::datetime::v1::time_span;

    auto d = time_span_v1::from_days(2); // 2 days
    EXPECT_EQ(d.total_msecs(), 2 * 24LL * 3600LL * 1000LL);
    EXPECT_EQ(d.total_secs(), 2 * 24LL * 3600LL);

    auto h = time_span_v1::from_hours(3); // 3 hours
    EXPECT_EQ(h.total_secs(), 3 * 3600LL);

    auto m = time_span_v1::from_minutes(4); // 4 minutes
    EXPECT_EQ(m.total_secs(), 4 * 60LL);

    auto s = time_span_v1::from_seconds(5); // 5 seconds
    EXPECT_EQ(s.total_secs(), 5LL);

    auto ms = time_span_v1::from_msecs(1500); // 1500 milliseconds
    EXPECT_EQ(ms.total_msecs(), 1500LL);
    EXPECT_EQ(ms.total_secs(), 1LL); // truncated toward zero
    EXPECT_FALSE(d == h);

    auto d_copy = time_span_v1::from_msecs(d.total_msecs()); // d와 동일한 기간을 갖는 time_span 생성
    EXPECT_TRUE(d == d_copy);
}

TEST(DateTimeV1Test, AddSubtractAndSecsTo) {

    auto dt = make_known_utc_datetime(); // 2020-01-02 03:04:05.123 UTC

    auto dt_plus = dt.add_hours(5).add_minutes(30).add_secs(10).add_msecs(250); // 2020-01-02 08:34:15.373 UTC
    auto span = dt.secs_to(dt_plus);
    EXPECT_EQ(span.total_msecs(), ((5LL * 3600) + (30LL * 60) + 10LL) * 1000LL + 250LL);

    // operator+(time_span) and operator-(time_span)
    auto sum = dt + span;
    EXPECT_EQ(sum.to_time_point(), dt_plus.to_time_point());

    auto back = sum - span;
    EXPECT_EQ(back.to_time_point(), dt.to_time_point());

    // operator-(date_time) -> time_span
    auto s2 = dt_plus - dt;
    EXPECT_EQ(s2.total_msecs(), span.total_msecs());

    // negative span
    auto neg = dt - dt_plus;
    EXPECT_EQ(neg.total_msecs(), -span.total_msecs());
}

TEST(DateTimeV1Test, MonthsAndYears) {

    using date_time_v1 = j2::datetime::v1::date_time;
    using time_spec_v1 = j2::datetime::v1::time_spec;
    using time_span_v1 = j2::datetime::v1::time_span;

    // Start: 2020-01-31 00:00:00 UTC

    std::tm tm = {};
    tm.tm_year = 2020 - 1900;
    tm.tm_mon = 0; // tm_mon은 0이 1월 
    tm.tm_mday = 31;
    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;

    auto base = date_time_v1::from_tm(&tm, time_spec_v1::utc); // 2020-01-31 00:00:00 UTC

    auto next = base.add_months(1); // 2020-02-29 (leap year)
    EXPECT_NE(next.to_time_t(), base.to_time_t()); // 2020-02-29로 넘어가야 함

    auto later = base.add_years(1); // 2021-01-31 (2021년은 윤년이 아니므로 2월 29일이 없어서 1월 31일로 조정)
    EXPECT_EQ(later.year(), 2021);
}

TEST(DateTimeV1Test, EqualityAndInequality) {

    using date_time_v1 = j2::datetime::v1::date_time;
    using time_spec_v1 = j2::datetime::v1::time_spec;
    using time_span_v1 = j2::datetime::v1::time_span;

    auto dt = make_known_utc_datetime(); // 2020-01-02 03:04:05.123 UTC
    auto dt_same = date_time_v1::from_time_point(dt.to_time_point(), dt.spec()); // 같은 time_point과 spec으로부터 date_time 생성
    EXPECT_TRUE(dt == dt_same);

    auto dt_diff = dt.add_secs(1); // 1초 더한 다른 date_time
    EXPECT_FALSE(dt == dt_diff);
}



