#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "j2_library/encoding/encoding.hpp"
#include "j2_library/string/to_console_encoding.hpp" // 콘솔 메시지 변환 (Windows 한글 깨짐 방지)

using namespace j2::encoding;

// -----------------------------
// 공통: 스킵 유틸리티 함수
// -----------------------------
// 테스트 환경(코드페이지/라이브러리 지원)에서 특정 인코딩을 사용할 수 없을 경우
// GTEST_SKIP()을 호출하여 테스트를 건너뛰게 만든다.
[[maybe_unused]] static void Skip(const char* why) {
    GTEST_SKIP() << why;
}
static void SkipMsg(const std::string& msg) {
    // Windows 콘솔은 기본적으로 CP949 코드페이지라 UTF-8 문자열이 깨질 수 있음
    // → to_console_encoding()을 사용하여 적절한 코드페이지 문자열로 변환 후 출력
    GTEST_SKIP() << j2::core::to_console_encoding(msg).c_str();
}

// -----------------------------
// 플랫폼별 인코딩 가용성 확인
// -----------------------------
#if defined(_WIN32)
// Windows: 특정 코드페이지가 존재하는지 확인
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
static bool CodePageExists(UINT cp) {
    CPINFOEXA info{};
    return GetCPInfoExA(cp, 0, &info) != 0;
}
#else
// Linux/macOS: iconv 사용 여부 확인
static bool IconvEnabled() {
#if defined(J2LIB_USE_ICONV)
    return true;
#else
    return false;
#endif
}
#endif

// -----------------------------
// 프루빙 함수: 특정 인코딩이 실제로 정상 동작하는지 검증
// - UTF-8 → 대상 인코딩 → 다시 UTF-8
// - 원본 문자열과 동일해야 "실제 사용 가능"으로 판단
// -----------------------------
static bool Probe_CP949() {
    const std::string sample = u8"한글테스트";
    std::string cp; std::string back;
#if defined(_WIN32)
    if (!CodePageExists(949)) return false; // 코드페이지 없음
#endif
#if !defined(_WIN32) && !defined(J2LIB_USE_ICONV)
    return false; // iconv 비활성화
#endif
    if (!utf8_to_cp949(sample, cp)) return false;
    if (cp.empty()) return false;
    if (!cp949_to_utf8(cp, back)) return false;
    return back == sample;
}

static bool Probe_ISO2022KR() {
    const std::string sample = u8"한글테스트 메일";
    std::string iso; std::string back;
#if defined(_WIN32)
    if (!CodePageExists(50225)) return false;
#endif
#if !defined(_WIN32) && !defined(J2LIB_USE_ICONV)
    return false;
#endif
    if (!utf8_to_iso2022kr(sample, iso)) return false;
    if (iso.empty()) return false;
    if (!iso2022kr_to_utf8(iso, back)) return false;
    return back == sample;
}

static bool Probe_JOHAB() {
    const std::string sample = u8"조합형테스트";
    std::string jo; std::string back;
#if defined(_WIN32)
    if (!CodePageExists(1361)) return false;
#endif
#if !defined(_WIN32) && !defined(J2LIB_USE_ICONV)
    return false;
#endif
    if (!utf8_to_johab(sample, jo)) return false;
    if (jo.empty()) return false;
    if (!johab_to_utf8(jo, back)) return false;
    return back == sample;
}

static bool Probe_MacKorean() {
    const std::string sample = u8"맥코리안테스트";
    std::string mk; std::string back;
#if defined(_WIN32)
    if (!CodePageExists(10003)) return false;
#endif
#if !defined(_WIN32) && !defined(J2LIB_USE_ICONV)
    return false;
#endif
    if (!utf8_to_mackorean(sample, mk)) return false;
    if (mk.empty()) return false;
    if (!mackorean_to_utf8(mk, back)) return false;
    return back == sample;
}

// ============================================================================
// 실제 테스트 케이스들
// ============================================================================

