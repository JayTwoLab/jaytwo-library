#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <utility>
#include <charconv>

#include "j2_library/export.hpp" // For J2LIB_API macro definition

namespace j2::string
{
    // -------------------------
    // 트림 계열
    // -------------------------

    // 앞쪽 공백 제거 (제자리)
    J2LIB_API void ltrim(std::string& s);

    // 뒤쪽 공백 제거 (제자리)
    J2LIB_API void rtrim(std::string& s);

    // 양쪽 공백 제거 (제자리)
    J2LIB_API void trim(std::string& s);

    // 앞쪽 공백 제거 (복사본)
    J2LIB_API std::string ltrim_copy(std::string s);

    // 뒤쪽 공백 제거 (복사본)
    J2LIB_API std::string rtrim_copy(std::string s);

    // 양쪽 공백 제거 (복사본)
    J2LIB_API std::string trim_copy(std::string s);

    // -------------------------
    // 치환 계열
    // -------------------------

    // 전체 치환(복사본). find_token 이 비어있으면 원문 반환
    J2LIB_API std::string replace(const std::string& text,
        const std::string& find_token,
        const std::string& replace_token);

    // 전체 치환(제자리). find_token 이 비어있으면 아무 작업도 하지 않음
    J2LIB_API void replace_all_inplace(std::string& text,
        const std::string& find_token,
        const std::string& replace_token);

    // 첫 번째만 치환(복사본)
    J2LIB_API std::string replace_first(const std::string& text,
        const std::string& find_token,
        const std::string& replace_token);

    // 마지막만 치환(복사본)
    J2LIB_API std::string replace_last(const std::string& text,
        const std::string& find_token,
        const std::string& replace_token);

    // 여러 (find->replace) 쌍을 순차 적용하여 치환
    J2LIB_API std::string replace_all_map(
        const std::string& text,
        const std::vector<std::pair<std::string, std::string>>& replacements);

    // -------------------------
    // 대소문자 변환 / 포함 / 접두/접미
    // -------------------------

    // 문자열을 소문자로 변환 (ASCII 기준)
    J2LIB_API std::string to_lower(const std::string& s);

    // 문자열을 대문자로 변환 (ASCII 기준)
    J2LIB_API std::string to_upper(const std::string& s);

    // 포함 여부(대소문자 구분)
    J2LIB_API bool contains(const std::string& s, const std::string& token);

    // 문자열이 특정 접두사로 시작하는지 확인 (C++17 호환)
    J2LIB_API bool starts_with(const std::string& s, const std::string& prefix);

    // 문자열이 특정 접미사로 끝나는지 확인 (C++17 호환)
    J2LIB_API bool ends_with(const std::string& s, const std::string& suffix);

    // 대소문자 무시 동등 비교/검색 (ASCII 기준)
    J2LIB_API bool iequals(const std::string& a, const std::string& b);
    J2LIB_API bool icontains(const std::string& s, const std::string& token);
    J2LIB_API bool istarts_with(const std::string& s, const std::string& prefix);
    J2LIB_API bool iends_with(const std::string& s, const std::string& suffix);

    // 집합 매칭
    J2LIB_API bool starts_with_any(const std::string& s, const std::vector<std::string>& prefixes);
    J2LIB_API bool ends_with_any(const std::string& s, const std::vector<std::string>& suffixes);

    // 공통 접두/접미 추출
    J2LIB_API std::string common_prefix(const std::vector<std::string>& items);
    J2LIB_API std::string common_suffix(const std::vector<std::string>& items);

    // -------------------------
    // 분할/결합
    // -------------------------

    // 구분자(문자) 기준 분할
    J2LIB_API std::vector<std::string> split(const std::string& s, char delimiter);

    // 구분자(문자열) 기준 분할. keep_empty=true 이면 빈 토큰 유지.
    J2LIB_API std::vector<std::string> split(const std::string& s,
        const std::string& delimiter,
        bool keep_empty = false);

    // 토큰화: 구분자 집합(any-of) 기준 분할. keep_empty=true 이면 빈 토큰 유지.
    J2LIB_API std::vector<std::string> tokenize_any_of(const std::string& s,
        const std::string& delims,
        bool keep_empty = false);

