// test_try_opt.cpp
// GoogleTest로 TRY_OPT/TRY_PTR 계열 매크로 동작을 검증하는 테스트
// - 성공 시 값 언래핑/바인딩
// - 실패 시 상위 함수의 반환 타입에 맞게 기본값/nullopt/return 전파
// - void 반환 함수에서의 조용한 종료
//
// 빌드 예:
//   g++ -std=c++17 test_try_opt.cpp -I. -lgtest -lpthread -o test_try_opt
//
// 주의: 프로젝트 구조에 맞게 include 경로를 조정하세요.

#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <vector>
#include <cctype>
#include "j2_library/macro/try_opt.hpp"

// -----------------------------
// 데모용 보조 함수들
// -----------------------------
// 선택적으로 문자열을 반환: ok면 "JayTwo42", 아니면 nullopt
static std::optional<std::string> get_user_name(bool ok = true) {
    if (!ok) return std::nullopt;
    return std::string("JayTwo42");
}

// 문자열 끝 숫자를 정수로 파싱: 없으면 nullopt
static std::optional<int> parse_tail_number(const std::string& s) {
    if (s.empty()) return std::nullopt;
    char c = s.back();
    if (c < '0' || c > '9') return std::nullopt;
    return static_cast<int>(c - '0');
}

// 포인터 반환(데모): ok면 정적 변수 주소, 아니면 nullptr
static int* get_int_ptr(bool ok = true) {
    static int v = 5;
    return ok ? &v : nullptr;
}

// -----------------------------
// TRY_OPT 매크로를 사용하는 함수들
// -----------------------------

// 1) std::optional<std::string> 반환: 실패 시 nullopt 전파
static std::optional<std::string> make_greeting_opt(bool ok_user) {
    // 실패 시 현재 함수는 즉시 nullopt 반환
    TRY_OPT(name, get_user_name(ok_user));
    return std::string("Hello, ") + name;
}

// 2) 프리머티브(int) 반환: 실패 시 기본값(0) 반환
static int name_length_or_zero(bool ok_user) {
    TRY_OPT(name, get_user_name(ok_user));  // 실패면 int{} == 0 반환
    return static_cast<int>(name.size());
}

// 3) opt/int 체인: 중첩 언래핑
static std::optional<int> tail_number_plus_one(bool ok_user) {
    TRY_OPT(name, get_user_name(ok_user));      // nullopt 전파
    TRY_OPT(num, parse_tail_number(name));     // nullopt 전파
    return num + 1;                             // 성공 시 값
}

// 4) TRY_OPT_NO_BIND: 바인딩 없이 존재성만 검증
static std::optional<int> always_42_if_name_exists(bool ok_user) {
    TRY_OPT_NO_BIND(get_user_name(ok_user));    // 실패 시 nullopt
    return 42;                                  // 성공 시 42
}

// 5) void 반환: TRY_OPT_VOID / TRY_OPT_BIND_VOID
static void push_upper_name_or_return(bool ok_user, std::vector<std::string>& out) {
    // 값 바인딩 없이 존재성만 검사 → 실패 시 조용히 return
    TRY_OPT_VOID(get_user_name(ok_user));
    // 값이 필요하면 바인딩 버전 사용
    TRY_OPT_BIND_VOID(name, get_user_name(ok_user)); // 실패 시 조용히 return
    for (auto& ch : name) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    out.push_back(name);
}

// -----------------------------
// 포인터 계열 매크로(TRY_PTR ...) 함수들
// -----------------------------

// 6) optional<int> 반환: nullptr이면 nullopt 전파
static std::optional<int> read_ptr_value_opt(bool ok_ptr) {
    TRY_PTR(p, get_int_ptr(ok_ptr));  // nullptr이면 nullopt 반환
    return *p;
}

// 7) int 반환: nullptr이면 기본값 0 반환
static int read_ptr_value_or_zero(bool ok_ptr) {
    TRY_PTR(p, get_int_ptr(ok_ptr));  // nullptr이면 int{} == 0 반환
    return *p + 10;                   // 5 + 10 == 15 (정상 케이스)
}

// 8) void 반환: nullptr이면 조용히 return
static void add_ptr_value_or_return(bool ok_ptr, std::vector<int>& out) {
    TRY_PTR_BIND_VOID(p, get_int_ptr(ok_ptr));  // nullptr → return
    out.push_back(*p);
}

// -----------------------------
// 테스트 케이스들
// -----------------------------

TEST(TryOpt, OptionalReturn_SuccessAndFailure) {
    // 성공 케이스: "Hello, JayTwo42"
    auto g1 = make_greeting_opt(true);
    ASSERT_TRUE(g1.has_value());
    EXPECT_NE(g1->find("Hello, JayTwo42"), std::string::npos);

    // 실패 케이스: nullopt
    auto g2 = make_greeting_opt(false);
    EXPECT_FALSE(g2.has_value());
}

TEST(TryOpt, PrimitiveReturn_DefaultOnFailure) {
    // 성공 시 길이 > 0
    int len_ok = name_length_or_zero(true);
    EXPECT_GT(len_ok, 0);

    // 실패 시 0 (기본값)
    int len_fail = name_length_or_zero(false);
    EXPECT_EQ(len_fail, 0);
}

TEST(TryOpt, NestedUnwrap_OptionalChain) {
    // "JayTwo42" → tail 숫자 2 → +1 == 3
    auto v1 = tail_number_plus_one(true);
    ASSERT_TRUE(v1.has_value());
    EXPECT_EQ(*v1, 3);

    // 이름이 없으면 nullopt
    auto v2 = tail_number_plus_one(false);
    EXPECT_FALSE(v2.has_value());
}

TEST(TryOpt, NoBind_CheckOnly) {
    // 이름이 있으면 42
    auto v1 = always_42_if_name_exists(true);
    ASSERT_TRUE(v1.has_value());
    EXPECT_EQ(*v1, 42);

    // 이름이 없으면 nullopt
    auto v2 = always_42_if_name_exists(false);
    EXPECT_FALSE(v2.has_value());
}

TEST(TryOpt, VoidVariants_SilentReturn) {
    std::vector<std::string> out;

    // 성공: 대문자 변환된 이름 푸시됨
    push_upper_name_or_return(true, out);
    ASSERT_EQ(out.size(), 1u);
    EXPECT_EQ(out[0], "JAYTWO42");

    // 실패: 조용히 return, 변동 없음
    push_upper_name_or_return(false, out);
    EXPECT_EQ(out.size(), 1u);
}

TEST(TryPtr, PointerMacros_Basic) {
    // optional<int> 반환: nullptr이면 nullopt
    auto o1 = read_ptr_value_opt(true);
    ASSERT_TRUE(o1.has_value());
    EXPECT_EQ(*o1, 5);

    auto o2 = read_ptr_value_opt(false);
    EXPECT_FALSE(o2.has_value());

    // int 반환: nullptr이면 0 (기본값)
    int v_ok = read_ptr_value_or_zero(true);
    EXPECT_EQ(v_ok, 15); // 5 + 10

    int v_fail = read_ptr_value_or_zero(false);
    EXPECT_EQ(v_fail, 0);
}

TEST(TryPtr, PointerMacros_Void) {
    std::vector<int> out;

    // 성공: 값 5가 push
    add_ptr_value_or_return(true, out);
    ASSERT_EQ(out.size(), 1u);
    EXPECT_EQ(out[0], 5);

    // 실패: 조용히 return, 크기 유지
    add_ptr_value_or_return(false, out);
    EXPECT_EQ(out.size(), 1u);
}
