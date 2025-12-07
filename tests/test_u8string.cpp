
#include <gtest/gtest.h>

#include "j2_library/j2_library.hpp"

// 테스트 편의를 위한 using
using j2::string::u8::u8str;
using j2::string::u8::u8string;

// 기본 생성자 / 기본 상태 테스트
// 기본 생성자 / 기본 상태 테스트
TEST(u8str_basic, default_ctor_and_empty)
{
    // 기본 생성 시 빈 문자열이어야 함
    u8str s;
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0u);
    EXPECT_EQ(s.to_std_string(), std::string());
}

// 다양한 타입으로부터의 생성자 테스트
TEST(u8str_basic, constructors_from_various_types)
{
    const char* cstr = "Hello";
    std::string std_str = "World";
#if defined(_WIN32)
    std::wstring wstr = L"윈도우";
#else
    std::wstring wstr = L"리눅스";
#endif
    std::u16string u16 = u"UTF16";
    std::u32string u32 = U"UTF32";

    u8str s1(cstr);
    u8str s2(std_str);
    u8str s3(wstr);
    u8str s4(u16);
    u8str s5(u32);

    EXPECT_EQ(s1.to_std_string(), "Hello");
    EXPECT_EQ(s2.to_std_string(), "World");
    EXPECT_FALSE(s3.to_std_string().empty());  // 플랫폼마다 내용은 다를 수 있음
    EXPECT_EQ(s4.to_std_string(), "UTF16");
    EXPECT_EQ(s5.to_std_string(), "UTF32");
}

// from_* 세터 함수 테스트
TEST(u8str_basic, from_setters)
{
    u8str s;
    s.from_std_string(std::string("Hello"));
    EXPECT_EQ(s.to_std_string(), "Hello");

    s.from_cstr("World");
    EXPECT_EQ(s.to_std_string(), "World");

    std::wstring w = L"테스트";
    s.from_wstring(w);
    EXPECT_FALSE(s.to_std_string().empty());

    s.from_u16string(u"u16");
    EXPECT_EQ(s.to_std_string(), "u16");

    s.from_u32string(U"u32");
    EXPECT_EQ(s.to_std_string(), "u32");
}

// clear / size / empty 테스트
TEST(u8str_basic, clear_size_empty)
{
    u8str s("내용있음");
    EXPECT_FALSE(s.empty());
    EXPECT_GT(s.size(), 0u);

    s.clear();
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0u);
}

// 변환 함수 테스트: to_wstring / to_u16 / to_u32
TEST(u8str_convert, to_w_u16_u32_roundtrip)
{
    // 이모지 포함 UTF-8 문자열
    u8str s("안녕하세요😎");

    std::wstring  ws = s.to_wstring();
    std::u16string u16 = s.to_u16string();
    std::u32string u32 = s.to_u32string();

    EXPECT_FALSE(ws.empty());
    EXPECT_FALSE(u16.empty());
    EXPECT_FALSE(u32.empty());

    // u16 / u32 에서 다시 UTF-8 로 되돌렸을 때 원본과 동일해야 함
    auto back_from_u16 = j2::string::u8::to_u8_string(u16);
    auto back_from_u32 = j2::string::u8::to_u8_string(u32);

    EXPECT_EQ(j2::string::u8::to_std_string(back_from_u16),
        s.to_std_string());
    EXPECT_EQ(j2::string::u8::to_std_string(back_from_u32),
        s.to_std_string());
}

// starts_with / ends_with / contains 테스트
TEST(u8str_search, starts_ends_contains)
{
    u8str s("안녕하세요 UTF-8 테스트");

    // starts_with (const char* 버전 포함)
    EXPECT_TRUE(s.starts_with("안녕"));
    EXPECT_FALSE(s.starts_with("UTF"));
    u8str prefix("안녕하세요");
    EXPECT_TRUE(s.starts_with(prefix));

    u8str too_long("안녕하세요 UTF-8 테스트 그리고 더 있습니다");
    EXPECT_FALSE(s.starts_with(too_long));

    // ends_with
    EXPECT_TRUE(s.ends_with("테스트"));
    EXPECT_FALSE(s.ends_with("안녕"));
    u8str suffix("테스트");
    EXPECT_TRUE(s.ends_with(suffix));

    EXPECT_FALSE(s.ends_with("매우 긴 접미사"));

    // contains
    EXPECT_TRUE(s.contains("UTF-8"));
    EXPECT_FALSE(s.contains("없는문자열"));
    u8str needle("UTF-8");
    EXPECT_TRUE(s.contains(needle));

    // needle 이 빈 문자열이면 항상 true
    EXPECT_TRUE(s.contains(""));
    EXPECT_TRUE(s.contains(u8str("")));
}

