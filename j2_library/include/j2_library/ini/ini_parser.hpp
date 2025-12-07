#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <utility>
#include <cstdint>

#include "j2_library/export.hpp"

// INI 파서/저장기 (C++17)
// - 주석: 라인 시작 ';' 또는 '#', 값 뒤 inline 주석 ';' 또는 '#'
// - 값 트리밍: 앞/뒤 공백, 탭, CR/LF 제거
// - 문자열 모드:
//    * Raw(작은따옴표 '...'): 저장 시 \t \n \r \\ ' 이스케이프, 로드 시 복원
//    * Literal(큰따옴표 "..."): 특수문자 해석 없이 그대로 저장/로드(큰따옴표만 \" 보호)
// - 불리언: true/false, 정수/실수: 숫자 리터럴

namespace j2::ini {

    enum class value_kind { String, Bool, Int, Double };

    struct J2LIB_API entry {
        std::string key;
        std::string value;
        value_kind  kind{ value_kind::String };
        bool        string_literal{ false }; // true: Literal("..."), false: Raw('...')
    };

    struct J2LIB_API section {
        std::vector<entry> entries;               // 삽입 순서 보존
        std::map<std::string, std::size_t> index; // 키 -> 인덱스
    };

    class J2LIB_API ini_parser {
    public:
        // 파일 로드/저장
        bool load(const std::string& path);
        bool save(const std::string& path) const;

        // 존재 여부
        bool has_section(const std::string& section_name) const;
        bool has(const std::string& section_name, const std::string& key) const;

        // 조회
        std::optional<std::string> get_string(const std::string& section_name,
            const std::string& key) const;
        std::optional<bool>        get_bool(const std::string& section_name,
            const std::string& key) const;
        std::optional<int64_t>     get_int(const std::string& section_name,
            const std::string& key) const;
        std::optional<double>      get_double(const std::string& section_name,
            const std::string& key) const;

        // 설정 (섹션/키 없으면 추가)
        // 문자열(Raw): \t \n \r \\ ' 저장 시 이스케이프, 로드 시 복원
        void set_string(const std::string& section_name,
            const std::string& key,
            const std::string& value);
        // 문자열(Literal): 특수문자 해석 없이 그대로(큰따옴표만 \" 보호)
        void set_string_literal(const std::string& section_name,
            const std::string& key,
            const std::string& value);

        // 불리언/정수/실수 (명시적 세터: 모호성 제거)
        void set_bool(const std::string& section_name,
            const std::string& key,
            bool value);
        void set_int(const std::string& section_name,
            const std::string& key,
            int64_t value);
        void set_double(const std::string& section_name,
            const std::string& key,
            double value);

        // 삭제
        bool remove(const std::string& section_name, const std::string& key);
        bool remove_section(const std::string& section_name);

        // 섹션 나열(삽입 순서)
        std::vector<std::string> section_names() const;

    private:
        // 섹션 컨테이너 (삽입 순서 보존)
        std::vector<std::pair<std::string, section>> sections_;
        std::map<std::string, std::size_t> sec_index_;

        // 유틸
        static std::string trim(std::string s);
        static void strip_inline_comment(std::string& s);

        // 인용/언인용
        static std::string unquote_and_unescape(const std::string& v, char quote_ch);
        static std::string quote_single_and_escape_raw(const std::string& v);      // Raw 저장: '...'
        static std::string quote_double_and_escape_literal(const std::string& v);  // Literal 저장: "..."

        // 문자열 전용 이스케이프 변환 (Raw 모드에만 적용)
        static std::string decode_escapes_for_string(const std::string& s); // \t \n \r \\ ' -> 실제문자
        static std::string encode_escapes_for_string(const std::string& s); // 실제문자 -> \t \n \r \\ '

        // 파서
        static bool parse_bool(const std::string& v, bool& out);
        static bool parse_int(const std::string& v, int64_t& out);
        static bool parse_double(const std::string& v, double& out);
        static std::string format_double(double d);

        section* ensure_section(const std::string& section_name);
        const section* find_section(const std::string& section_name) const;
        section* find_section(const std::string& section_name);

        void upsert_string(const std::string& section_name,
            const std::string& key,
            const std::string& value,
            bool literal_mode);
        void upsert_numeric(const std::string& section_name,
            const std::string& key,
            const std::string& value,
            value_kind kind);
    };

} // namespace j2::ini
