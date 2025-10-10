#pragma once

#include <string>
#include <vector>
#include <cstddef>
#include <cstdint>

#include "j2_library/export.hpp" // J2LIB_API

namespace j2::core
{
    // UTF-8 <-> UTF-16
    J2LIB_API bool utf8_to_utf16(const std::string& utf8, std::u16string& out);
    J2LIB_API bool utf16_to_utf8(const std::u16string& u16, std::string& out);

    // UTF-8 <-> UTF-32
    J2LIB_API bool utf8_to_utf32(const std::string& utf8, std::u32string& out);
    J2LIB_API bool utf32_to_utf8(const std::u32string& u32, std::string& out);

    // UTF-16 <-> UTF-32
    J2LIB_API bool utf16_to_utf32(const std::u16string& u16, std::u32string& out);
    J2LIB_API bool utf32_to_utf16(const std::u32string& u32, std::u16string& out);

    // UTF-8 <-> CP949 (EUC-KR superset; Windows: CP 949)
    J2LIB_API bool utf8_to_cp949(const std::string& utf8, std::string& out_cp949);
    J2LIB_API bool cp949_to_utf8(const std::string& cp949, std::string& out_utf8);

    // UTF-16 <-> CP949
    J2LIB_API bool utf16_to_cp949(const std::u16string& u16, std::string& out_cp949);
    J2LIB_API bool cp949_to_utf16(const std::string& cp949, std::u16string& out_u16);

    // ---------- 추가: ISO-2022-KR ----------
    // UTF-8 <-> ISO-2022-KR
    J2LIB_API bool utf8_to_iso2022kr(const std::string& utf8, std::string& out_iso2022kr);
    J2LIB_API bool iso2022kr_to_utf8(const std::string& iso2022kr, std::string& out_utf8);
    // UTF-16 <-> ISO-2022-KR
    J2LIB_API bool utf16_to_iso2022kr(const std::u16string& u16, std::string& out_iso2022kr);
    J2LIB_API bool iso2022kr_to_utf16(const std::string& iso2022kr, std::u16string& out_u16);

    // ---------- 추가: JOHAB (CP 1361) ----------
    // UTF-8 <-> JOHAB
    J2LIB_API bool utf8_to_johab(const std::string& utf8, std::string& out_johab);
    J2LIB_API bool johab_to_utf8(const std::string& johab, std::string& out_utf8);
    // UTF-16 <-> JOHAB
    J2LIB_API bool utf16_to_johab(const std::u16string& u16, std::string& out_johab);
    J2LIB_API bool johab_to_utf16(const std::string& johab, std::u16string& out_u16);

    // ---------- 추가: MacKorean ----------
    // UTF-8 <-> MacKorean
    J2LIB_API bool utf8_to_mackorean(const std::string& utf8, std::string& out_mackor);
    J2LIB_API bool mackorean_to_utf8(const std::string& mackor, std::string& out_utf8);
    // UTF-16 <-> MacKorean
    J2LIB_API bool utf16_to_mackorean(const std::u16string& u16, std::string& out_mackor);
    J2LIB_API bool mackorean_to_utf16(const std::string& mackor, std::u16string& out_u16);

    // 도우미: 실패 시 예외를 던지는 버전
    J2LIB_API std::u16string utf8_to_utf16_or_throw(const std::string& utf8);
    J2LIB_API std::string    utf16_to_utf8_or_throw(const std::u16string& u16);
    J2LIB_API std::u32string utf8_to_utf32_or_throw(const std::string& utf8);
    J2LIB_API std::string    utf32_to_utf8_or_throw(const std::u32string& u32);
    J2LIB_API std::u32string utf16_to_utf32_or_throw(const std::u16string& u16);
    J2LIB_API std::u16string utf32_to_utf16_or_throw(const std::u32string& u32);
    J2LIB_API std::string    utf8_to_cp949_or_throw(const std::string& utf8);
    J2LIB_API std::string    cp949_to_utf8_or_throw(const std::string& cp949);
    J2LIB_API std::string    utf16_to_cp949_or_throw(const std::u16string& u16);
    J2LIB_API std::u16string cp949_to_utf16_or_throw(const std::string& cp949);

    // 예외 버전(추가 코덱)
    J2LIB_API std::string    utf8_to_iso2022kr_or_throw(const std::string& utf8);
    J2LIB_API std::string    iso2022kr_to_utf8_or_throw(const std::string& iso2022kr);
    J2LIB_API std::u16string iso2022kr_to_utf16_or_throw(const std::string& iso2022kr);
    J2LIB_API std::string    utf16_to_iso2022kr_or_throw(const std::u16string& u16);

    J2LIB_API std::string    utf8_to_johab_or_throw(const std::string& utf8);
    J2LIB_API std::string    johab_to_utf8_or_throw(const std::string& johab);
    J2LIB_API std::u16string johab_to_utf16_or_throw(const std::string& johab);
    J2LIB_API std::string    utf16_to_johab_or_throw(const std::u16string& u16);

    J2LIB_API std::string    utf8_to_mackorean_or_throw(const std::string& utf8);
    J2LIB_API std::string    mackorean_to_utf8_or_throw(const std::string& mackor);
    J2LIB_API std::u16string mackorean_to_utf16_or_throw(const std::string& mackor);
    J2LIB_API std::string    utf16_to_mackorean_or_throw(const std::u16string& u16);

} // namespace j2::core
