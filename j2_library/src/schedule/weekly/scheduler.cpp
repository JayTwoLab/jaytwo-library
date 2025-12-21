#include "j2_library/schedule/weekly/scheduler.hpp"

#include <algorithm>
#include <vector>
#include <cctype>

namespace j2::schedule::weekly {

    // 문자열 기반 구현
    std::tm make_tm_with_wday_str_hour_min(
        const std::string& weekday_str,
        int hour,
        int minute
    ) {
        std::string s = weekday_str;
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });

        int tm_wday = 0; // Sunday=0
        if (s.find("mon") == 0) tm_wday = 1;
        else if (s.find("tue") == 0) tm_wday = 2;
        else if (s.find("wed") == 0) tm_wday = 3;
        else if (s.find("thu") == 0) tm_wday = 4;
        else if (s.find("fri") == 0) tm_wday = 5;
        else if (s.find("sat") == 0) tm_wday = 6;
        else if (s.find("sun") == 0) tm_wday = 0;
        else {
            // 안전 fallback: 첫 3글자 검사
            std::string p = s.substr(0, std::min<std::size_t>(3, s.size()));
            if (p == "mon") tm_wday = 1;
            else if (p == "tue") tm_wday = 2;
            else if (p == "wed") tm_wday = 3;
            else if (p == "thu") tm_wday = 4;
            else if (p == "fri") tm_wday = 5;
            else if (p == "sat") tm_wday = 6;
            else if (p == "sun") tm_wday = 0;
        }

        std::tm t{};
        t.tm_wday = tm_wday;
        t.tm_hour = hour;
        t.tm_min = minute;
        return t;
    }

    // weekday enum 오버로드: enum -> std::tm 으로 직접 변환
    std::tm make_tm_with_wday_hour_min(
        weekday wd,
        int hour,
        int minute
    ) {
        // weekday enum: mon=0..sun=6
        // std::tm::tm_wday: Sunday=0..Saturday=6
        int wd_int = static_cast<int>(wd);
        int tm_wday = (wd_int + 1) % 7; // mon(0) -> 1, ... sun(6) -> 0
        std::tm t{};
        t.tm_wday = tm_wday;
        t.tm_hour = hour;
        t.tm_min = minute;
        return t;
    }

    scheduler::scheduler(time_base base)
        : time_base_(base),
          ranges_(),
          now_provider_() {
    }

    void scheduler::set_now_provider(std::function<std::tm()> now_provider) {
        now_provider_ = std::move(now_provider);
    }
    
    void scheduler::set_now(weekday wd, int hour, int minute) {
        now_provider_ = [wd, hour, minute]() -> std::tm {
            return make_tm_with_wday_hour_min(wd, hour, minute);
        };
    }

    // JSON에서 읽어 스케줄에 추가합니다 (to_json으로 만든 형식을 읽음)
    void scheduler::load_from_json(const nlohmann::json& j) {
        auto parsed = from_json(j); // schedule_json::from_json
        for (const auto& r : parsed) {
            add_range(r);
        }
    }

    // Add range and merge with existing ranges if they overlap or are adjacent (weekly wrap-aware)
    void scheduler::add_range(const weekly_range& r) {
        constexpr long long MIN_PER_DAY = 24LL * 60LL;
        constexpr long long WEEK_MINUTES = 7LL * MIN_PER_DAY;

        auto to_pair = [&](const weekly_range& x) -> std::pair<long long, long long> {
            long long s = static_cast<long long>(static_cast<int>(x.start_day)) * MIN_PER_DAY
                          + static_cast<long long>(x.start_time.hour) * 60LL
                          + static_cast<long long>(x.start_time.minute);
            long long e = static_cast<long long>(static_cast<int>(x.end_day)) * MIN_PER_DAY
                          + static_cast<long long>(x.end_time.hour) * 60LL
                          + static_cast<long long>(x.end_time.minute);
            if (e < s) e += WEEK_MINUTES;
            return {s, e};
        };

        auto new_pair = to_pair(r);
        long long new_s = new_pair.first;
        long long new_e = new_pair.second;

        // mark which existing ranges will be merged/removed
        std::vector<char> removed(ranges_.size(), 0);

        // First pass: find overlaps/adjacency considering shifts of -WEEK, 0, +WEEK
        for (size_t i = 0; i < ranges_.size(); ++i) {
            auto p = to_pair(ranges_[i]);
            long long es0 = p.first;
            long long ee0 = p.second;

            for (int shift = -1; shift <= 1; ++shift) {
                long long shiftv = static_cast<long long>(shift) * WEEK_MINUTES;
                long long es = es0 + shiftv;
                long long ee = ee0 + shiftv;

                // overlap or adjacent (merge if es <= new_e + 1 && ee >= new_s - 1)
                if (es <= new_e + 1 && ee >= new_s - 1) {
                    removed[i] = 1;
                    new_s = std::min(new_s, es);
                    new_e = std::max(new_e, ee);
                    break;
                }
            }
        }

        // Repeat because merging expanded interval may now overlap other existing ranges
        bool changed = true;
        while (changed) {
            changed = false;
            for (size_t i = 0; i < ranges_.size(); ++i) {
                if (removed[i]) continue;
                auto p = to_pair(ranges_[i]);
                long long es0 = p.first;
                long long ee0 = p.second;

                for (int shift = -1; shift <= 1; ++shift) {
                    long long shiftv = static_cast<long long>(shift) * WEEK_MINUTES;
                    long long es = es0 + shiftv;
                    long long ee = ee0 + shiftv;

                    if (es <= new_e + 1 && ee >= new_s - 1) {
                        removed[i] = 1;
                        new_s = std::min(new_s, es);
                        new_e = std::max(new_e, ee);
                        changed = true;
                        break;
                    }
                }
            }
        }

        // Build new ranges vector excluding removed ones
        weekly_ranges out;
        out.reserve(ranges_.size() + 1);
        for (size_t i = 0; i < ranges_.size(); ++i) {
            if (!removed[i]) out.push_back(ranges_[i]);
        }

        // Convert merged interval back to weekly_range representation
        long long span = new_e - new_s;
        weekly_range merged{};
        if (span + 1 >= WEEK_MINUTES) {
            // full week
            merged.start_day = weekday::mon;
            merged.start_time.hour = 0;
            merged.start_time.minute = 0;
            merged.end_day = weekday::sun;
            merged.end_time.hour = 23;
            merged.end_time.minute = 59;
        } else {
            long long s_mod = ((new_s % WEEK_MINUTES) + WEEK_MINUTES) % WEEK_MINUTES;
            long long e_mod = ((new_e % WEEK_MINUTES) + WEEK_MINUTES) % WEEK_MINUTES;

            merged.start_day = static_cast<weekday>(static_cast<int>(s_mod / MIN_PER_DAY));
            {
                long long m = s_mod % MIN_PER_DAY;
                merged.start_time.hour = static_cast<int>(m / 60);
                merged.start_time.minute = static_cast<int>(m % 60);
            }

            merged.end_day = static_cast<weekday>(static_cast<int>(e_mod / MIN_PER_DAY));
            {
                long long m = e_mod % MIN_PER_DAY;
                merged.end_time.hour = static_cast<int>(m / 60);
                merged.end_time.minute = static_cast<int>(m % 60);
            }
        }

        out.push_back(merged);
        ranges_.swap(out);
    }

    const weekly_ranges& scheduler::ranges() const {
        return ranges_;
    }

    bool scheduler::is_active_now() const {
        if (now_provider_) {
            return is_active_now(now_provider_);
        }
        std::tm tm = time_base_.now_tm();
        return is_active_at_tm(tm);
    }

    bool scheduler::is_active_now(std::function<std::tm()> now_provider) const {
        if (now_provider) {
            std::tm tm = now_provider();
            return is_active_at_tm(tm);
        }
        return is_active_now();
    }

    bool scheduler::is_active_at_tm(const std::tm& tm) const {
        int now_day = (tm.tm_wday + 6) % 7; // mon=0
        int now_min = tm.tm_hour * 60 + tm.tm_min;

        for (const auto& r : ranges_) {
            int sd = static_cast<int>(r.start_day);
            int ed = static_cast<int>(r.end_day);

            int sm = r.start_time.hour * 60 + r.start_time.minute;
            int em = r.end_time.hour * 60 + r.end_time.minute;

            bool day_match =
                (sd <= ed && now_day >= sd && now_day <= ed) ||
                (sd > ed && (now_day >= sd || now_day <= ed));

            if (!day_match) continue;

            if (sd == ed) {
                if (now_min >= sm && now_min <= em) return true;
            }
            else if (now_day == sd) {
                if (now_min >= sm) return true;
            }
            else if (now_day == ed) {
                if (now_min <= em) return true;
            }
            else {
                return true;
            }
        }
        return false;
    }

} // namespace j2::schedule::weekly
