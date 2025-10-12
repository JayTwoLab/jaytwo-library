#pragma once

#include <utility>

#include "j2_library/export.hpp"

namespace j2::core
{
    // Helper to create overloaded lambdas
    template <typename... Ts>
    struct overload : Ts... {
        using Ts::operator()...;
    };

    // Deduction guide
    template <typename... Ts>
    overload(Ts...) -> overload<Ts...>;

} // namespace j2::core
