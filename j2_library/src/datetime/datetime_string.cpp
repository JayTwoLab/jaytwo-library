#include "j2_library/datetime/datetime.hpp"

#include <cctype>
#include <limits>

namespace j2::datetime {

    // ---------------- 내부 유틸 ----------------

    // 숫자 n자리 읽기
    static bool read_ndigits(const std::string& s, size_t& pos, int ndig, int& out) {
        if (pos + static_cast<size_t>(ndig) > s.size()) return false;
        int v = 0;
        for (int i = 0; i < ndig; ++i) {
            char c = s[pos + i];
            if (!std::isdigit(static_cast<unsigned char>(c))) return false;
            v = v * 10 + (c - '0');
        }
        out = v;
        pos += static_cast<size_t>(ndig);
        return true;
    }

    // 형식 토큰 일치 검사
    static bool match_token(const std::string& fmt, size_t pos, const char* tok) {
        for (int i = 0; tok[i]; ++i) {
            if (pos + static_cast<size_t>(i) >= fmt.size()) return false;
            if (fmt[pos + static_cast<size_t>(i)] != tok[i]) return false;
        }
        return true;
    }

    // 윤년
    static bool is_leap(int y) {
        return ((y % 4 == 0) && (y % 100 != 0)) || (y % 400 == 0);
    }

