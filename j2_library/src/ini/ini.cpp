#include "j2_library/ini/ini.hpp"

#include <fstream>
#include <sstream>
#include <charconv>
#include <cctype>
#include <cmath>
#include <limits>
#include <iomanip>
#include <cerrno>
#include <algorithm>

namespace j2::ini {

    // 앞/뒤 공백, 탭, CR/LF 제거
    std::string Ini::trim(std::string s) {
        auto isspace_u = [](unsigned char c) { return std::isspace(c) != 0; };
        auto b = std::find_if_not(s.begin(), s.end(), isspace_u);
        if (b == s.end()) return {};
        auto e = std::find_if_not(s.rbegin(), s.rend(), isspace_u).base();
        return std::string(b, e);
    }

    // 따옴표 밖의 ';' 또는 '#' 이후를 잘라냄 (inline 주석)
    void Ini::strip_inline_comment(std::string& s) {
        bool in_single = false, in_double = false;
        for (std::size_t i = 0; i < s.size(); ++i) {
            char c = s[i];
            if (c == '\\') { ++i; continue; }    // 이스케이프 다음은 건너뜀
            if (c == '\'' && !in_double) { in_single = !in_single; continue; }
            if (c == '"' && !in_single) { in_double = !in_double; continue; }
            if (!in_single && !in_double) {
                if (c == ';' || c == '#') { s.erase(i); break; }
            }
        }
    }

    // 따옴표 벗기기 + 해당 따옴표에 대한 이스케이프 복원
    std::string Ini::unquote_and_unescape(const std::string& v, char quote_ch) {
        if (v.size() >= 2 && v.front() == quote_ch && v.back() == quote_ch) {
            std::string inner = v.substr(1, v.size() - 2);
            std::string out; out.reserve(inner.size());
            for (std::size_t i = 0; i < inner.size(); ++i) {
                if (inner[i] == '\\' && i + 1 < inner.size()) {
                    char n = inner[i + 1];
                    if (n == quote_ch) { out.push_back(quote_ch); ++i; continue; }
                }
                out.push_back(inner[i]);
            }
            return out;
        }
        return v;
    }

    // Raw 저장용: 제어문자 이스케이프 후 작은따옴표로 감쌈
    std::string Ini::quote_single_and_escape_raw(const std::string& v) {
        std::string enc = encode_escapes_for_string(v);
        std::string out; out.reserve(enc.size() + 2);
        out.push_back('\'');
        for (char c : enc) {
            if (c == '\'') out.push_back('\\'); // 작은따옴표 자체 이스케이프
            out.push_back(c);
        }
        out.push_back('\'');
        return out;
    }

    // Literal 저장용: 특수문자 해석 없이 그대로, 큰따옴표만 보호
    std::string Ini::quote_double_and_escape_literal(const std::string& v) {
        std::string out; out.reserve(v.size() + 2);
        out.push_back('"');
        for (char c : v) {
            if (c == '"') out.push_back('\\'); // 큰따옴표만 보호
            out.push_back(c);
        }
        out.push_back('"');
        return out;
    }

    // Raw 모드 로드: \t \r \n \\ ' -> 실제 문자
    std::string Ini::decode_escapes_for_string(const std::string& s) {
        std::string out; out.reserve(s.size());
        for (size_t i = 0; i < s.size(); ++i) {
            if (s[i] == '\\' && i + 1 < s.size()) {
                char n = s[i + 1];
                switch (n) {
                case 't': out.push_back('\t'); ++i; continue;
                case 'n': out.push_back('\n'); ++i; continue;
                case 'r': out.push_back('\r'); ++i; continue;
                case '\\': out.push_back('\\'); ++i; continue;
                case '\'': out.push_back('\''); ++i; continue;
                default: break; // 알 수 없는 이스케이프는 그대로 둠
                }
            }
            out.push_back(s[i]);
        }
        return out;
    }

