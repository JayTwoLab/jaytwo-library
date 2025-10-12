// 파일명: tests/test_overload.cpp
// 목적: j2::core::overload 템플릿의 동작을 GoogleTest로 검증
//
// 포함 테스트 항목
// 1) 직접 호출 시 올바른 오버로드 선택 및 반환값 확인
// 2) std::variant + std::visit 조합에서 올바른 방문/해결 확인
// 3) 템플릿 추론 가이드(CTAD) 정상 동작 여부를 정적 확인
// 4) const char* 인자 전달 시 std::string 오버로드로의 합리적 변환 확인
//

#include <gtest/gtest.h>
#include <string>
#include <variant>
#include <type_traits>

#include "j2_library/core/overload.hpp"

// Test Suite: Overload
// - 업로드하신 overload.hpp(템플릿 상속과 using Ts::operator()...; 구조)를 사용합니다.
// - main.cpp에서 보신 variant<int, std::string> 방문 패턴을 기반으로 테스트합니다.

TEST(Overload, WorksWithDirectCall)
{
    // 두 개의 람다를 상속으로 합친 호출 객체를 생성합니다.
    // int -> int+1, std::string -> 문자열 길이 반환
    auto f = j2::core::overload{
        [](int x) { return x + 1; },
        [](const std::string& s) { return static_cast<int>(s.size()); }
    };

    // int에 대한 오버로드가 선택되어 41 -> 42 가 되어야 합니다.
    EXPECT_EQ(f(41), 42);

    // std::string에 대한 오버로드가 선택되어 길이 3을 반환해야 합니다.
    EXPECT_EQ(f(std::string("abc")), 3);

    // const char* 인자는 std::string로 합리적 변환이 일어나며 문자열 오버로드가 선택됩니다.
    EXPECT_EQ(f("hello"), 5);
}

TEST(Overload, WorksWithVariantVisit)
{
    // main.cpp와 동일한 타입 구성: std::variant<int, std::string>
    std::variant<int, std::string> v;

    // 방문 시 타입별로 다른 값을 반환: int -> x*2, string -> 길이
    auto g = j2::core::overload{
        [](int x) { return x * 2; },
        [](const std::string& s) { return static_cast<int>(s.size()); }
    };

    v = 42;
    // int 보관 시, int 오버로드가 선택되어 84 반환
    EXPECT_EQ(std::visit(g, v), 84);

    v = std::string("Hello");
    // string 보관 시, string 오버로드가 선택되어 길이 5 반환
    EXPECT_EQ(std::visit(g, v), 5);
}

TEST(Overload, DeductionGuideAndInvocability)
{
    // 템플릿 인수 명시 없이 CTAD(클래스 템플릿 인수 추론)로 타입이 결정되어야 합니다.
    auto h = j2::core::overload{
        [](int) {},
        [](const std::string&) {}
    };

    // 정적 확인: h가 int, std::string에 대해 호출 가능한지 점검합니다.
    static_assert(std::is_invocable_v<decltype(h), int>, "int 인자에 대해 호출 가능해야 합니다.");
    static_assert(std::is_invocable_v<decltype(h), std::string>, "std::string 인자에 대해 호출 가능해야 합니다.");

    // 실제 호출에서도 예외 없이 동작해야 합니다.
    EXPECT_NO_THROW(h(0));
    EXPECT_NO_THROW(h(std::string("ok")));
}

TEST(Overload, PreferExactMatchOverConvertible)
{
    // 동일한 의미의 두 오버로드가 있을 때, 정확히 일치하는 시그니처가 우선되어야 합니다.
    // 여기서는 const char* 와 std::string& 두 후보 중 const char*가 정확 일치입니다.
    bool called_cstr = false;
    bool called_str = false;

    auto ov = j2::core::overload{
        [&](const char*) { called_cstr = true; },
        [&](const std::string&) { called_str = true; }
    };

    const char* c = "hi";
    ov(c);

    EXPECT_TRUE(called_cstr);
    EXPECT_FALSE(called_str);
}
