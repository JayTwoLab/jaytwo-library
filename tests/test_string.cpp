#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "j2_library/string/string.hpp"
#include "j2_library/string/to_console_encoding.hpp"

// 문자열 유틸 전반을 검증하는 테스트 스위트
// - 트림(공백 제거) / 치환(문자열 바꾸기)
// - 대소문 변환 / 접두사·접미사 확인
// - 분할(split)/결합(join) / 패딩 / 말줄임 등
TEST(StringUtils, TrimBasic)
{
    // ltrim: 문자열 앞쪽 공백 제거
    std::string s1 = "   abc  ";
    j2::string::ltrim(s1);
    EXPECT_EQ(s1, "abc  ");  // 앞 공백 3개 제거 → "abc" + 뒤 공백 2개 유지

    // rtrim: 문자열 뒤쪽 공백 제거
    j2::string::rtrim(s1);
    EXPECT_EQ(s1, "abc");    // 뒤 공백 제거 후 "abc"만 남음

    // trim_copy: 문자열 앞뒤 공백 모두 제거 (복사본 리턴)
    std::string s2 = " \t\n  xyz \r\n ";
    EXPECT_EQ(j2::string::trim_copy(s2), "xyz");
}

TEST(StringUtils, ReplaceVariants)
{
    // replace: 원본 문자열의 모든 토큰을 새 토큰으로 바꾼 복사본 리턴
    EXPECT_EQ(j2::string::replace("a_b_c_b", "b", "X"), "a_X_c_X");

    // replace_all_inplace: 제자리 치환 (원본 문자열 자체가 수정됨)
    std::string t = "foo foo bar foo";
    j2::string::replace_all_inplace(t, "foo", "qux");
    EXPECT_EQ(t, "qux qux bar qux");

    // replace_first: 첫 번째 매치만 교체
    EXPECT_EQ(j2::string::replace_first("aaabaa", "aa", "Z"), "Zabaa"); 
    // replace_last: 마지막 매치만 교체
    EXPECT_EQ(j2::string::replace_last("aaabaa", "aa", "Z"), "aaabZ");
}

TEST(StringUtils, CaseAndSearch)
{
    // 대소문 변환
    EXPECT_EQ(j2::string::to_lower("AbC123"), "abc123");
    EXPECT_EQ(j2::string::to_upper("AbC123"), "ABC123");

    // 문자열 포함 여부 / 접두사 / 접미사 확인
    EXPECT_TRUE(j2::string::contains("hello world", "world"));
    EXPECT_TRUE(j2::string::starts_with("foobar", "foo"));
    EXPECT_TRUE(j2::string::ends_with("foobar", "bar"));

    // 대소문 무시 비교 (i = ignore case)
    EXPECT_TRUE(j2::string::iequals("AbC", "aBc"));
    EXPECT_TRUE(j2::string::icontains("Hello World", "WORLD"));
    EXPECT_TRUE(j2::string::istarts_with("Prefix", "pre"));
    EXPECT_TRUE(j2::string::iends_with("Suffix", "FIX"));
}

TEST(StringUtils, SplitJoin)
{
    // split: 구분자 기준으로 문자열 분리 (문자 단위)
    auto v1 = j2::string::split("a,b,,c", ',');
    ASSERT_EQ(v1.size(), 4u);
    EXPECT_EQ(v1[0], "a");
    EXPECT_EQ(v1[1], "b");
    EXPECT_EQ(v1[2], "");   // 빈 토큰도 유지
    EXPECT_EQ(v1[3], "c");

    // split: 구분자 기준 (문자열 단위)
    auto v2 = j2::string::split("one<>two<>three", "<>", false);
    ASSERT_EQ(v2.size(), 3u);
    EXPECT_EQ(v2[0], "one");
    EXPECT_EQ(v2[1], "two");
    EXPECT_EQ(v2[2], "three");

    // join: 벡터의 문자열을 구분자 넣어 합치기
    EXPECT_EQ(j2::string::join(v2, "|"), "one|two|three");

    // split_trimmed: 분할 후 각 토큰을 trim, 빈 문자열은 제거
    auto v3 = j2::string::split_trimmed("  a ,  b ,   , c  ", ',', true);
    ASSERT_EQ(v3.size(), 3u);
    EXPECT_EQ(v3[0], "a");
    EXPECT_EQ(v3[1], "b");
    EXPECT_EQ(v3[2], "c");

    // split_lines: 개행 기준 분할 (CR, LF, CRLF 모두 처리)
    auto v4 = j2::string::split_lines("a\r\nb\nc\rd", false);
    std::vector<std::string> expect4 = { "a","b","c","d" };
    EXPECT_EQ(v4, expect4);
}

TEST(StringUtils, PaddingRemoveCharsRepeat)
{
    // pad_*: 문자열 길이가 n이 되도록 특정 문자로 채우기
    EXPECT_EQ(j2::string::pad_left("7", 3, '0'), "007");
    EXPECT_EQ(j2::string::pad_right("7", 3, '0'), "700");
    EXPECT_EQ(j2::string::pad_center("7", 3, '0'), "070");

    // remove_chars: 특정 문자 집합을 제거
    EXPECT_EQ(j2::string::remove_chars("a-b_c.d", "-_."), "abcd");

    // repeat: 문자열 반복
    EXPECT_EQ(j2::string::repeat("ab", 3), "ababab");
}

