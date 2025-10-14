#pragma once
// ---------------------------------------------------------------------------
// C++17 디렉터리 생성 유틸리티 헤더
// ---------------------------------------------------------------------------
// - 네임스페이스: j2::directory
// - std::filesystem 기반으로 디렉터리 생성 및 상태 확인을 수행합니다.
// - 내부적으로 std::error_code 기반 예외 처리를 수행합니다.
// - 한글/영문 메시지를 Language 열거형으로 선택할 수 있습니다.
// - CreateDirOptions 구조체를 통해 부모 경로 생성 여부, 권한 설정 등을
//   제어할 수 있습니다.
// - DirectoryMaker 클래스는 create_directory_tree()를 호출해 실제 경로를
//   생성하고, 결과는 CreateDirResult 구조체로 반환됩니다.
//
// ---------------------------------------------------------------------------
// 간단 예시
// ---------------------------------------------------------------------------
// #include "j2_library/directory/directory.hpp"
// using namespace j2::directory;
//
// int main() {
//     CreateDirOptions opt;
//     opt.make_parents = true;
//     DirectoryMaker maker(Language::Korean, opt);
//     CreateDirResult res = maker.create_directory_tree("logs/2025/10");
//     if (res) {
//         std::cout << "성공: " << res.message << std::endl;
//     } else {
//         std::cerr << "실패: " << res.message << std::endl;
//     }
// }
// ---------------------------------------------------------------------------

#include <filesystem>
#include <system_error>
#include <string>
#include <vector>

#include "j2_library/export.hpp"

namespace j2::directory {

    // -----------------------------------------------------------------------
    // 메시지 언어 설정
    // -----------------------------------------------------------------------
    enum class Language {
        English,  // 영어 메시지
        Korean    // 한국어 메시지
    };

    // -----------------------------------------------------------------------
    // 디렉터리 생성 결과 코드
    // -----------------------------------------------------------------------
    enum class CreatePathCode {
        Created,            // 새 디렉터리가 생성됨
        AlreadyExists,      // 이미 디렉터리가 존재함
        InvalidPath,        // 잘못된 경로 (빈 문자열 등)
        NotDirectory,       // 경로 중 일부가 디렉터리가 아님
        PermissionDenied,   // 권한 부족
        SymlinkBlocked,     // 심볼릭 링크 차단됨
        ReadOnlyFilesystem, // 읽기 전용 파일 시스템
        NameTooLong,        // 파일 이름이 너무 김
        NoSuchParent,       // 부모 디렉터리가 존재하지 않음
        DiskFull,           // 디스크 공간 부족
        Interrupted,        // 호출이 중단됨
        UnknownError        // 알 수 없는 오류
    };

    // -----------------------------------------------------------------------
    // 권한 설정 모드
    // -----------------------------------------------------------------------
    enum class PermMode {
        Add,    // 기존 권한에 추가
        Remove, // 기존 권한에서 제거
        Exact   // 정확히 지정한 권한으로 설정
    };

    // -----------------------------------------------------------------------
    // 디렉터리 생성 옵션 구조체
    // -----------------------------------------------------------------------
    struct J2LIB_API CreateDirOptions {
        bool make_parents = true;       // 상위 디렉터리 자동 생성 여부
        bool succeed_if_exists = true;  // 이미 존재 시 성공으로 간주 여부
        bool follow_symlinks = false;   // 심볼릭 링크 따라갈지 여부
        bool set_permissions = false;   // 생성 후 권한 설정 여부

        // 기본 권한: 소유자 전체, 그룹 읽기/실행, 기타 읽기/실행
        std::filesystem::perms perms_mask =
            std::filesystem::perms::owner_all |
            std::filesystem::perms::group_read |
            std::filesystem::perms::group_exec |
            std::filesystem::perms::others_read |
            std::filesystem::perms::others_exec;

        PermMode perm_mode = PermMode::Exact; // 권한 설정 모드
    };