    // 일자 유효성
    static bool valid_ymd(int Y, int M, int D) {
        if (Y < 0 || M < 1 || M > 12) return false;
        static const int mdays[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
        int dim = mdays[M - 1];
        if (M == 2 && is_leap(Y)) dim = 29;
        return D >= 1 && D <= dim;
    }

    // tm → time_t (UTC)
    static std::optional<std::time_t> tm_to_time_utc(std::tm tmv) {
#if defined(_WIN32)
        std::time_t t = _mkgmtime(&tmv);
        if (t == static_cast<std::time_t>(-1)) return std::nullopt;
        return t;
#else
#if defined(__USE_MISC) || defined(_GNU_SOURCE) || defined(__APPLE__)
        std::time_t t = timegm(&tmv);
        if (t == static_cast<std::time_t>(-1)) return std::nullopt;
        return t;
#else
        return std::nullopt; // timegm 미제공 환경에선 실패
#endif
#endif
    }

    // tm → time_t (Localtime)
    static std::optional<std::time_t> tm_to_time_local(std::tm tmv) {
        std::time_t t = std::mktime(&tmv);
        if (t == static_cast<std::time_t>(-1)) return std::nullopt;
        return t;
    }

    // tzmode 기준 "현재" 시각/날짜
    static std::tm now_in_zone(TimeZoneMode tz) {
        std::tm out{};
        auto now = std::time(nullptr);
#if defined(_WIN32)
        if (tz == TimeZoneMode::UTC) { gmtime_s(&out, &now); }
        else { localtime_s(&out, &now); }
#else
        if (tz == TimeZoneMode::UTC) { out = *std::gmtime(&now); }
        else { out = *std::localtime(&now); }
#endif
        return out;
    }

    // 고정 오프셋(초) 기준의 "오늘 날짜"
    static std::tm today_in_fixed_offset_seconds(int offset_sec) {
        std::tm out{};
        auto now = std::time(nullptr);
        std::time_t shifted = now + offset_sec;
#if defined(_WIN32)
        gmtime_s(&out, &shifted);
#else
        out = *std::gmtime(&shifted);
#endif
        return out;
    }

    // time_t → UTC/Local tm (스레드-세이프 우선)
    static bool time_t_to_utc_tm(std::time_t t, std::tm& out) {
#if defined(_WIN32)
        return gmtime_s(&out, &t) == 0;
#else
#if defined(_POSIX_VERSION)
        return gmtime_r(&t, &out) != nullptr;
#else
        std::tm* p = std::gmtime(&t);
        if (!p) return false;
        out = *p; return true;
#endif
#endif
    }
    static bool time_t_to_local_tm(std::time_t t, std::tm& out) {
#if defined(_WIN32)
        return localtime_s(&out, &t) == 0;
#else
#if defined(_POSIX_VERSION)
        return localtime_r(&t, &out) != nullptr;
#else
        std::tm* p = std::localtime(&t);
        if (!p) return false;
        out = *p; return true;
#endif
#endif
    }

    // ---------------- (A) 형식 파서 ----------------

    // 변경: base_tm을 선택적으로 사용하기 위해 포인터 인자로 변경
    static DateTimeParseResult
        parse_strict_datetime_ex_impl(const std::string& datetime,
            const std::string& format,
            TimeZoneMode tzmode,
            const std::tm* base_tm_opt)
    {
        DateTimeParseResult r;
        r.present = {};

        // 초기값 설정:
        // - base_tm_opt가 있으면 그 값을 초기값으로 사용(누락 토큰 보정).
        // - 없으면 임시 tm을 0으로 초기화하되, 보정은 하지 않음(누락 금지).
        std::tm tmv{}; // 초기값 1900년 1월 0일 00:00:00 (0일 → 유효하지 않은 값 (일은 최소 1이어야 정상))
        if (base_tm_opt) {
            tmv = *base_tm_opt;
        }

        int Y = base_tm_opt ? (tmv.tm_year + 1900) : 0;
        int M = base_tm_opt ? (tmv.tm_mon + 1) : 0;
        int D = base_tm_opt ? (tmv.tm_mday) : 0; 
        int h = base_tm_opt ? (tmv.tm_hour) : 0;
        int m = base_tm_opt ? (tmv.tm_min) : 0;
        int s = base_tm_opt ? (tmv.tm_sec) : 0;
        int ms = 0;

        size_t ip = 0;
        for (size_t fp = 0; fp < format.size();) {
            if (match_token(format, fp, "YYYY")) { if (!read_ndigits(datetime, ip, 4, Y)) { r.error = u8"YYYY Error"; return r; } r.present.Y = true; fp += 4; }
            else if (match_token(format, fp, "MM")) { if (!read_ndigits(datetime, ip, 2, M)) { r.error = u8"MM Error"; return r; } r.present.M = true; fp += 2; }
            else if (match_token(format, fp, "DD")) { if (!read_ndigits(datetime, ip, 2, D)) { r.error = u8"DD Error"; return r; } r.present.D = true; fp += 2; }
            else if (match_token(format, fp, "hh")) { if (!read_ndigits(datetime, ip, 2, h)) { r.error = u8"hh Error"; return r; } r.present.h = true; fp += 2; }
            else if (match_token(format, fp, "mm")) { if (!read_ndigits(datetime, ip, 2, m)) { r.error = u8"mm Error"; return r; } r.present.m = true; fp += 2; }
            else if (match_token(format, fp, "ss")) { if (!read_ndigits(datetime, ip, 2, s)) { r.error = u8"ss Error"; return r; } r.present.s = true; fp += 2; }
            else if (match_token(format, fp, "SSS")) { if (!read_ndigits(datetime, ip, 3, ms)) { r.error = u8"SSS Error"; return r; } r.present.SSS = true; fp += 3; }
            else {
                if (ip >= datetime.size()) { r.error = u8"Literally inconsistent"; return r; }
                if (format[fp] != datetime[ip]) { r.error = u8"Literally inconsistent"; return r; }
                ++fp; ++ip;
            }
        }
        if (ip != datetime.size()) { r.error = u8"Extra character"; return r; }

        // base가 없는 경우: 핵심 토큰 누락 금지
        if (!base_tm_opt) {
            const bool date_ok = r.present.Y && r.present.M && r.present.D;
            const bool time_ok = r.present.h && r.present.m && r.present.s;
            if (!date_ok || !time_ok) {
                r.error = u8"Missing tokens without base (require YYYY,MM,DD,hh,mm,ss)";
                return r;
            }
        }

        // === 범위 검증 ===
        if (r.present.h && (h < 0 || h > 23)) {
            r.error = u8"Hour range error"; return r;
        }
        if (r.present.m && (m < 0 || m > 59)) {
            r.error = u8"Minute range error"; return r;
        }
        if (r.present.s && (s < 0 || s > 59)) {
            r.error = u8"Second range error"; return r;
        }

        if (r.present.Y || r.present.M || r.present.D) {
            if (r.present.M && (M < 1 || M > 12)) {
                r.error = u8"Month range error"; return r;
            }
            if (!valid_ymd(Y, M, D)) {
                r.error = u8"Date Validation Error"; return r;
            }
        }

        // 변환용 tm 구성
        tmv.tm_year = Y - 1900;
        tmv.tm_mon = M - 1;
        tmv.tm_mday = D;
        tmv.tm_hour = h;
        tmv.tm_min = m;
        tmv.tm_sec = s;
        tmv.tm_isdst = -1;

        auto t = (tzmode == TimeZoneMode::UTC) ? tm_to_time_utc(tmv) : tm_to_time_local(tmv);
        if (!t.has_value()) { r.error = u8"Time conversion failure"; return r; }

        // 성공 결과 설정
        r.ok = true;

        if (base_tm_opt) {
            r.broken = tmv;   // base가 있을 때만 tm 반환
        }
        else {
            r.broken = std::nullopt; // base가 없으면 없음 처리
        }

        r.millisecond = (r.present.SSS ? ms : 0); // 밀리초 (0~999)

        r.epoch = *t;               // UNIX epoch (초)

        r.epoch_ms = static_cast<std::int64_t>(r.epoch) * 1000 + r.millisecond; // UNIX epoch (밀리초)

        r.timepoint = std::chrono::system_clock::time_point{
            std::chrono::milliseconds{r.epoch_ms} }; // C++ time_point

        return r;
    }

    J2LIB_API DateTimeParseResult
        parse_strict_datetime(const std::string& datetime,
            const std::string& format,
            TimeZoneMode tzmode) {
        return parse_strict_datetime_ex_impl(datetime, format, tzmode, nullptr);
    }

    J2LIB_API DateTimeParseResult
        parse_strict_datetime_with_base(const std::string& datetime,
            const std::string& format,
            TimeZoneMode tzmode,
            const std::tm& base_tm) {
        return parse_strict_datetime_ex_impl(datetime, format, tzmode, &base_tm);
    }

    J2LIB_API DateTimeParseResult
        parse_strict_datetime_optbase(const std::string& datetime,
            const std::string& format,
            TimeZoneMode tzmode,
            const std::tm* base_or_null) {
        // base_or_null == nullptr → 누락 토큰 금지
        return parse_strict_datetime_ex_impl(datetime, format, tzmode, base_or_null);
    }

    J2LIB_API std::optional<std::int64_t>
        parse_strict_datetime_epoch_ms(const std::string& datetime,
            const std::string& format,
            TimeZoneMode tzmode) {
        auto r = parse_strict_datetime(datetime, format, tzmode);
        if (!r.ok) return std::nullopt;
        return r.epoch_ms;
    }

    // ---------------- (B) ISO-8601 파서 ----------------

    static bool read_2d(const std::string& s, size_t& pos, int& v) {
        return read_ndigits(s, pos, 2, v);
    }

    // "Z" | (+|-)HH[:?]MM | (+|-)HH
    static bool parse_tz_offset(const std::string& s, size_t& pos, int& offset_sec) {
        if (pos >= s.size()) return false;
        if (s[pos] == 'Z' || s[pos] == 'z') { offset_sec = 0; ++pos; return true; }
        char sign = s[pos];
        if (sign != '+' && sign != '-') return false;
        ++pos;
        int HH = 0; if (!read_ndigits(s, pos, 2, HH)) return false;
        int MM = 0;
        if (pos < s.size() && s[pos] == ':') {
            ++pos; if (!read_ndigits(s, pos, 2, MM)) return false;
        }
        else {
            if (pos + 2 <= s.size() &&
                std::isdigit((unsigned char)s[pos]) &&
                std::isdigit((unsigned char)s[pos + 1])) {
                if (!read_ndigits(s, pos, 2, MM)) return false;
            }
        }
        int sec = HH * 3600 + MM * 60;
        if (sign == '-') sec = -sec;
        offset_sec = sec;
        return true;
    }

    // .frac → 밀리초(앞 3자리), 부족 시 0 패딩
    static bool parse_fraction_ms(const std::string& s, size_t& pos, int& out_ms) {
        if (pos >= s.size() || s[pos] != '.') return false;
        ++pos;
        int digits = 0;
        int ms = 0;
        while (pos < s.size() && std::isdigit((unsigned char)s[pos]) && digits < 9) {
            int d = s[pos] - '0';
            if (digits < 3) ms = ms * 10 + d;
            ++pos; ++digits;
        }
        if (digits == 0) return false;
        while (digits < 3) { ms *= 10; ++digits; }
        out_ms = ms;
        return true;
    }

    static DateTimeParseResult
        parse_iso8601_strict(const std::string& str, TimeZoneMode fallback_tzmode)
    {
        DateTimeParseResult r;
        size_t p = 0;
        int Y = -1, M = -1, D = -1, hh = -1, mm = -1, ss = -1, ms = 0;
        bool has_date = false, has_time = false, has_tz = false;
        int tz_offset_sec = 0;

        auto parse_date_extended = [&]() -> bool {
            int y, m, d; size_t op = p;
            if (!read_ndigits(str, p, 4, y)) { p = op; return false; }
            if (p >= str.size() || str[p] != '-') { p = op; return false; } ++p;
            if (!read_ndigits(str, p, 2, m)) { p = op; return false; }
            if (p >= str.size() || str[p] != '-') { p = op; return false; } ++p;
            if (!read_ndigits(str, p, 2, d)) { p = op; return false; }
            Y = y; M = m; D = d; return true;
            };
        auto parse_time_extended = [&]()->bool {
            int H, Min; size_t op = p;
            if (!read_2d(str, p, H)) { p = op; return false; }
            if (p >= str.size() || str[p] != ':') { p = op; return false; } ++p;
            if (!read_2d(str, p, Min)) { p = op; return false; }
            int S = -1; if (p < str.size() && str[p] == ':') { ++p; if (!read_2d(str, p, S)) { p = op; return false; } }
            hh = H; mm = Min; ss = S; return true;
            };

        // 날짜(+T/공백)시간, 또는 시간만
        size_t p0 = p;
        if (parse_date_extended()) {
            has_date = true;
            if (p < str.size() && (str[p] == 'T' || str[p] == 't' || str[p] == ' ')) {
                ++p;
                if (!parse_time_extended()) { r.error = u8"Time format error"; return r; }
                has_time = true;
            }
        }
        else {
            p = p0;
            if (parse_time_extended()) has_time = true;
            else { r.error = u8"Not in ISO-8601 format"; return r; }
        }

        if (has_time && p < str.size() && str[p] == '.') {
            if (!parse_fraction_ms(str, p, ms)) { r.error = u8"Fractional ms seconds format error"; return r; }
        }

        if (p < str.size()) {
            size_t tzp = p; int off = 0;
            if (parse_tz_offset(str, tzp, off)) { has_tz = true; tz_offset_sec = off; p = tzp; }
        }

        if (p != str.size()) { r.error = u8"Extra input characters exist"; return r; }

        // 날짜가 없으면 오늘 날짜 보정
        if (!has_date) {
            std::tm base = has_tz ? today_in_fixed_offset_seconds(tz_offset_sec)
                : now_in_zone(fallback_tzmode);
            Y = base.tm_year + 1900;
            M = base.tm_mon + 1;
            D = base.tm_mday;
        }
        if (ss < 0) ss = 0;

        std::tm tmv{}; tmv.tm_year = Y - 1900; tmv.tm_mon = M - 1; tmv.tm_mday = D;
        tmv.tm_hour = hh; tmv.tm_min = mm; tmv.tm_sec = ss; tmv.tm_isdst = -1;

        std::optional<std::time_t> t;
        if (has_tz) {
            auto t_as_utc = tm_to_time_utc(tmv);
            if (!t_as_utc) { r.error = u8"UTC conversion failed (timegem/_mkgmtime)"; return r; }
            t = t_as_utc.value() - tz_offset_sec; // 오프셋 적용
        }
        else {
            t = (fallback_tzmode == TimeZoneMode::UTC) ? tm_to_time_utc(tmv)
                : tm_to_time_local(tmv);
        }
        if (!t) { r.error = u8"Time conversion failure"; return r; }

        r.ok = true;
        r.epoch = *t;
        r.millisecond = ms;
        r.epoch_ms = static_cast<std::int64_t>(r.epoch) * 1000 + r.millisecond;
        r.broken = tmv;
        r.timepoint = std::chrono::system_clock::time_point{
            std::chrono::milliseconds{r.epoch_ms} };
        return r;
    }

    // -------- 공개 API --------

    J2LIB_API DateTimeParseResult
        parse_iso8601_datetime(const std::string& iso8601,
            TimeZoneMode fallback_tzmode) {
        return parse_iso8601_strict(iso8601, fallback_tzmode);
    }

    J2LIB_API DateTimeParseResult
        parse_datetime_auto(const std::string& text,
            const std::string& format_or_literal,
            TimeZoneMode tzmode) {
        if (format_or_literal == "ISO8601") {
            return parse_iso8601_datetime(text, tzmode);
        }
        return parse_strict_datetime(text, format_or_literal, tzmode);
    }

    // ---- 변환/헬퍼 구현 ----

    J2LIB_API bool get_utc_tm(const std::chrono::system_clock::time_point& tp, std::tm& out) {
        std::time_t t = std::chrono::system_clock::to_time_t(tp);
        return time_t_to_utc_tm(t, out);
    }

    J2LIB_API bool get_local_tm(const std::chrono::system_clock::time_point& tp, std::tm& out) {
        std::time_t t = std::chrono::system_clock::to_time_t(tp);
        return time_t_to_local_tm(t, out);
    }

    J2LIB_API bool get_utc_tm(const DateTimeParseResult& r, std::tm& out) {
        return get_utc_tm(r.timepoint, out);
    }

    J2LIB_API bool get_local_tm(const DateTimeParseResult& r, std::tm& out) {
        return get_local_tm(r.timepoint, out);
    }

    J2LIB_API
        std::optional<std::chrono::system_clock::time_point>
        parse_datetime_timepoint(const std::string& text,
            const std::string& format_or_literal,
            TimeZoneMode tzmode)
    {
        auto r = parse_datetime_auto(text, format_or_literal, tzmode);
        if (!r.ok) return std::nullopt;
        return r.timepoint;
    }

} // namespace j2::datetime
