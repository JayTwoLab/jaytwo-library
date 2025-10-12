#pragma once
// C++17 경로 생성 유틸리티 (헤더)
// - 네임스페이스: j2::directory
// - std::filesystem 직접 사용
// - 함수 내부에서만 예외 처리
// - 한/영 메시지 전환 지원(Language)
// - 클래스 기반 설정/정책 보관

#include <filesystem>
#include <system_error>
#include <string>
#include <vector>

#include "j2_library/export.hpp"

namespace j2::directory {

    // 메시지 언어 설정
    enum class Language {
        English,
        Korean
    };

    // 결과 코드
    enum class CreatePathCode {
        Created,            // 새로 생성됨
        AlreadyExists,      // 이미 존재함
        InvalidPath,        // 빈 경로 등
        NotDirectory,       // 중간 경로가 디렉터리가 아님
        PermissionDenied,   // 권한 부족
        SymlinkBlocked,     // 심볼릭 링크 차단됨
        ReadOnlyFilesystem, // 읽기 전용 파일시스템
        NameTooLong,        // 이름이 너무 김
        NoSuchParent,       // 부모 경로 없음
        DiskFull,           // 공간 부족
        Interrupted,        // 호출 중단
        UnknownError        // 알 수 없는 오류
    };

    // 권한 설정 방식
    enum class PermMode { Add, Remove, Exact };

    // 옵션
    struct J2LIB_API CreateDirOptions {
        bool make_parents = true;        // 상위(부모) 경로까지 자동 생성 여부
        bool succeed_if_exists = true;   // 이미 존재하면 성공으로 간주 여부
        bool follow_symlinks = false;    // 심볼릭 링크 따라갈지 여부
        bool set_permissions = false;    // 생성 직후 권한 설정 시도 여부
        std::filesystem::perms perms_mask =
            std::filesystem::perms::owner_all |
            std::filesystem::perms::group_read | std::filesystem::perms::group_exec |
            std::filesystem::perms::others_read | std::filesystem::perms::others_exec;
        PermMode perm_mode = PermMode::Exact;
    };

    // 결과 객체
    struct J2LIB_API CreateDirResult {
        CreatePathCode code = CreatePathCode::UnknownError;
        std::error_code ec;
        std::string message;
        std::filesystem::path requested;
        std::vector<std::filesystem::path> created;
        bool success = false;  // ← 추가: 최종 성공 여부(AlreadyExists 처리 정책 반영)

        explicit operator bool() const { return success; }
    };

    // 클래스
    class J2LIB_API DirectoryMaker {
    public:
        explicit DirectoryMaker(Language lang = Language::Korean,
            CreateDirOptions opt = {}) noexcept;

        Language language() const noexcept { return lang_; }
        void set_language(Language lang) noexcept { lang_ = lang; }
        const CreateDirOptions& options() const noexcept { return opt_; }
        void set_options(const CreateDirOptions& opt) noexcept { opt_ = opt; }

        CreateDirResult create_directory_tree(const std::filesystem::path& target) const noexcept;

        static const char* to_string(CreatePathCode c) noexcept;

    private:
        static CreatePathCode map_errc_(const std::error_code& ec) noexcept;
        std::filesystem::file_status safe_status_(const std::filesystem::path& p,
            bool follow_symlinks,
            std::error_code& ec) const noexcept;
        void try_set_perms_(const std::filesystem::path& p) const noexcept;

        std::string msg_invalid_path_() const;
        std::string msg_already_exists_() const;
        std::string msg_already_exists_error_() const; // succeed_if_exists=false인 경우 메시지
        std::string msg_symlink_blocked_() const;
        std::string msg_not_directory_(const std::filesystem::path& p) const;
        std::string msg_state_fail_(const std::filesystem::path& p, const std::error_code& ec) const;
        std::string msg_no_parent_(const std::filesystem::path& p) const;
        std::string msg_create_fail_(const std::filesystem::path& p, const std::error_code& ec) const;
        std::string msg_created_ok_() const;
        std::string msg_nothing_new_() const;
        std::string msg_fs_exception_(const std::string& what, const std::error_code& ec) const;
        std::string msg_std_exception_(const std::string& what) const;
        std::string msg_unknown_exception_() const;

    private:
        Language lang_;
        CreateDirOptions opt_;
    };

} // namespace j2::directory