// index_of / last_index_of 테스트
TEST(u8str_search, index_of_and_last_index_of)
{
    u8str s("apple banana apple grape");

    // 첫 번째 "apple"
    EXPECT_EQ(s.index_of("apple"), 0);

    // 두 번째 "apple"
    auto idx_second = s.index_of("apple", 1);
    EXPECT_GT(idx_second, 0);

    // 없는 문자열
    EXPECT_EQ(s.index_of("orange"), -1);

    // needle 이 빈 문자열이면 0
    EXPECT_EQ(s.index_of(""), 0);

    // last_index_of
    auto last = s.last_index_of("apple");
    EXPECT_GT(last, 0);
    EXPECT_EQ(s.last_index_of("orange"), -1);
    EXPECT_EQ(s.last_index_of(""), 0);
}

// trim / ltrim / rtrim 테스트
TEST(u8str_trim, trim_functions)
{
    u8str s(" \t\nABC  ");
    s.ltrim();
    EXPECT_EQ(s.to_std_string(), "ABC  ");

    u8str s2(" \t\nABC  ");
    s2.rtrim();
    EXPECT_EQ(s2.to_std_string(), " \t\nABC");

    u8str s3(" \t\nABC  ");
    s3.trim();
    EXPECT_EQ(s3.to_std_string(), "ABC");

    // 전부 공백인 경우
    u8str s4("   \n\t  ");
    s4.trim();
    EXPECT_TRUE(s4.to_std_string().empty());

    // 이미 공백이 없는 경우
    u8str s5("ABC");
    s5.trim();
    EXPECT_EQ(s5.to_std_string(), "ABC");
}

// split 테스트
TEST(u8str_split, basic_and_edge_cases)
{
    u8str s("사과,배,포도,,수박");

    // skip_empty = false
    auto v1 = s.split(",", false);
    ASSERT_EQ(v1.size(), 5u);
    EXPECT_EQ(v1[0].to_std_string(), "사과");
    EXPECT_EQ(v1[1].to_std_string(), "배");
    EXPECT_EQ(v1[2].to_std_string(), "포도");
    EXPECT_EQ(v1[3].to_std_string(), "");
    EXPECT_EQ(v1[4].to_std_string(), "수박");

    // skip_empty = true
    auto v2 = s.split(",", true);
    ASSERT_EQ(v2.size(), 4u);
    EXPECT_EQ(v2[0].to_std_string(), "사과");
    EXPECT_EQ(v2[1].to_std_string(), "배");
    EXPECT_EQ(v2[2].to_std_string(), "포도");
    EXPECT_EQ(v2[3].to_std_string(), "수박");

    // 구분자가 없는 경우
    u8str s2("단일문자열");
    auto v3 = s2.split(",", false);
    ASSERT_EQ(v3.size(), 1u);
    EXPECT_EQ(v3[0].to_std_string(), "단일문자열");

    // delim 이 빈 문자열인 경우 (전체를 하나로)
    auto v4 = s2.split("", false);
    ASSERT_EQ(v4.size(), 1u);
    EXPECT_EQ(v4[0].to_std_string(), "단일문자열");
}

// replace / replace_all 테스트
TEST(u8str_replace, replace_and_replace_all)
{
    u8str s("안녕하세요, 세상. 안녕하세요, 여러분.");

    // 첫 번째 매치만 치환
    s.replace("안녕하세요", "Hello");
    EXPECT_EQ(s.to_std_string(), "Hello, 세상. 안녕하세요, 여러분.");

    // 모든 매치 치환
    s.replace_all("안녕하세요", "Hi");
    EXPECT_EQ(s.to_std_string(), "Hello, 세상. Hi, 여러분.");

    // u8str 버전
    u8str from("Hello");
    u8str to("안녕");
    s.replace_all(from, to);
    EXPECT_EQ(s.to_std_string(), "안녕, 세상. Hi, 여러분.");

    // from 이 빈 문자열인 경우: 아무 일도 일어나지 않아야 함
    u8str s2("AAA");
    s2.replace(u8str(""), u8str("B"));
    EXPECT_EQ(s2.to_std_string(), "AAA");

    s2.replace_all(u8str(""), u8str("B"));
    EXPECT_EQ(s2.to_std_string(), "AAA");
}

// left / right / mid / substr_utf8 테스트 (UTF-8 코드포인트 기준)
TEST(u8str_substring, left_right_mid_utf8)
{
    // "안녕" (2), "😀" (1), "하세요" (3) => 총 6 코드포인트
    u8str s("안녕😀하세요");

    auto l2 = s.left(2);
    auto r2 = s.right(2);
    auto m2_3 = s.mid(2, 3);
    auto sub2_3 = s.substr_utf8(2, 3);

    EXPECT_EQ(l2.to_std_string(), "안녕");
    EXPECT_EQ(r2.to_std_string(), "세요");
    EXPECT_EQ(m2_3.to_std_string(), "😀하세");
    EXPECT_EQ(sub2_3.to_std_string(), "😀하세");

    // n 이 0인 경우
    EXPECT_TRUE(s.left(0).to_std_string().empty());
    EXPECT_TRUE(s.right(0).to_std_string().empty());

    // n 이 전체 길이 이상인 경우
    auto all_left = s.left(100);
    auto all_right = s.right(100);
    EXPECT_EQ(all_left.to_std_string(), s.to_std_string());
    EXPECT_EQ(all_right.to_std_string(), s.to_std_string());

    // pos 가 범위를 벗어난 경우
    EXPECT_TRUE(s.mid(100, 2).to_std_string().empty());

    // count 가 0인 경우
    EXPECT_TRUE(s.mid(0, 0).to_std_string().empty());

    // 빈 문자열 대상
    u8str empty_s;
    EXPECT_TRUE(empty_s.left(3).to_std_string().empty());
    EXPECT_TRUE(empty_s.right(3).to_std_string().empty());
    EXPECT_TRUE(empty_s.mid(0, 1).to_std_string().empty());
}