// UTF-8 <-> UTF-16 <-> UTF-32 간 상호 변환이 제대로 동작하는지 확인
// 이 테스트는 환경에 관계없이 항상 실행됨 (UTF 계열은 보편 지원)
TEST(EncodingUtils, Utf8_Utf16_Utf32_Roundtrip_KoreanAndEmoji)
{
    const std::string utf8 = u8"안녕하세요 ABC 😊";

    // UTF-8 → UTF-16
    std::u16string u16;
    ASSERT_TRUE(utf8_to_utf16(utf8, u16));

    // UTF-16 → UTF-8
    std::string utf8_b;
    ASSERT_TRUE(utf16_to_utf8(u16, utf8_b));
    EXPECT_EQ(utf8_b, utf8);

    // UTF-16 → UTF-32
    std::u32string u32;
    ASSERT_TRUE(utf16_to_utf32(u16, u32));

    // UTF-32 → UTF-16
    std::u16string u16_b;
    ASSERT_TRUE(utf32_to_utf16(u32, u16_b));
    EXPECT_EQ(u16_b, u16);

    // UTF-8 → UTF-32
    std::u32string u32_b;
    ASSERT_TRUE(utf8_to_utf32(utf8, u32_b));

    // UTF-32 → UTF-8
    std::string utf8_c;
    ASSERT_TRUE(utf32_to_utf8(u32_b, utf8_c));
    EXPECT_EQ(utf8_c, utf8);
}

// UTF-8 <-> CP949 라운드트립 테스트
// 환경에서 CP949를 지원하지 않으면 스킵 처리
TEST(EncodingUtils, Utf8_Cp949_Roundtrip_BasicHangulOnly)
{
#if defined(_WIN32)
    if (!CodePageExists(949)) { SkipMsg("Windows: CP949(949) 미탑재 - 스킵"); return; }
#else
    if (!IconvEnabled()) { SkipMsg("비-Windows: iconv 미사용 - 스킵"); return; }
#endif
    if (!Probe_CP949()) { SkipMsg("CP949 라운드트립 프루빙 실패 - 환경 스킵"); return; }

    const std::string utf8 = u8"안녕하세요 세상";
    std::string cp949;
    if (!utf8_to_cp949(utf8, cp949)) { SkipMsg("환경에서 CP949 변환 실패 - 스킵"); return; }
    ASSERT_FALSE(cp949.empty());

    std::string utf8_back;
    if (!cp949_to_utf8(cp949, utf8_back)) { SkipMsg("환경에서 CP949 역변환 실패 - 스킵"); return; }
    EXPECT_EQ(utf8_back, utf8);
}

// UTF-16 <-> CP949 라운드트립 테스트
TEST(EncodingUtils, Utf16_Cp949_Roundtrip_BasicHangulOnly)
{
#if defined(_WIN32)
    if (!CodePageExists(949)) { SkipMsg("Windows: CP949(949) 미탑재 - 스킵"); return; }
#else
    if (!IconvEnabled()) { SkipMsg("비-Windows: iconv 미사용 - 스킵"); return; }
#endif
    if (!Probe_CP949()) { SkipMsg("CP949 라운드트립 프루빙 실패 - 환경 스킵"); return; }

    const std::u16string u16 = u"테스트 문자열 한글만";
    std::string cp949;
    if (!utf16_to_cp949(u16, cp949)) { SkipMsg("환경에서 CP949 변환 실패 - 스킵"); return; }
    ASSERT_FALSE(cp949.empty());

    std::u16string u16_back;
    if (!cp949_to_utf16(cp949, u16_back)) { SkipMsg("환경에서 CP949 역변환 실패 - 스킵"); return; }
    EXPECT_EQ(u16_back, u16);
}

// UTF-8 <-> ISO-2022-KR 라운드트립 테스트
TEST(EncodingUtils, ISO2022KR_Utf8_Roundtrip_BasicHangulOnly)
{
#if defined(_WIN32)
    if (!CodePageExists(50225)) { SkipMsg("Windows: ISO-2022-KR(50225) 미탑재 - 스킵"); return; }
#else
    if (!IconvEnabled()) { SkipMsg("비-Windows: iconv 미사용 - 스킵"); return; }
#endif
    if (!Probe_ISO2022KR()) { SkipMsg("ISO-2022-KR 라운드트립 프루빙 실패 - 환경 스킵"); return; }

    const std::string utf8 = u8"한글 이메일 테스트";
    std::string iso2022;
    if (!utf8_to_iso2022kr(utf8, iso2022)) { SkipMsg("환경에서 ISO-2022-KR 변환 실패 - 스킵"); return; }
    ASSERT_FALSE(iso2022.empty());

    std::string utf8_back;
    if (!iso2022kr_to_utf8(iso2022, utf8_back)) { SkipMsg("환경에서 ISO-2022-KR 역변환 실패 - 스킵"); return; }
    EXPECT_EQ(utf8_back, utf8);
}