    // Raw 모드 저장: 실제 문자 -> \t \n \r \\ '
    std::string Ini::encode_escapes_for_string(const std::string& s) {
        std::string out; out.reserve(s.size());
        for (char c : s) {
            switch (c) {
            case '\t': out += "\\t"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\\': out += "\\\\"; break;
            case '\'': out += "\\'"; break;
            default:   out.push_back(c); break;
            }
        }
        return out;
    }

    // 불리언 파싱 (대소문자 무관)
    bool Ini::parse_bool(const std::string& v, bool& out) {
        std::string s; s.reserve(v.size());
        for (char c : v) s.push_back(static_cast<char>(std::tolower(c)));
        if (s == "1" || s == "true" || s == "yes" || s == "on") { out = true;  return true; }
        if (s == "0" || s == "false" || s == "no" || s == "off") { out = false; return true; }
        return false;
    }

    // 정수 파싱 (from_chars, 전체 소비 요구)
    bool Ini::parse_int(const std::string& v, int64_t& out) {
        const char* b = v.c_str();
        const char* e = b + v.size();
        auto res = std::from_chars(b, e, out);
        return res.ec == std::errc() && res.ptr == e;
    }

    // 실수 파싱 (strtod, 전체 소비 요구)
    bool Ini::parse_double(const std::string& v, double& out) {
        errno = 0;
        char* pEnd = nullptr;
        out = std::strtod(v.c_str(), &pEnd);
        return errno == 0 && pEnd && *pEnd == '\0';
    }

    // 실수 서식 (정밀도 17, 불필요한 끝 0 제거)
    std::string Ini::format_double(double d) {
        if (!std::isfinite(d)) return std::to_string(d);
        std::ostringstream oss;
        oss.setf(std::ios::fmtflags(0), std::ios::floatfield);
        oss << std::setprecision(17) << d;
        std::string s = oss.str();
        if (s.find('.') != std::string::npos) {
            while (!s.empty() && s.back() == '0') s.pop_back();
            if (!s.empty() && s.back() == '.') s.pop_back();
        }
        if (s.empty()) s = "0";
        return s;
    }

    Section* Ini::ensure_section(const std::string& section) {
        auto it = sec_index_.find(section);
        if (it != sec_index_.end()) return &sections_[it->second].second;
        sections_.push_back({ section, Section{} });
        sec_index_[section] = sections_.size() - 1;
        return &sections_.back().second;
    }
    const Section* Ini::find_section(const std::string& section) const {
        auto it = sec_index_.find(section);
        if (it == sec_index_.end()) return nullptr;
        return &sections_[it->second].second;
    }
    Section* Ini::find_section(const std::string& section) {
        auto it = sec_index_.find(section);
        if (it == sec_index_.end()) return nullptr;
        return &sections_[it->second].second;
    }

    void Ini::upsert_string(const std::string& section,
        const std::string& key,
        const std::string& value,
        bool literal_mode) {
        auto* s = ensure_section(section);
        auto it = s->index.find(key);
        if (it == s->index.end()) {
            s->entries.push_back({ key, value, ValueKind::String, literal_mode });
            s->index[key] = s->entries.size() - 1;
        }
        else {
            auto& e = s->entries[it->second];
            e.value = value;
            e.kind = ValueKind::String;
            e.string_literal = literal_mode;
        }
    }

    void Ini::upsert_numeric(const std::string& section,
        const std::string& key,
        const std::string& value,
        ValueKind kind) {
        auto* s = ensure_section(section);
        auto it = s->index.find(key);
        if (it == s->index.end()) {
            s->entries.push_back({ key, value, kind, false });
            s->index[key] = s->entries.size() - 1;
        }
        else {
            auto& e = s->entries[it->second];
            e.value = value;
            e.kind = kind;
            e.string_literal = false;
        }
    }

