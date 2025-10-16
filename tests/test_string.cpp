#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "j2_library/string/string.hpp"
#include "j2_library/string/to_console_encoding.hpp"
 
using namespace j2::string;

// 문자열 유틸 전반을 검증하는 테스트 스위트
// - 트림(공백 제거) / 치환(문자열 바꾸기)
// - 대소문 변환 / 접두사·접미사 확인
// - 분할(split)/결합(join) / 패딩 / 말줄임 등
TEST(StringUtils, TrimBasic)
{
    // ltrim: 문자열 앞쪽 공백 제거
    std::string s1 = "   abc  ";
    ltrim(s1);
    EXPECT_EQ(s1, "abc  ");  // 앞 공백 3개 제거 → "abc" + 뒤 공백 2개 유지

    // rtrim: 문자열 뒤쪽 공백 제거
    rtrim(s1);
    EXPECT_EQ(s1, "abc");    // 뒤 공백 제거 후 "abc"만 남음

    // trim_copy: 문자열 앞뒤 공백 모두 제거 (복사본 리턴)
    std::string s2 = " \t\n  xyz \r\n ";
    EXPECT_EQ(trim_copy(s2), "xyz");
}

TEST(StringUtils, ReplaceVariants)
{
    // replace: 원본 문자열의 모든 토큰을 새 토큰으로 바꾼 복사본 리턴
    EXPECT_EQ(replace("a_b_c_b", "b", "X"), "a_X_c_X");

    // replace_all_inplace: 제자리 치환 (원본 문자열 자체가 수정됨)
    std::string t = "foo foo bar foo";
    replace_all_inplace(t, "foo", "qux");
    EXPECT_EQ(t, "qux qux bar qux");

    // replace_first: 첫 번째 매치만 교체
    EXPECT_EQ(replace_first("aaabaa", "aa", "Z"), "Zabaa");

    // replace_last: 마지막 매치만 교체
    EXPECT_EQ(replace_last("aaabaa", "aa", "Z"), "aaabZ");
}

TEST(StringUtils, CaseAndSearch)
{
    // 대소문 변환
    EXPECT_EQ(to_lower("AbC123"), "abc123");
    EXPECT_EQ(to_upper("AbC123"), "ABC123");

    // 문자열 포함 여부 / 접두사 / 접미사 확인
    EXPECT_TRUE(contains("hello world", "world"));
    EXPECT_TRUE(starts_with("foobar", "foo"));
    EXPECT_TRUE(ends_with("foobar", "bar"));

    // 대소문 무시 비교 (i = ignore case)
    EXPECT_TRUE(iequals("AbC", "aBc"));
    EXPECT_TRUE(icontains("Hello World", "WORLD"));
    EXPECT_TRUE(istarts_with("Prefix", "pre"));
    EXPECT_TRUE(iends_with("Suffix", "FIX"));
}

TEST(StringUtils, SplitJoin)
{
    // split: 구분자 기준으로 문자열 분리 (문자 단위)
    auto v1 = split("a,b,,c", ',');
    ASSERT_EQ(v1.size(), 4u);
    EXPECT_EQ(v1[0], "a");
    EXPECT_EQ(v1[1], "b");
    EXPECT_EQ(v1[2], "");   // 빈 토큰도 유지
    EXPECT_EQ(v1[3], "c");

    // split: 구분자 기준 (문자열 단위)
    auto v2 = split("one<>two<>three", "<>", false);
    ASSERT_EQ(v2.size(), 3u);
    EXPECT_EQ(v2[0], "one");
    EXPECT_EQ(v2[1], "two");
    EXPECT_EQ(v2[2], "three");

    // join: 벡터의 문자열을 구분자 넣어 합치기
    EXPECT_EQ(join(v2, "|"), "one|two|three");

    // split_trimmed: 분할 후 각 토큰을 trim, 빈 문자열은 제거
    auto v3 = split_trimmed("  a ,  b ,   , c  ", ',', true);
    ASSERT_EQ(v3.size(), 3u);
    EXPECT_EQ(v3[0], "a");
    EXPECT_EQ(v3[1], "b");
    EXPECT_EQ(v3[2], "c");

    // split_lines: 개행 기준 분할 (CR, LF, CRLF 모두 처리)
    auto v4 = split_lines("a\r\nb\nc\rd", false);
    std::vector<std::string> expect4 = { "a","b","c","d" };
    EXPECT_EQ(v4, expect4);
}

TEST(StringUtils, PaddingRemoveCharsRepeat)
{
    // pad_*: 문자열 길이가 n이 되도록 특정 문자로 채우기
    EXPECT_EQ(pad_left("7", 3, '0'), "007");
    EXPECT_EQ(pad_right("7", 3, '0'), "700");
    EXPECT_EQ(pad_center("7", 3, '0'), "070");

    // remove_chars: 특정 문자 집합을 제거
    EXPECT_EQ(remove_chars("a-b_c.d", "-_."), "abcd");

    // repeat: 문자열 반복
    EXPECT_EQ(repeat("ab", 3), "ababab");
}

TEST(StringUtils, CollapseNormalize)
{
    // collapse_spaces: 연속된 공백/개행/탭을 단일 공백으로 축약, 앞뒤 공백 제거
    EXPECT_EQ(collapse_spaces("  a \t  b \n  c  "), "a b c");

    // normalize_newlines: 모든 종류의 개행(CR, LF, CRLF)을 지정 문자열로 통일
    EXPECT_EQ(normalize_newlines("a\r\nb\rc\n", "\n"), "a\nb\nc\n");

    // is_blank: 문자열이 비었거나 공백만 있으면 true
    EXPECT_TRUE(is_blank(" \t \n "));
    EXPECT_FALSE(is_blank("  x "));
}

TEST(StringUtils, PrefixSuffixRemoveAndSafeSubstr)
{
    std::string s = "prefix-body-suffix";

    // remove_prefix / remove_suffix: 접두사/접미사가 일치하면 제거
    EXPECT_TRUE(remove_prefix(s, "prefix-"));
    EXPECT_TRUE(remove_suffix(s, "-suffix"));
    EXPECT_EQ(s, "body");

    // removed_prefix / removed_suffix: 원본은 유지하고 제거한 복사본 반환
    EXPECT_EQ(removed_prefix("xx-body", "xx-"), "body");
    EXPECT_EQ(removed_suffix("body-yy", "-yy"), "body");

    // safe_substr: 범위를 넘어가도 안전하게 빈 문자열 또는 가능한 범위 반환
    EXPECT_EQ(safe_substr("abcdef", 2, 10), "cdef"); // 시작 2, 길이 10 → 끝까지
    EXPECT_EQ(safe_substr("abcdef", 10, 2), "");     // 시작 위치가 문자열 길이보다 크면 빈 문자열
}

TEST(StringUtils, EllipsizeBasicAndUtf8Safe)
{
    // ellipsize: 바이트 기준 잘라내고 "..." 붙임
    EXPECT_EQ(ellipsize("abcdef", 4), "a...");  // 4바이트 길이 제한 → "a" + "..."
    EXPECT_EQ(ellipsize("abc", 5), "abc");      // 길이가 충분하면 그대로 반환

    // ellipsize_utf8_safe: UTF-8 코드포인트 기준으로 안전하게 자르기
    std::string utf8 = u8"한글ABC😊Z";
    // 코드포인트 5개만 남기고 "..." 추가
    // 남는 부분: "한","글","A","B","C"
    EXPECT_EQ(ellipsize_utf8_safe(utf8, 5, "..."), std::string(u8"한글ABC") + "...");
}
