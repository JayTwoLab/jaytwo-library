#pragma once
#include "j2_library/export.hpp"
#include <array>

namespace j2
{
    // Library Version (manually maintained)
    // Update these values manually when releasing a new version.
    inline constexpr int VersionMajor = 0;
    inline constexpr int VersionMinor = 1;
    inline constexpr int VersionPatch = 0;

    namespace version_detail
    {
        constexpr int count_digits(int v) noexcept
        {
            if (v == 0) return 1;
            int c = 0;
            while (v != 0) { v /= 10; ++c; }
            return c;
        }

        template <std::size_t N>
        constexpr std::array<char, N> build_version_string(int a, int b, int c) noexcept
        {
            std::array<char, N> arr{};
            std::size_t pos = 0;

            auto write_int = [&](int v) {
                int d = count_digits(v);
                // compute highest pow10
                int pow10 = 1;
                for (int i = 1; i < d; ++i) pow10 *= 10;
                if (v == 0) { arr[pos++] = '0'; return; }
                while (pow10 > 0) {
                    int digit = v / pow10;
                    arr[pos++] = static_cast<char>('0' + digit);
                    v -= digit * pow10;
                    pow10 /= 10;
                }
            };

            write_int(a);
            arr[pos++] = '.';
            write_int(b);
            arr[pos++] = '.';
            write_int(c);
            arr[pos] = '\0';
            return arr;
        }
    } // namespace detail

    // compile-time array sized from digit counts + separators + null terminator
    inline constexpr std::size_t VersionStringSize =
        version_detail::count_digits(VersionMajor)
        + 1
        + version_detail::count_digits(VersionMinor)
        + 1
        + version_detail::count_digits(VersionPatch)
        + 1; // null

    inline constexpr auto VersionStringArr = version_detail::build_version_string<VersionStringSize>(
        VersionMajor, VersionMinor, VersionPatch);

    // 기존 코드와 호환되도록 const char* 인터페이스 제공
    inline constexpr const char* VersionString = VersionStringArr.data();
}
