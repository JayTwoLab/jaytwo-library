
#include "j2_library/datetime/datetime_convert.hpp"
#include "j2_library/datetime/datetime_util.hpp"

namespace j2::datetime {

    // ---------------- 변환 ----------------

    // tm → time_t (UTC)
    std::optional<std::time_t> tm_to_time_utc(std::tm tmv) {
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
    std::optional<std::time_t> tm_to_time_local(std::tm tmv) {
        std::time_t t = std::mktime(&tmv);
        if (t == static_cast<std::time_t>(-1)) return std::nullopt;
        return t;
    }



    // time_t → UTC/Local tm (스레드-세이프 우선)
    bool time_t_to_utc_tm(std::time_t t, std::tm& out) {
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

    bool time_t_to_local_tm(std::time_t t, std::tm& out) {
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

    bool get_utc_tm(const std::chrono::system_clock::time_point& tp, std::tm& out) {
        std::time_t t = std::chrono::system_clock::to_time_t(tp);
        return time_t_to_utc_tm(t, out);
    }

    bool get_local_tm(const std::chrono::system_clock::time_point& tp, std::tm& out) {
        std::time_t t = std::chrono::system_clock::to_time_t(tp);
        return time_t_to_local_tm(t, out);
    }

    // (4) std::tm + time_zone_mode → time_point
    std::chrono::system_clock::time_point to_timepoint(const std::tm& tmv, time_zone_mode tzmode) {
        std::optional<std::time_t> t;
        if (tzmode == time_zone_mode::utc)
            t = j2::datetime::tm_to_time_utc(tmv);
        else
            t = j2::datetime::tm_to_time_local(tmv);

        if (!t.has_value()) {
            return std::chrono::system_clock::time_point{}; // 실패 시 epoch 0 반환
        }
        return std::chrono::system_clock::from_time_t(*t);
    }

    // (5) time_point + time_zone_mode → std::tm
    bool to_tm(const std::chrono::system_clock::time_point& tp, time_zone_mode tzmode, std::tm& out) {
        std::time_t t = std::chrono::system_clock::to_time_t(tp);
        if (tzmode == time_zone_mode::utc)
            return j2::datetime::time_t_to_utc_tm(t, out);
        else
            return j2::datetime::time_t_to_local_tm(t, out);
    }


}
