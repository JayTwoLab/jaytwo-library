#pragma once
#include <string>

#include "j2_library/export.hpp"
#include "j2_library/file/executable_path.hpp"

namespace j2::file {

    // 실행 파일 이름 반환 형식
    enum class NameType {
        Filename, // 확장자 포함 (Hello.exe)
        Stem      // 확장자 제외 (Hello)
    };

    // 실행 파일 이름 반환. 예: Hello.exe 또는 Hello
    J2LIB_API std::string executable_name(
        std::error_code& ec,
        NameType type = NameType::Filename,
        const ExecPathOptions& opt = {});

    // 예외를 던지지 않고, 실패 시 빈 문자열 반환
    J2LIB_API std::string executable_name(
        NameType type = NameType::Filename,
        const ExecPathOptions& opt = {});

    // 실행 파일 이름 반환. 예: L"Hello.exe" 또는 L"Hello" 
    J2LIB_API std::wstring executable_name_w(
        std::error_code& ec,
        NameType type = NameType::Filename,
        const ExecPathOptions& opt = {});

    // 예외를 던지지 않고, 실패 시 빈 문자열 반환
    J2LIB_API std::wstring executable_name_w(
        NameType type = NameType::Filename,
        const ExecPathOptions& opt = {});

} // namespace j2::file
