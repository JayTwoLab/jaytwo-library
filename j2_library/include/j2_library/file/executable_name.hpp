#pragma once
#include <string>

#include "j2_library/export.hpp"
#include "j2_library/file/executable_path.hpp"

namespace j2::file {

    // 실행 파일 이름 반환 형식
    enum class name_type {
        filename, // 확장자 포함 (Hello.exe)
        stem      // 확장자 제외 (Hello)
    };

    // 실행 파일 이름 반환. 예: Hello.exe 또는 Hello
    J2LIB_API std::string executable_name(
        std::error_code& ec,
        name_type type = name_type::filename,
        const exec_path_options& opt = {});

    // 예외를 던지지 않고, 실패 시 빈 문자열 반환
    J2LIB_API std::string executable_name(
        name_type type = name_type::filename,
        const exec_path_options& opt = {});

    // 실행 파일 이름 반환. 예: L"Hello.exe" 또는 L"Hello" 
    J2LIB_API std::wstring executable_name_w(
        std::error_code& ec,
        name_type type = name_type::filename,
        const exec_path_options& opt = {});

    // 예외를 던지지 않고, 실패 시 빈 문자열 반환
    J2LIB_API std::wstring executable_name_w(
        name_type type = name_type::filename,
        const exec_path_options& opt = {});

} // namespace j2::file
