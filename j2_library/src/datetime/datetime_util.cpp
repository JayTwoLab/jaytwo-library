
#include "j2_library/datetime/datetime_convert.hpp"
#include "j2_library/datetime/datetime_util.hpp"

namespace j2::datetime {

    // ---------------- 내부 유틸 ----------------

    // 숫자 n자리 읽기
    bool read_ndigits(const std::string& s, size_t& pos, int ndig, int& out) {
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
    bool match_token(const std::string& fmt, size_t pos, const char* tok) {
        for (int i = 0; tok[i]; ++i) {
            if (pos + static_cast<size_t>(i) >= fmt.size()) return false;
            if (fmt[pos + static_cast<size_t>(i)] != tok[i]) return false;
        }
        return true;
    }

    // 윤년
    bool is_leap(int y) {
        return ((y % 4 == 0) && (y % 100 != 0)) || (y % 400 == 0);
    }

    // 일자 유효성
    bool valid_ymd(int Y, int M, int D) {
        if (Y < 0 || M < 1 || M > 12) return false;
        static const int mdays[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
        int dim = mdays[M - 1];
        if (M == 2 && is_leap(Y)) dim = 29;
        return D >= 1 && D <= dim;
    }


    // tzmode 기준 "현재" 시각/날짜
    std::tm now_in_zone(TimeZoneMode tz) {
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
    std::tm today_in_fixed_offset_seconds(int offset_sec) {
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


    // 내부 유틸: 0 채움 숫자 쓰기
    J2LIB_API
        void j2::datetime::append_ndigits(std::string& out, int value, int width) {
        int v = value;
        if (v < 0) v = 0;
        char buf[32];
        int pos = 31; buf[pos] = '\0';
        for (int i = 0; i < width; ++i) {
            int d = v % 10; v /= 10;
            buf[--pos] = static_cast<char>('0' + d);
        }
        out.append(&buf[pos]);
    }

    // 내부 유틸: 토큰 일치 검사
    J2LIB_API
        bool j2::datetime::fmt_match(const std::string& fmt, size_t fp, const char* tok) {
        for (int i = 0; tok[i]; ++i) {
            if (fp + static_cast<size_t>(i) >= fmt.size()) return false;
            if (fmt[fp + static_cast<size_t>(i)] != tok[i]) return false;
        }
        return true;
    }

    // 핵심 포맷터: tm → 문자열
    J2LIB_API
        std::string j2::datetime::format_from_tm_core(const std::tm& tmv,
            const std::string& format) {
        std::string out;
        out.reserve(format.size() + 16);

        for (size_t fp = 0; fp < format.size();) {
            if (fmt_match(format, fp, "YYYY")) {
                append_ndigits(out, tmv.tm_year + 1900, 4);
                fp += 4;
            }
            else if (fmt_match(format, fp, "MM")) {
                append_ndigits(out, tmv.tm_mon + 1, 2);
                fp += 2;
            }
            else if (fmt_match(format, fp, "DD")) {
                append_ndigits(out, tmv.tm_mday, 2);
                fp += 2;
            }
            else if (fmt_match(format, fp, "hh")) {
                append_ndigits(out, tmv.tm_hour, 2);
                fp += 2;
            }
            else if (fmt_match(format, fp, "mm")) {
                append_ndigits(out, tmv.tm_min, 2);
                fp += 2;
            }
            else if (fmt_match(format, fp, "ss")) {
                append_ndigits(out, tmv.tm_sec, 2);
                fp += 2;
            }
            else {
                out.push_back(format[fp]);
                ++fp;
            }
        }
        return out;
    }


    // (1) std::tm + format
    J2LIB_API std::string j2::datetime::format_datetime(
        const std::tm& tmv,
        const std::string& format) {
        return format_from_tm_core(tmv, format);
    }

    // (2) time_t + tzmode + format
    J2LIB_API std::string j2::datetime::format_datetime(
        std::time_t t,
        j2::datetime::TimeZoneMode tzmode,
        const std::string& format) {
        std::tm tmv{};
        bool ok = (tzmode == TimeZoneMode::UTC)
            ? j2::datetime::time_t_to_utc_tm(t, tmv)
            : j2::datetime::time_t_to_local_tm(t, tmv);
        if (!ok) return std::string{};
        return format_from_tm_core(tmv, format);
    }

    // (3) time_point + tzmode + format
    J2LIB_API std::string j2::datetime::format_datetime(
            const std::chrono::system_clock::time_point& tp,
            j2::datetime::TimeZoneMode tzmode,
            const std::string& format) {
        std::time_t t = std::chrono::system_clock::to_time_t(tp);
        std::tm tmv{};
        bool ok = (tzmode == TimeZoneMode::UTC)
            ? j2::datetime::time_t_to_utc_tm(t, tmv)
            : j2::datetime::time_t_to_local_tm(t, tmv);
        if (!ok) return std::string{};
        return format_from_tm_core(tmv, format);
    }


} // namespace j2::datetime