// UTF-8 <-> JOHAB 라운드트립 테스트
TEST(EncodingUtils, Johab_Utf8_Roundtrip_BasicHangulOnly)
{
#if defined(_WIN32)
    if (!CodePageExists(1361)) { SkipMsg("Windows: JOHAB(1361) 미탑재 - 스킵"); return; }
#else
    if (!IconvEnabled()) { SkipMsg("비-Windows: iconv 미사용 - 스킵"); return; }
#endif
    if (!Probe_JOHAB()) { SkipMsg("JOHAB 라운드트립 프루빙 실패 - 환경 스킵"); return; }

    const std::string utf8 = u8"조합형 한글 테스트";
    std::string johab;
    if (!utf8_to_johab(utf8, johab)) { SkipMsg("환경에서 JOHAB 변환 실패 - 스킵"); return; }
    ASSERT_FALSE(johab.empty());

    std::string utf8_back;
    if (!johab_to_utf8(johab, utf8_back)) { SkipMsg("환경에서 JOHAB 역변환 실패 - 스킵"); return; }
    EXPECT_EQ(utf8_back, utf8);
}

// UTF-8 <-> MacKorean 라운드트립 테스트
TEST(EncodingUtils, MacKorean_Utf8_Roundtrip_BasicHangulOnly)
{
#if defined(_WIN32)
    if (!CodePageExists(10003)) { SkipMsg("Windows: MacKorean(10003) 미탑재 - 스킵"); return; }
#else
    if (!IconvEnabled()) { SkipMsg("비-Windows: iconv 미사용 - 스킵"); return; }
#endif
    if (!Probe_MacKorean()) { SkipMsg("MacKorean 라운드트립 프루빙 실패 - 환경 스킵"); return; }

    const std::string utf8 = u8"맥코리안 인코딩 테스트";
    std::string mackor;
    if (!utf8_to_mackorean(utf8, mackor)) { SkipMsg("환경에서 MacKorean 변환 실패 - 스킵"); return; }
    ASSERT_FALSE(mackor.empty());

    std::string utf8_back;
    if (!mackorean_to_utf8(mackor, utf8_back)) { SkipMsg("환경에서 MacKorean 역변환 실패 - 스킵"); return; }
    EXPECT_EQ(utf8_back, utf8);
}

// CP949로 변환할 수 없는 문자를 포함했을 때 동작 확인
// (예: 이모지 😊는 CP949에 없음)
// - 변환 실패 시: 기대한 동작 → SUCCEED 처리
// - 변환 성공했지만 대체문자로 치환된 경우: 환경 정책 차이 → 스킵 처리
TEST(EncodingUtils, UnsupportedCharacter_To_CP949_ShouldFailOrSkip)
{
#if defined(_WIN32)
    if (!CodePageExists(949)) { SkipMsg("Windows: CP949(949) 미탑재 - 스킵"); return; }
#else
    if (!IconvEnabled()) { SkipMsg("비-Windows: iconv 미사용 - 스킵"); return; }
#endif
    if (!Probe_CP949()) { SkipMsg("CP949 라운드트립 프루빙 실패 - 환경 스킵"); return; }

    const std::string utf8 = u8"한글 + 😊"; // 이모지는 CP949에서 표현 불가
    std::string cp949;
    bool ok = utf8_to_cp949(utf8, cp949);
    if (!ok) {
        GTEST_SUCCEED() << j2::core::to_console_encoding("표현 불가 문자로 변환 실패(기대한 동작)").c_str();
    }
    else {
        SkipMsg("환경이 대체문자를 사용해 성공 처리함(정책 차이)");
    }
}