    // -----------------------------------------------------------------------
    // 디렉터리 생성 결과 구조체
    // -----------------------------------------------------------------------
    struct J2LIB_API CreateDirResult {
        CreatePathCode code = CreatePathCode::UnknownError; // 결과 코드
        std::error_code ec;                                // 에러 코드
        std::string message;                               // 결과 메시지
        std::filesystem::path requested;                   // 요청한 경로
        std::vector<std::filesystem::path> created;        // 실제 생성된 경로 목록
        bool success = false;                              // 최종 성공 여부

        // bool 변환 연산자: 성공 여부 반환
        explicit operator bool() const { return success; }
    };

    // -----------------------------------------------------------------------
    // 디렉터리 생성기 클래스
    // -----------------------------------------------------------------------
    class J2LIB_API DirectoryMaker {
    public:
        // 생성자
        // - 언어(Language)와 옵션(CreateDirOptions)을 설정합니다.
        // - 기본값: Language::Korean, 부모 자동 생성 옵션 활성화
        //
        // 예시:
        // DirectoryMaker maker(Language::English, {});
        explicit DirectoryMaker(Language lang = Language::Korean,
            CreateDirOptions opt = {}) noexcept;

        // 현재 언어 설정을 반환합니다.
        // 예시:
        // Language lang = maker.language();
        Language language() const noexcept { return lang_; }

        // 언어를 변경합니다.
        // 예시:
        // maker.set_language(Language::English);
        void set_language(Language lang) noexcept { lang_ = lang; }

        // 현재 옵션을 반환합니다.
        // 예시:
        // const CreateDirOptions& opt = maker.options();
        const CreateDirOptions& options() const noexcept { return opt_; }

        // 옵션을 변경합니다.
        // 예시:
        // CreateDirOptions opt; opt.make_parents = false;
        // maker.set_options(opt);
        void set_options(const CreateDirOptions& opt) noexcept { opt_ = opt; }

        // 지정한 경로의 디렉터리를 생성합니다.
        // 옵션에 따라 부모 경로 생성, 권한 설정 등이 적용됩니다.
        //
        // 예시:
        // CreateDirResult res = maker.create_directory_tree("data/output");
        // if (res) std::cout << "성공" << std::endl;
        CreateDirResult create_directory_tree(
            const std::filesystem::path& target) const noexcept;

        // CreatePathCode를 문자열로 변환합니다.
        // 예시:
        // const char* s = DirectoryMaker::to_string(CreatePathCode::Created);
        static const char* to_string(CreatePathCode c) noexcept;

    private:
        // std::error_code를 CreatePathCode로 매핑
        static CreatePathCode map_errc_(const std::error_code& ec) noexcept;

        // 안전하게 파일 상태를 조회
        std::filesystem::file_status safe_status_(const std::filesystem::path& p,
            bool follow_symlinks,
            std::error_code& ec) const noexcept;

        // 권한 설정 시도 (옵션에 따라 적용)
        void try_set_perms_(const std::filesystem::path& p) const noexcept;

        // 메시지 생성 함수들 (언어 설정에 따라 다르게 출력)
        std::string msg_invalid_path_() const;
        std::string msg_already_exists_() const;
        std::string msg_already_exists_error_() const;
        std::string msg_symlink_blocked_() const;
        std::string msg_not_directory_(const std::filesystem::path& p) const;
        std::string msg_state_fail_(const std::filesystem::path& p,
            const std::error_code& ec) const;
        std::string msg_no_parent_(const std::filesystem::path& p) const;
        std::string msg_create_fail_(const std::filesystem::path& p,
            const std::error_code& ec) const;
        std::string msg_created_ok_() const;
        std::string msg_nothing_new_() const;
        std::string msg_fs_exception_(const std::string& what,
            const std::error_code& ec) const;
        std::string msg_std_exception_(const std::string& what) const;
        std::string msg_unknown_exception_() const;

    private:
        Language lang_;        // 현재 언어 설정
        CreateDirOptions opt_; // 현재 옵션 설정
    };

} // namespace j2::directory
