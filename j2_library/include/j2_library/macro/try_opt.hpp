#pragma once
#include <optional>
#include <utility>

#define J2_PP_CAT2(a,b) a##b
#define J2_PP_CAT(a,b)  J2_PP_CAT2(a,b)
#define J2_UNIQUE_NAME(prefix) J2_PP_CAT(prefix, __LINE__)

// 내부 구현
#define J2_TRY_OPT_IMPL(VAR, EXPR, TMP) \
    auto TMP = (EXPR);                   \
    if (!TMP) return {};                 \
    auto VAR = std::move(*TMP)

#define J2_TRY_OPT_NO_BIND_IMPL(EXPR, TMP) \
    { auto TMP = (EXPR); if (!TMP) return {}; }

#define J2_TRY_OPT_VOID_IMPL(EXPR, TMP) \
    { auto TMP = (EXPR); if (!TMP) return; }

#define J2_TRY_OPT_BIND_VOID_IMPL(VAR, EXPR, TMP) \
    auto TMP = (EXPR);                             \
    if (!TMP) return;                              \
    auto VAR = std::move(*TMP)

// 공개 매크로
#define TRY_OPT(VAR, EXPR)               J2_TRY_OPT_IMPL(VAR, EXPR, J2_UNIQUE_NAME(_opt_))
#define TRY_OPT_NO_BIND(EXPR)            J2_TRY_OPT_NO_BIND_IMPL(EXPR, J2_UNIQUE_NAME(_opt_))
#define TRY_OPT_VOID(EXPR)               J2_TRY_OPT_VOID_IMPL(EXPR, J2_UNIQUE_NAME(_opt_))
#define TRY_OPT_BIND_VOID(VAR, EXPR)     J2_TRY_OPT_BIND_VOID_IMPL(VAR, EXPR, J2_UNIQUE_NAME(_opt_))

// (선택) 포인터 전용
#define J2_TRY_PTR_IMPL(VAR, EXPR, TMP) \
    auto TMP = (EXPR);                   \
    if (!(TMP)) return {};               \
    auto VAR = (TMP)

#define J2_TRY_PTR_NO_BIND_IMPL(EXPR, TMP) \
    { auto TMP = (EXPR); if (!(TMP)) return {}; }

#define J2_TRY_PTR_VOID_IMPL(EXPR, TMP) \
    { auto TMP = (EXPR); if (!(TMP)) return; }

#define J2_TRY_PTR_BIND_VOID_IMPL(VAR, EXPR, TMP) \
    auto TMP = (EXPR);                             \
    if (!(TMP)) return;                            \
    auto VAR = (TMP)

#define TRY_PTR(VAR, EXPR)               J2_TRY_PTR_IMPL(VAR, EXPR, J2_UNIQUE_NAME(_ptr_))
#define TRY_PTR_NO_BIND(EXPR)            J2_TRY_PTR_NO_BIND_IMPL(EXPR, J2_UNIQUE_NAME(_ptr_))
#define TRY_PTR_VOID(EXPR)               J2_TRY_PTR_VOID_IMPL(EXPR, J2_UNIQUE_NAME(_ptr_))
#define TRY_PTR_BIND_VOID(VAR, EXPR)     J2_TRY_PTR_BIND_VOID_IMPL(VAR, EXPR, J2_UNIQUE_NAME(_ptr_))
