#include "j2_library/file/executable_name.hpp"

namespace j2::file {

    // std::string 버전 (에러코드)
    std::string executable_name(
        std::error_code& ec,
        NameType type,
        const ExecPathOptions& opt) {

        auto p = executable_path(ec, opt);
        if (ec) return {};
        if (type == NameType::Stem) {
#if defined(_WIN32)
            return p.stem().u8string();
#else
            return p.stem().string();
#endif
        }
#if defined(_WIN32)
        return p.filename().u8string();
#else
        return p.filename().string();
#endif
    }

    // std::string 버전 (무-예외)
    std::string executable_name(
        NameType type,
        const ExecPathOptions& opt) {

        std::error_code ec;
        auto s = executable_name(ec, type, opt);
        // 더 이상 throw 하지 않음. 실패 시 빈 문자열 반환.
        return ec ? std::string{} : s;
    }

    // std::wstring 버전 (에러코드)
    std::wstring executable_name_w(
        std::error_code& ec,
        NameType type,
        const ExecPathOptions& opt) {

        auto p = executable_path(ec, opt);
        if (ec) return {};
        if (type == NameType::Stem) {
            return p.stem().wstring();
        }
        return p.filename().wstring();
    }

    // std::wstring 버전 (무-예외)
    std::wstring executable_name_w(
        NameType type,
        const ExecPathOptions& opt) {

        std::error_code ec;
        auto s = executable_name_w(ec, type, opt);
        // 더 이상 throw 하지 않음. 실패 시 빈 문자열 반환.
        return ec ? std::wstring{} : s;
    }

} // namespace j2::file