    bool Ini::load(const std::string& path) {
        sections_.clear();
        sec_index_.clear();

        std::ifstream ifs(path, std::ios::binary);
        if (!ifs) return false;

        Section* cur_sec = ensure_section("");
        std::string cur, line;

        while (std::getline(ifs, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back(); // CRLF 정리
            std::string work = trim(line);
            if (work.empty()) continue;
            if (work[0] == ';' || work[0] == '#') continue;

            // 섹션
            if (work.front() == '[' && work.back() == ']') {
                cur = trim(work.substr(1, work.size() - 2));
                cur_sec = ensure_section(cur);
                continue;
            }

            // key=value
            auto pos = work.find('=');
            if (pos == std::string::npos) continue;

            std::string key = trim(work.substr(0, pos));
            std::string val = trim(work.substr(pos + 1));

            // inline 주석 제거 -> 재트리밍
            strip_inline_comment(val);
            val = trim(val);

            // 따옴표 검사 & 언쿼트
            bool quoted = false; char quote_ch = 0;
            if (val.size() >= 2 &&
                ((val.front() == '\'' && val.back() == '\'') ||
                    (val.front() == '"' && val.back() == '"'))) {
                quoted = true;
                quote_ch = val.front();
                val = unquote_and_unescape(val, quote_ch); // 같은 따옴표만 복원
            }

            // 타입 판별 및 문자열 모드 결정
            ValueKind kind = ValueKind::String;
            bool literal_mode = false;

            if (!quoted) {
                bool b; int64_t i; double d;
                if (parse_bool(val, b)) { kind = ValueKind::Bool; }
                else if (parse_int(val, i)) { kind = ValueKind::Int; }
                else if (parse_double(val, d)) { kind = ValueKind::Double; }
                else {
                    // 따옴표 없는 문자열은 Raw 규칙 적용: \t \n \r \\ ' 해석
                    val = decode_escapes_for_string(val);
                    kind = ValueKind::String;
                    literal_mode = false;
                }
            }
            else {
                // 작은따옴표 → Raw (디코드), 큰따옴표 → Literal (디코드 없음)
                if (quote_ch == '\'') {
                    val = decode_escapes_for_string(val);
                    literal_mode = false;
                }
                else {
                    literal_mode = true;
                }
                kind = ValueKind::String;
            }

            // 삽입/갱신
            auto it = cur_sec->index.find(key);
            if (it == cur_sec->index.end()) {
                cur_sec->entries.push_back({ key, val, kind, literal_mode });
                cur_sec->index[key] = cur_sec->entries.size() - 1;
            }
            else {
                auto& e = cur_sec->entries[it->second];
                e.value = val;
                e.kind = kind;
                e.string_literal = literal_mode;
            }
        }

        return true;
    }

    bool Ini::save(const std::string& path) const {
        std::ofstream ofs(path, std::ios::binary | std::ios::trunc);
        if (!ofs) return false;

        for (std::size_t si = 0; si < sections_.size(); ++si) {
            const auto& sec_name = sections_[si].first;
            const auto& sec = sections_[si].second;

            if (!sec_name.empty())
                ofs << "[" << sec_name << "]\n";

            for (const auto& e : sec.entries) {
                switch (e.kind) {
                case ValueKind::String: {
                    if (e.string_literal) {
                        // Literal: "..." (큰따옴표만 보호)
                        ofs << e.key << "="
                            << quote_double_and_escape_literal(e.value) << "\n";
                    }
                    else {
                        // Raw: '...' (제어문자 이스케이프)
                        ofs << e.key << "="
                            << quote_single_and_escape_raw(e.value) << "\n";
                    }
                    break;
                }
                case ValueKind::Bool: {
                    bool b{};
                    if (parse_bool(e.value, b))
                        ofs << e.key << "=" << (b ? "true" : "false") << "\n";
                    else
                        ofs << e.key << "=" << "false\n";
                    break;
                }
                case ValueKind::Int: {
                    int64_t i{};
                    if (parse_int(e.value, i))
                        ofs << e.key << "=" << i << "\n";
                    else
                        ofs << e.key << "=" << "0\n";
                    break;
                }
                case ValueKind::Double: {
                    double d{};
                    if (parse_double(e.value, d))
                        ofs << e.key << "=" << format_double(d) << "\n";
                    else
                        ofs << e.key << "=" << "0\n";
                    break;
                }
                }
            }

            if (si + 1 < sections_.size())
                ofs << "\n";
        }

        return true;
    }

    bool Ini::has_section(const std::string& section) const {
        return sec_index_.count(section) != 0;
    }
    bool Ini::has(const std::string& section, const std::string& key) const {
        const auto* s = find_section(section);
        if (!s) return false;
        return s->index.count(key) != 0;
    }

    std::optional<std::string>
        Ini::get_string(const std::string& section, const std::string& key) const {
        const auto* s = find_section(section);
        if (!s) return std::nullopt;
        auto it = s->index.find(key);
        if (it == s->index.end()) return std::nullopt;
        return s->entries[it->second].value;
    }

    std::optional<bool>
        Ini::get_bool(const std::string& section, const std::string& key) const {
        const auto* s = find_section(section);
        if (!s) return std::nullopt;
        auto it = s->index.find(key);
        if (it == s->index.end()) return std::nullopt;
        bool b{};
        if (parse_bool(s->entries[it->second].value, b)) return b;
        return std::nullopt;
    }

    std::optional<int64_t>
        Ini::get_int(const std::string& section, const std::string& key) const {
        const auto* s = find_section(section);
        if (!s) return std::nullopt;
        auto it = s->index.find(key);
        if (it == s->index.end()) return std::nullopt;
        int64_t i{};
        if (parse_int(s->entries[it->second].value, i)) return i;
        return std::nullopt;
    }

    std::optional<double>
        Ini::get_double(const std::string& section, const std::string& key) const {
        const auto* s = find_section(section);
        if (!s) return std::nullopt;
        auto it = s->index.find(key);
        if (it == s->index.end()) return std::nullopt;
        double d{};
        if (parse_double(s->entries[it->second].value, d)) return d;
        return std::nullopt;
    }

    // ===== 명시적 세터 구현 =====
    void Ini::set_string(const std::string& section,
        const std::string& key,
        const std::string& value) {
        upsert_string(section, key, value, /*literal_mode=*/false);
    }
    void Ini::set_string_literal(const std::string& section,
        const std::string& key,
        const std::string& value) {
        upsert_string(section, key, value, /*literal_mode=*/true);
    }
    void Ini::set_bool(const std::string& section,
        const std::string& key,
        bool value) {
        upsert_numeric(section, key, value ? "true" : "false", ValueKind::Bool);
    }
    void Ini::set_int(const std::string& section,
        const std::string& key,
        int64_t value) {
        upsert_numeric(section, key, std::to_string(value), ValueKind::Int);
    }
    void Ini::set_double(const std::string& section,
        const std::string& key,
        double value) {
        upsert_numeric(section, key, format_double(value), ValueKind::Double);
    }

    // ===== 삭제 =====
    bool Ini::remove(const std::string& section, const std::string& key) {
        auto* s = find_section(section);
        if (!s) return false;
        auto it = s->index.find(key);
        if (it == s->index.end()) return false;
        std::size_t idx = it->second;
        s->entries.erase(s->entries.begin() + static_cast<std::ptrdiff_t>(idx));
        s->index.erase(it);
        for (std::size_t i = idx; i < s->entries.size(); ++i) {
            s->index[s->entries[i].key] = i;
        }
        return true;
    }

    bool Ini::remove_section(const std::string& section) {
        auto it = sec_index_.find(section);
        if (it == sec_index_.end()) return false;
        std::size_t idx = it->second;
        sections_.erase(sections_.begin() + static_cast<std::ptrdiff_t>(idx));
        sec_index_.erase(it);
        for (std::size_t i = idx; i < sections_.size(); ++i) {
            sec_index_[sections_[i].first] = i;
        }
        return true;
    }

    std::vector<std::string> Ini::section_names() const {
        std::vector<std::string> v;
        v.reserve(sections_.size());
        for (const auto& p : sections_) v.push_back(p.first);
        return v;
    }

} // namespace j2::ini
