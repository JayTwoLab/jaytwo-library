// 문자열 콘솔 인코딩 변환 유틸 (구현)
// - platform별 분기 구현
 

#include "j2_library/string/to_console_encoding.hpp"

#if defined(_WIN32)
#include <windows.h>
#endif

#include <string>

namespace j2::core {

#if defined(_WIN32)
    // 내부 헬퍼: UTF-8 → 지정 코드페이지(예: 51949, 949) 멀티바이트 변환
    static std::string utf8_to_codepage_impl(const std::string& utf8, unsigned int codepage) {
        if (utf8.empty()) return std::string{};

        // 1) UTF-8 → UTF-16
        int wlen = ::MultiByteToWideChar(CP_UTF8, 0,
            utf8.c_str(),
            static_cast<int>(utf8.size()),
            nullptr, 0);
        if (wlen <= 0) return std::string{};

        std::wstring wbuf(static_cast<size_t>(wlen), L'\0');
        if (::MultiByteToWideChar(CP_UTF8, 0,
            utf8.c_str(),
            static_cast<int>(utf8.size()),
            &wbuf[0], wlen) <= 0) {
            return std::string{};
        }

        // 2) UTF-16 → codepage 멀티바이트
        int mblen = ::WideCharToMultiByte(codepage, 0,
            wbuf.c_str(), wlen,
            nullptr, 0, nullptr, nullptr);
        if (mblen <= 0) return std::string{};

        std::string mbuf(static_cast<size_t>(mblen), '\0');
        if (::WideCharToMultiByte(codepage, 0,
            wbuf.c_str(), wlen,
            &mbuf[0], mblen, nullptr, nullptr) <= 0) {
            return std::string{};
        }
        return mbuf;
    }
#endif // _WIN32

    std::string to_console_encoding(const std::string& utf8) {
#if defined(_WIN32)
        // 1순위: EUC-KR (51949) 시도
        {
            std::string encoded = utf8_to_codepage_impl(utf8, 51949);
            if (!encoded.empty()) return encoded;
        }
    // 2순위: CP949 (949) 폴백
        {
            std::string encoded = utf8_to_codepage_impl(utf8, 949);
            if (!encoded.empty()) return encoded;
        }
        // 최종 실패: 원본 UTF-8 반환
        return utf8;
#else
        // Linux/macOS: UTF-8 그대로 사용
        return utf8;
#endif
    }

} // namespace j2::core
