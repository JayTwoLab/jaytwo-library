#pragma once
// 문자열 콘솔 인코딩 변환 유틸 (헤더)
// - 내부 문자열은 UTF-8 로 가정
// - Windows: UTF-8 → EUC-KR(51949), 실패 시 CP949(949) 폴백
// - Linux/macOS: 변환 없이 UTF-8 반환
// - 네임스페이스: j2::core

#include <string>

#include "j2_library/export.hpp"

namespace j2::string {

    /// UTF-8 문자열을 콘솔에 적합한 인코딩으로 변환하여 반환합니다.
    /// - Windows: EUC-KR(51949) → 실패 시 CP949(949)
    /// - Linux/macOS: 입력 그대로(UTF-8) 반환
    /// 주의: 반환값은 변환 결과 바이트열이므로, std::cout 등에 그대로 출력하십시오.
    J2LIB_API std::string to_console_encoding(const std::string& utf8_string);

} // namespace j2::core
