#pragma once

// ========================================================================
// Export/Import 매크로 사용 가이드 (J2LIB_API / J2LIB_HIDDEN)
//
// [개요]
// - 동적 라이브러리(DLL/so)에서 외부로 공개할 심볼(class, struct, 함수 등)에
//   J2LIB_API를 붙여 export/import를 제어합니다.
// - 내부 전용 심볼에는 J2LIB_HIDDEN을 붙여 숨깁니다.
// - 정적 라이브러리만 사용할 경우에는 의미가 크지 않으나, 
//   동일 헤더를 동적/정적 빌드에서 재사용하려면 유지하는 것이 좋습니다.
//
// --------------------------------------------------------------------
// [케이스별 사용법]
//
// 1) class / struct
// ------------------------------------------------------------
// - 클래스/구조체 전체를 외부에서 사용할 수 있도록 할 때는 J2LIB_API를 붙입니다.
// - vtable, 생성자, 멤버 함수 심볼이 export/import 되어야 하기 때문입니다.
//
// 예시)
//     class J2LIB_API Calculator {
//     public:
//         int add(int a, int b) const;
//     };
//
//     struct J2LIB_API Point {
//         int x;
//         int y;
//     };
//
// ------------------------------------------------------------
//
// 2) enum / enum class
// ------------------------------------------------------------
// - 보통 export/import 필요 없음.
// - enum은 단순히 정수 상수 집합으로 치환되므로 별도의 심볼 export가 필요하지 않습니다.
//
// 예시)
//     enum class Color {
//         Red,
//         Green,
//         Blue
//     };
//
// ※ 단, enum 안에 정적 멤버/함수를 정의하는 특수한 경우에는 그 함수에 J2LIB_API 필요.
//
// ------------------------------------------------------------
//
// 3) 일반 함수
// ------------------------------------------------------------
// - 외부에서 호출할 함수라면 반드시 J2LIB_API를 붙입니다.
// - 내부 전용 함수는 J2LIB_HIDDEN을 붙여 숨깁니다.
//
// 예시)
//     J2LIB_API int add(int a, int b);        // 외부 공개
//     J2LIB_HIDDEN void internal_helper();    // 내부 전용
//
// ------------------------------------------------------------
//
// 4) 템플릿 함수 (헤더 인라인 정의)
// ------------------------------------------------------------
// - 일반적인 경우 export/import 필요 없음.
// - 호출하는 번역 단위에서 인스턴스화되므로, 헤더에 정의만 있으면 됩니다.
//
// 예시)
//     template <typename T>
//     bool is_equal(T lhs, T rhs) {
//         return lhs == rhs;
//     }
//
// ------------------------------------------------------------
//
// 5) 템플릿 함수 (특정 타입만 라이브러리에서 제공: 명시적 인스턴스화)
// ------------------------------------------------------------
// - 특정 타입만 DLL/so에서 제공하고 싶다면 extern template + explicit instantiation 사용.
// - 이 경우에만 J2LIB_API를 붙입니다.
//
// 헤더 예시)
//     template <typename T>
//     T clamp(T v, T lo, T hi);
//
//     extern template J2LIB_API int clamp<int>(int v, int lo, int hi);
//     extern template J2LIB_API double clamp<double>(double v, double lo, double hi);
//
// 소스 예시)
//     template <typename T>
//     T clamp(T v, T lo, T hi) {
//         if (v < lo) return lo;
//         if (v > hi) return hi;
//         return v;
//     }
//
//     template J2LIB_API int clamp<int>(int v, int lo, int hi);
//     template J2LIB_API double clamp<double>(double v, double lo, double hi);
//
// ------------------------------------------------------------
//
// [정리]
// - class / struct  → J2LIB_API 필요
// - enum / enum class → 보통 불필요
// - 일반 함수 → J2LIB_API 필요
// - 템플릿 함수 (헤더 인라인) → 불필요
// - 템플릿 함수 (명시적 인스턴스화) → J2LIB_API 필요
//
// ========================================================================

// Static/Shared Separation: Empty export/visibility macros during static build.
//  NOTE: Public API: J2LIB_API is set for static/sharing.
#if defined(J2LIB_STATIC)
#define J2LIB_API
#define J2LIB_HIDDEN
#elif defined(_WIN32) || defined(__CYGWIN__)
#if defined(J2LIB_BUILDING_DLL)
#define J2LIB_API __declspec(dllexport)
#else
#define J2LIB_API __declspec(dllimport)
#endif
#define J2LIB_HIDDEN
#else
#if __GNUC__ >= 4
#define J2LIB_API __attribute__((visibility("default")))
#define J2LIB_HIDDEN __attribute__((visibility("hidden")))
#else
#define J2LIB_API
#define J2LIB_HIDDEN
#endif
#endif