TEST(StringUtils, CollapseNormalize)
{
    // collapse_spaces: 연속된 공백/개행/탭을 단일 공백으로 축약, 앞뒤 공백 제거
    EXPECT_EQ(j2::string::collapse_spaces("  a \t  b \n  c  "), "a b c");

    // normalize_newlines: 모든 종류의 개행(CR, LF, CRLF)을 지정 문자열로 통일
    EXPECT_EQ(j2::string::normalize_newlines("a\r\nb\rc\n", "\n"), "a\nb\nc\n");

    // is_blank: 문자열이 비었거나 공백만 있으면 true
    EXPECT_TRUE(j2::string::is_blank(" \t \n "));
    EXPECT_FALSE(j2::string::is_blank("  x "));
}

TEST(StringUtils, PrefixSuffixRemoveAndSafeSubstr)
{
    std::string s = "prefix-body-suffix";

    // remove_prefix / remove_suffix: 접두사/접미사가 일치하면 제거
    EXPECT_TRUE(j2::string::remove_prefix(s, "prefix-"));
    EXPECT_TRUE(j2::string::remove_suffix(s, "-suffix"));
    EXPECT_EQ(s, "body");

    // removed_prefix / removed_suffix: 원본은 유지하고 제거한 복사본 반환
    EXPECT_EQ(j2::string::removed_prefix("xx-body", "xx-"), "body");
    EXPECT_EQ(j2::string::removed_suffix("body-yy", "-yy"), "body");

    // safe_substr: 범위를 넘어가도 안전하게 빈 문자열 또는 가능한 범위 반환
    EXPECT_EQ(j2::string::safe_substr("abcdef", 2, 10), "cdef"); // 시작 2, 길이 10 → 끝까지
    EXPECT_EQ(j2::string::safe_substr("abcdef", 10, 2), "");     // 시작 위치가 문자열 길이보다 크면 빈 문자열
}

TEST(StringUtils, EllipsizeBasicAndUtf8Safe)
{
    // ellipsize: 바이트 기준 잘라내고 "..." 붙임
    EXPECT_EQ(j2::string::ellipsize("abcdef", 4), "a...");  // 4바이트 길이 제한 → "a" + "..."
    EXPECT_EQ(j2::string::ellipsize("abc", 5), "abc");      // 길이가 충분하면 그대로 반환

    // ellipsize_utf8_safe: UTF-8 코드포인트 기준으로 안전하게 자르기
    std::string utf8 = u8"한글ABC😊Z";
    // 코드포인트 5개만 남기고 "..." 추가
    // 남는 부분: "한","글","A","B","C"
    EXPECT_EQ(j2::string::ellipsize_utf8_safe(utf8, 5, "..."), std::string(u8"한글ABC") + "...");
}


/**
 * @brief 천 단위(3자리) 및 만 단위(4자리) 구분자 테스트
 */
TEST(StringUtils, AddSeparator) {
    // 1. 기본 3자리 구분 (양수)
    EXPECT_EQ(j2::string::add_separator("1234567"), u8"1,234,567");

    // 2. 기본 3자리 구분 (음수 및 소수점)
    EXPECT_EQ(j2::string::add_separator("-1234.56"), u8"-1,234.56");

    // 3. 한국식 4자리 구분
    EXPECT_EQ(j2::string::add_separator("123456789", 4), u8"1,2345,6789");

    // 4. 짧은 문자열 (구분 불필요)
    EXPECT_EQ(j2::string::add_separator("123"), u8"123");
    EXPECT_EQ(j2::string::add_separator("-12"), u8"-12");
    // 5. 빈 문자열 처리
    EXPECT_EQ(j2::string::add_separator(""), "");
}

/**
 * @brief 한국어 읽기 방식(만~대수) 변환 테스트
 */
TEST(StringUtils, ToHumanReadableKorean) {
    // 1. 일반적인 억/만 단위
    // "12345678900" -> "123억 4,567만 8,900"
    EXPECT_EQ(j2::string::to_human_readable_korean("12345678900"), u8"123억 4,567만 8,900");

    // 2. 중간 단위가 0인 경우 (생략 확인)
    // "100000001" -> "1억 1"
    EXPECT_EQ(j2::string::to_human_readable_korean("100000001"), u8"1억 1");
    // 3. 음수 및 소수점 포함
    EXPECT_EQ(j2::string::to_human_readable_korean("-50000.5"), u8"-5만.5");

    // 4. 콤마 미포함 옵션 테스트
    EXPECT_EQ(j2::string::to_human_readable_korean("12345", false), u8"1만 2345");
}

/**
 * @brief 거대 단위(경, 해 ... 대수) 경계값 테스트
 */
TEST(StringUtils, LargeScaleKoreanUnit) {
    // 1. '경' 단위 (10^16)
    // 1,0000,0000,0000,0000
    EXPECT_EQ(j2::string::to_human_readable_korean("10000000000000000"), u8"1경");

    // 2. '해' 단위 (10^20)
    EXPECT_EQ(j2::string::to_human_readable_korean("100000000000000000000"), u8"1해");

    // 3. 최종 단위 '대수' (10^72)
    // 1 뒤에 0이 72개 있는 경우
    std::string daesu_str = "1" + std::string(72, '0');
    EXPECT_EQ(j2::string::to_human_readable_korean(daesu_str), u8"1대수");

    // 4. 복합 거대 수치
    // "100020003" (조 위의 단위 예시) -> "1경 2조 3" (중간 0 생략 확인)
    EXPECT_EQ(j2::string::to_human_readable_korean("10002000000000003"), u8"1경 2조 3");
}

