#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <type_traits>
#include <limits>
#include <cmath>
#include "j2_library/export.hpp"

// 프로젝트 전역 별칭
using nj = nlohmann::json;
using njj = nlohmann::json::json_pointer;

namespace j2::json { // jj 로 축약하여 사용 가능 

    //--------------------------------------------------------------
    // 내부 헬퍼: 읽기 포인터 (예외 없음)
    //--------------------------------------------------------------
    inline const nj* get_node(const nj& j, const njj& ptr) noexcept {
        if (j.contains(ptr)) return &j.at(ptr);
        return nullptr;
    }

    //--------------------------------------------------------------
    // 내부 헬퍼: 쓰기 포인터 (경로 생성 X, 예외 없음)
    //--------------------------------------------------------------
    inline nj* get_node_mutable(nj& j, const njj& ptr) noexcept {
        if (j.contains(ptr)) return &j.at(ptr);
        return nullptr;
    }

    //--------------------------------------------------------------
    // 숫자 변환: 정수/실수 범위·정수성 검사 (예외 없음)
    //--------------------------------------------------------------
    template <class T>
    inline bool try_number_cast(const nj& node, T& out) noexcept {
        static_assert(std::is_arithmetic_v<T>, "T must be arithmetic");
        if (!node.is_number()) return false;

        if constexpr (std::is_floating_point_v<T>) {
            // nlohmann::json은 float/double 저장 시 내부 double 기반
            double v = node.get<double>();
            if (std::isnan(v) || std::isinf(v)) return false;
            out = static_cast<T>(v);
            return true;
        }
        else {
            // 정수형
            long double v = node.is_number_float()
                ? static_cast<long double>(node.get<double>())
                : static_cast<long double>(node.get<long long>());
            if (std::isnan(v) || std::isinf(v)) return false;
            if (node.is_number_float() && std::floor(v) != v) return false;
            const long double lo = static_cast<long double>(std::numeric_limits<T>::min());
            const long double hi = static_cast<long double>(std::numeric_limits<T>::max());
            if (v < lo || v > hi) return false;
            out = static_cast<T>(v);
            return true;
        }
    }

    //--------------------------------------------------------------
    // 타입별 value_or 오버로드 (예외 없음)
    //--------------------------------------------------------------

    // string
    inline std::string value_or(const nj& j, const njj& ptr, const std::string& defval) noexcept {
        if (const nj* n = get_node(j, ptr)) {
            if (n->is_string()) return n->get<std::string>();
        }
        return defval;
    }

    // bool
    inline bool value_or(const nj& j, const njj& ptr, bool defval) noexcept {
        if (const nj* n = get_node(j, ptr)) {
            if (n->is_boolean()) return n->get<bool>();
        }
        return defval;
    }

    // 수치형 (int, double, long long, ...)
    template <class T,
        std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
    inline T value_or(const nj& j, const njj& ptr, T defval) noexcept {
        if (const nj* n = get_node(j, ptr)) {
            T out{};
            if (try_number_cast<T>(*n, out)) return out;
        }
        return defval;
    }

    // 경로 문자열 버전
    inline std::string value_or_path(const nj& j, const std::string& path, const std::string& defval) noexcept {
        return value_or(j, njj(path), defval);
    }
    inline bool value_or_path(const nj& j, const std::string& path, bool defval) noexcept {
        return value_or(j, njj(path), defval);
    }
    template <class T,
        std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
    inline T value_or_path(const nj& j, const std::string& path, T defval) noexcept {
        return value_or(j, njj(path), defval);
    }

    //--------------------------------------------------------------
    // 외부 공개 API (예외 없음)
    //--------------------------------------------------------------
    bool J2LIB_API exists(const nj& j, const std::string& path) noexcept;

    std::string J2LIB_API get_string(const nj& j,
        const std::string& path,
        const std::string& defval) noexcept;

    int J2LIB_API get_int(const nj& j,
        const std::string& path,
        int defval) noexcept;

    bool J2LIB_API get_bool(const nj& j,
        const std::string& path,
        bool defval) noexcept;

    double J2LIB_API get_double(const nj& j,
        const std::string& path,
        double defval) noexcept;

} // namespace j2::json

// 네임스페이스 별칭
namespace jj = j2::json;