// reverse_utf8 테스트
TEST(u8str_algo, reverse_utf8)
{
    u8str s("ABC안녕😀");
    u8str original = s;

    // 1글자 / 빈 문자열은 변화가 없어야 함
    u8str one("A");
    u8str empty;
    one.reverse_utf8();
    empty.reverse_utf8();
    EXPECT_EQ(one.to_std_string(), "A");
    EXPECT_TRUE(empty.to_std_string().empty());

    // 두 번 뒤집으면 원래 문자열과 같아야 함
    s.reverse_utf8();
    s.reverse_utf8();
    EXPECT_EQ(s.to_std_string(), original.to_std_string());
}

// pad_left / pad_right 테스트
TEST(u8str_algo, pad_left_right)
{
    u8str s("가나");  // 코드포인트 2개

    s.pad_left(4, '_');
    EXPECT_EQ(s.to_std_string(), "__가나");

    s.pad_right(6, '_');
    EXPECT_EQ(s.to_std_string(), "__가나__");

    // 이미 길이가 충분한 경우: 변경되지 않아야 함
    u8str s2("ABCDE");
    s2.pad_left(3, '_');
    s2.pad_right(3, '_');
    EXPECT_EQ(s2.to_std_string(), "ABCDE");
}

// strip_prefix / strip_suffix 테스트
TEST(u8str_algo, strip_prefix_suffix)
{
    u8str s("HelloWorld");

    EXPECT_TRUE(s.strip_prefix("Hello"));
    EXPECT_EQ(s.to_std_string(), "World");

    EXPECT_FALSE(s.strip_prefix("XXX"));
    EXPECT_EQ(s.to_std_string(), "World");

    EXPECT_TRUE(s.strip_suffix("World"));
    EXPECT_TRUE(s.to_std_string().empty());

    s.from_cstr("ABCDEF");
    EXPECT_FALSE(s.strip_suffix("ZZZ"));
    EXPECT_EQ(s.to_std_string(), "ABCDEF");

    // u8str 버전 prefix
    u8str s2("앞부분제거");
    u8str prefix("앞부분");
    EXPECT_TRUE(s2.strip_prefix(prefix));
    EXPECT_EQ(s2.to_std_string(), "제거");
}

// equals_ignore_case_ascii 테스트
TEST(u8str_algo, equals_ignore_case_ascii)
{
    u8str s1("HelloWorld");
    u8str s2("helloworld");
    u8str s3("HELLO_WORLD");
    u8str s4("Hello");

    EXPECT_TRUE(s1.equals_ignore_case_ascii(s2));
    EXPECT_TRUE(s1.equals_ignore_case_ascii("helloworld"));
    EXPECT_FALSE(s1.equals_ignore_case_ascii(s3));  // '_' 때문에 다른 문자열
    EXPECT_FALSE(s1.equals_ignore_case_ascii(s4));  // 길이 다름

    // 비 ASCII 문자 포함: ASCII 부분만 대소문자 비교 대상
    u8str s5("안녕Hello");
    u8str s6("안녕hello");
    EXPECT_TRUE(s5.equals_ignore_case_ascii(s6));
}

// to_lower / to_upper / copy 버전 테스트
TEST(u8str_algo, to_lower_upper)
{
    u8str s("AbCdEf123");
    s.to_lower();
    EXPECT_EQ(s.to_std_string(), "abcdef123");

    s.to_upper();
    EXPECT_EQ(s.to_std_string(), "ABCDEF123");

    u8str s2("Hello World!");
    auto lower = s2.to_lower_copy();
    auto upper = s2.to_upper_copy();

    EXPECT_EQ(s2.to_std_string(), "Hello World!");
    EXPECT_EQ(lower.to_std_string(), "hello world!");
    EXPECT_EQ(upper.to_std_string(), "HELLO WORLD!");
}

// 연산자 테스트: +=, +, ==, !=
TEST(u8str_operators, plus_and_compare)
{
    u8str a("Hello");
    u8str b("World");

    a += " ";
    a += b;
    EXPECT_EQ(a.to_std_string(), "Hello World");

    u8str c = u8("Hello") + " " + b;
    EXPECT_EQ(c.to_std_string(), "Hello World");

    u8str d("Hello World");
    EXPECT_TRUE(c == d);
    EXPECT_FALSE(c != d);

    u8str e("Different");
    EXPECT_FALSE(c == e);
    EXPECT_TRUE(c != e);

    // const char* + u8str 조합 테스트
    u8str f = "Say " + u8str("Hi");
    EXPECT_EQ(f.to_std_string(), "Say Hi");
}
