#pragma once

#include <utility>

#include "j2_library/export.hpp"

namespace j2::overload
{
    // -------------------------------------------------------------------------
    // overload
    //
    // 여러 개의 람다(또는 함수 객체)를 상속하여 하나의 함수 객체로
    // 결합하는 유틸리티입니다. C++17의 `using ...` 구문을 통해
    // 각 람다의 operator()를 상속받아 오버로드를 지원합니다.
    //
    // 주로 std::variant, std::visit과 함께 사용하면 코드가 간결해집니다.
    //
    // 사용 예시:
    // -------------------------------------------------------------------------
    //
    // // std::variant와 함께 사용하기
    // std::variant<int, std::string> v = "hello";
    // std::visit(j2::core::overload{
    //     [](int i) { std::cout << "int: " << i << '\n'; },
    //     [](const std::string& s) { std::cout << "string: " << s << '\n'; }
    // }, v);
    //
    // // 결과 출력:
    // // string: hello
    //
    // -------------------------------------------------------------------------
    // // 여러 타입에 대해 하나의 호출 객체 생성하기
    // auto f = j2::core::overload{
    //     [](int i) { return i * 2; },
    //     [](double d) { return d + 0.5; },
    //     [](const std::string& s) { return s.size(); }
    // };
    //
    // std::cout << f(10) << '\n';          // 20
    // std::cout << f(3.14) << '\n';        // 3.64
    // std::cout << f("test"s) << '\n';     // 4
    //
    // -------------------------------------------------------------------------
    // // if constexpr과 결합한 예시
    // template <typename T>
    // void process(const T& value) {
    //     auto visitor = j2::core::overload{
    //         [](int i) { std::cout << "integer: " << i << '\n'; },
    //         [](auto&& other) {
    //             std::cout << "other type\n";
    //         }
    //     };
    //     visitor(value);
    // }
    //
    // process(42);        // integer: 42
    // process(3.14);      // other type
    //
    // -------------------------------------------------------------------------
    template <typename... Ts>
    struct overload : Ts... {
        using Ts::operator()...;
    };

    // -------------------------------------------------------------------------
    // Deduction guide
    //
    // C++17 클래스 템플릿 인자 추론(CTAD)을 활용하여,
    // `overload{...}` 와 같이 사용할 때 템플릿 파라미터를
    // 명시하지 않아도 자동으로 추론되도록 합니다.
    //
    // 예:
    // auto f = overload{
    //     [](int) { return 1; },
    //     [](double) { return 2; }
    // };
    // // 여기서 f는 overload<lambda(int), lambda(double)> 타입으로 추론됨
    // -------------------------------------------------------------------------
    template <typename... Ts>
    overload(Ts...) -> overload<Ts...>;

} // namespace j2::core