    // 분할 후 각 토큰 trim + 빈 토큰 제거 옵션
    J2LIB_API std::vector<std::string> split_trimmed(const std::string& s,
        char delimiter,
        bool drop_empty = true);

    // 라인 분리(\r\n, \r, \n 모두 처리)
    J2LIB_API std::vector<std::string> split_lines(const std::string& s, bool keep_empty = false);

    // 문자열 합치기 (구분자 삽입)
    J2LIB_API std::string join(const std::vector<std::string>& tokens, const std::string& delimiter);

    // -------------------------
    // 공백/개행 정규화
    // -------------------------

    // 문자열이 비어있거나 공백만 있는지 확인
    J2LIB_API bool is_blank(const std::string& s);

    // 연속 공백을 하나의 공백으로 축약하고 앞뒤 공백 제거
    J2LIB_API std::string collapse_spaces(const std::string& s);

    // 모든 개행을 nl 로 통일 (\r\n, \r, \n → nl). 기본 nl="\n"
    J2LIB_API std::string normalize_newlines(const std::string& s, const std::string& nl = "\n");

    // -------------------------
    // 패딩/반복/삭제/감싸기
    // -------------------------

    // 왼쪽/오른쪽/가운데 패딩. 폭이 기존보다 작으면 원문 그대로 반환.
    J2LIB_API std::string pad_left(const std::string& s, std::size_t width, char fill = ' ');
    J2LIB_API std::string pad_right(const std::string& s, std::size_t width, char fill = ' ');
    J2LIB_API std::string pad_center(const std::string& s, std::size_t width, char fill = ' ');

    // 특정 문자 집합 제거
    J2LIB_API std::string remove_chars(const std::string& s, const std::string& chars);

    // 문자열 반복
    J2LIB_API std::string repeat(const std::string& s, std::size_t count);

    // 따옴표 제거/추가/감싸기
    J2LIB_API std::string strip_quotes(const std::string& s, char quote = '"');
    J2LIB_API std::string quote(const std::string& s, char quote = '"', bool escape = true);
    J2LIB_API std::string surround_if_missing(const std::string& s,
        const std::string& prefix,
        const std::string& suffix);

    // 들여쓰기: 각 라인 앞에 prefix 삽입
    J2LIB_API std::string indent_lines(const std::string& s, const std::string& prefix);

    // -------------------------
    // 접두/접미 제거
    // -------------------------

    // 접두/접미 제거 (제자리)
    J2LIB_API bool remove_prefix(std::string& s, const std::string& prefix);
    J2LIB_API bool remove_suffix(std::string& s, const std::string& suffix);

    // 접두/접미 제거 (복사본)
    J2LIB_API std::string removed_prefix(const std::string& s, const std::string& prefix);
    J2LIB_API std::string removed_suffix(const std::string& s, const std::string& suffix);

    // -------------------------
    // 안전 부분문자열/말줄임
    // -------------------------

    // 범위를 벗어나도 안전하게 잘라내기 (클램프 적용)
    J2LIB_API std::string safe_substr(const std::string& s, std::size_t pos, std::size_t count);

    // 최대 길이를 초과하면 말줄임 부호를 붙여 자르기 (바이트 기준)
    J2LIB_API std::string ellipsize(const std::string& s, std::size_t max_len, const std::string& ellipsis = "...");

    // UTF-8 코드포인트 경계 안전 말줄임(코드포인트 기준; 결합문자/이모지 클러스터는 고려하지 않음)
    J2LIB_API std::string ellipsize_utf8_safe(const std::string& s, std::size_t max_codepoints,
        const std::string& ellipsis = "...");

    // -------------------------
    // 숫자 파싱/검증 (ASCII)
    // -------------------------

    // ASCII 숫자(0-9)로만 구성되었는지
    J2LIB_API bool is_digits(const std::string& s);

    // 정수/실수 파싱 (from_chars 기반, 공백/접두공백 허용 안 함)
    J2LIB_API bool try_parse_int64(const std::string& s, long long& out, int base = 10);
    J2LIB_API bool try_parse_double(const std::string& s, double& out);

    // -------------------------
    // 와일드카드 매칭 (*, ?) - ASCII 기준
    // -------------------------
    J2LIB_API bool wildcard_match(const std::string& text, const std::string& pattern);

} // namespace j2::core
