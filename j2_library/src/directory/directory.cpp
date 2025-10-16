
#include "j2_library/directory/directory.hpp"
#include "j2_library/string/to_console_encoding.hpp"

namespace j2::directory {

    using std::errc;

    DirectoryMaker::DirectoryMaker(Language lang, CreateDirOptions opt) noexcept
        : lang_(lang), opt_(opt) {
    }

    CreatePathCode DirectoryMaker::map_errc_(const std::error_code& ec) noexcept {
        switch (static_cast<errc>(ec.value())) {
        case errc::permission_denied:         return CreatePathCode::PermissionDenied;
        case errc::read_only_file_system:     return CreatePathCode::ReadOnlyFilesystem;
        case errc::file_exists:               return CreatePathCode::AlreadyExists;
        case errc::not_a_directory:           return CreatePathCode::NotDirectory;
        case errc::no_such_file_or_directory: return CreatePathCode::NoSuchParent;
        case errc::filename_too_long:         return CreatePathCode::NameTooLong;
        case errc::no_space_on_device:        return CreatePathCode::DiskFull;
        case errc::interrupted:               return CreatePathCode::Interrupted;
        default:                              return CreatePathCode::UnknownError;
        }
    }

    std::filesystem::file_status
        DirectoryMaker::safe_status_(const std::filesystem::path& p,
            bool follow_symlinks,
            std::error_code& ec) const noexcept {
        return follow_symlinks ? std::filesystem::status(p, ec)
            : std::filesystem::symlink_status(p, ec);
    }

    void DirectoryMaker::try_set_perms_(const std::filesystem::path& p) const noexcept {
        if (!opt_.set_permissions) return;
        std::error_code ign;
        switch (opt_.perm_mode) {
        case PermMode::Exact:
            std::filesystem::permissions(p, opt_.perms_mask, ign);
            break;
        case PermMode::Add:
            std::filesystem::permissions(p, opt_.perms_mask, std::filesystem::perm_options::add, ign);
            break;
        case PermMode::Remove:
            std::filesystem::permissions(p, opt_.perms_mask, std::filesystem::perm_options::remove, ign);
            break;
        }
    }

    CreateDirResult DirectoryMaker::create_directory_tree(const std::filesystem::path& target) const noexcept {
        CreateDirResult out;
        out.requested = target;
        out.success = false;

        try {
            // 0) 입력 검증
            if (target.empty()) {
                out.code = CreatePathCode::InvalidPath;
                out.message = msg_invalid_path_();
                return out;
            }

            // 1) 최종 경로 상태 확인
            {
                std::error_code ec;
                auto st = safe_status_(target, opt_.follow_symlinks, ec);
                if (!ec) {
                    if (std::filesystem::is_directory(st)) {
                        out.code = CreatePathCode::AlreadyExists;
                        out.message = opt_.succeed_if_exists ? msg_already_exists_()
                            : msg_already_exists_error_();
                        out.success = opt_.succeed_if_exists;
                        return out;
                    }
                    if (std::filesystem::is_symlink(st) && !opt_.follow_symlinks) {
                        out.code = CreatePathCode::SymlinkBlocked;
                        out.message = msg_symlink_blocked_();
                        return out;
                    }
                    out.code = CreatePathCode::NotDirectory;
                    out.message = msg_not_directory_(target);
                    return out;
                }
                else if (ec && ec != errc::no_such_file_or_directory) {
                    out.code = map_errc_(ec);
                    out.ec = ec;
                    out.message = msg_state_fail_(target, ec);
                    return out;
                }
            }

            // 2) make_parents=false → 부모 확인 후 최종 경로만 생성
            if (!opt_.make_parents) {
                std::filesystem::path parent = target.parent_path();
                std::error_code pec;
                if (!parent.empty() && !std::filesystem::exists(parent, pec)) {
                    out.code = CreatePathCode::NoSuchParent;
                    out.ec = pec;
                    out.message = msg_no_parent_(parent);
                    return out;
                }

                std::error_code mec;
                bool made = std::filesystem::create_directory(target, mec);
                if (!made) {
                    if (!mec) {
                        std::error_code rec;
                        if (std::filesystem::is_directory(safe_status_(target, opt_.follow_symlinks, rec))) {
                            out.code = CreatePathCode::AlreadyExists;
                            out.message = opt_.succeed_if_exists ? msg_already_exists_()
                                : msg_already_exists_error_();
                            out.success = opt_.succeed_if_exists;
                            return out;
                        }
                        out.code = CreatePathCode::NotDirectory;
                        out.message = msg_not_directory_(target);
                        return out;
                    }
                    else {
                        if (mec == errc::file_exists) {
                            std::error_code rec;
                            if (std::filesystem::is_directory(safe_status_(target, opt_.follow_symlinks, rec))) {
                                out.code = CreatePathCode::AlreadyExists;
                                out.message = opt_.succeed_if_exists ? msg_already_exists_()
                                    : msg_already_exists_error_();
                                out.success = opt_.succeed_if_exists;
                                return out;
                            }
                            out.code = CreatePathCode::NotDirectory;
                            out.ec = mec;
                            out.message = msg_not_directory_(target);
                            return out;
                        }
                        out.code = map_errc_(mec);
                        out.ec = mec;
                        out.message = msg_create_fail_(target, mec);
                        return out;
                    }
                }

                try_set_perms_(target);
                out.created.push_back(target);
                out.code = CreatePathCode::Created;
                out.message = msg_created_ok_();
                out.success = true;
                return out;
            }

            // 3) make_parents=true → 상위부터 생성
            std::filesystem::path accum;
#if defined(_WIN32)
            if (target.has_root_name())      accum /= target.root_name();
            if (target.has_root_directory()) accum /= target.root_directory();
#else
            if (target.has_root_directory()) accum /= target.root_directory();
#endif

            for (const auto& part : target.relative_path()) {
                accum /= part;

                std::error_code ec;
                auto st = safe_status_(accum, opt_.follow_symlinks, ec);

                if (!ec && std::filesystem::exists(st)) {
                    if (std::filesystem::is_directory(st)) continue;
                    if (std::filesystem::is_symlink(st) && !opt_.follow_symlinks) {
                        out.code = CreatePathCode::SymlinkBlocked;
                        out.message = msg_symlink_blocked_();
                        return out;
                    }
                    out.code = CreatePathCode::NotDirectory;
                    out.message = msg_not_directory_(accum);
                    return out;
                }

                if (ec && ec != errc::no_such_file_or_directory) {
                    out.code = map_errc_(ec);
                    out.ec = ec;
                    out.message = msg_state_fail_(accum, ec);
                    return out;
                }

                std::error_code mec;
                bool made = std::filesystem::create_directory(accum, mec);
                if (!made) {
                    if (!mec) {
                        std::error_code rec;
                        if (std::filesystem::is_directory(safe_status_(accum, opt_.follow_symlinks, rec))) continue;
                        out.code = CreatePathCode::NotDirectory;
                        out.message = msg_not_directory_(accum);
                        return out;
                    }
                    else {
                        if (mec == errc::file_exists) {
                            std::error_code rec;
                            if (std::filesystem::is_directory(safe_status_(accum, opt_.follow_symlinks, rec))) continue;
                            out.code = CreatePathCode::NotDirectory;
                            out.ec = mec;
                            out.message = msg_not_directory_(accum);
                            return out;
                        }
                        out.code = map_errc_(mec);
                        out.ec = mec;
                        out.message = msg_create_fail_(accum, mec);
                        return out;
                    }
                }

                try_set_perms_(accum);
                out.created.push_back(accum);
            }

            if (out.created.empty()) {
                out.code = CreatePathCode::AlreadyExists;
                out.message = opt_.succeed_if_exists ? msg_nothing_new_()
                    : msg_already_exists_error_();
                out.success = opt_.succeed_if_exists;
            }
            else {
                out.code = CreatePathCode::Created;
                out.message = msg_created_ok_();
                out.success = true;
            }
            return out;
        }
        catch (const std::filesystem::filesystem_error& ex) {
            out.ec = ex.code();
            out.code = map_errc_(out.ec);
            out.message = msg_fs_exception_(ex.what(), out.ec);
            return out;
        }
        catch (const std::exception& ex) {
            out.code = CreatePathCode::UnknownError;
            out.message = msg_std_exception_(ex.what());
            return out;
        }
        catch (...) {
            out.code = CreatePathCode::UnknownError;
            out.message = msg_unknown_exception_();
            return out;
        }
    }

    const char* DirectoryMaker::to_string(CreatePathCode c) noexcept {
        switch (c) {
        case CreatePathCode::Created:            return "Created";
        case CreatePathCode::AlreadyExists:      return "AlreadyExists";
        case CreatePathCode::InvalidPath:        return "InvalidPath";
        case CreatePathCode::NotDirectory:       return "NotDirectory";
        case CreatePathCode::PermissionDenied:   return "PermissionDenied";
        case CreatePathCode::SymlinkBlocked:     return "SymlinkBlocked";
        case CreatePathCode::ReadOnlyFilesystem: return "ReadOnlyFilesystem";
        case CreatePathCode::NameTooLong:        return "NameTooLong";
        case CreatePathCode::NoSuchParent:       return "NoSuchParent";
        case CreatePathCode::DiskFull:           return "DiskFull";
        case CreatePathCode::Interrupted:        return "Interrupted";
        case CreatePathCode::UnknownError:       return "UnknownError";
        default:                                  return "Unknown";
        }
    }

    // ---------------------------
    // 메시지 (한/영, 한글은 변환 적용)
    // ---------------------------
    std::string DirectoryMaker::msg_invalid_path_() const {
        return lang_ == Language::English
            ? "Invalid path: empty or malformed."
            : j2::string::to_console_encoding("유효하지 않은 경로입니다(빈 경로 또는 잘못된 형식).");
    }
    std::string DirectoryMaker::msg_already_exists_() const {
        return lang_ == Language::English
            ? "Directory already exists."
            : j2::string::to_console_encoding("경로가 이미 디렉터리로 존재합니다.");
    }
    std::string DirectoryMaker::msg_already_exists_error_() const {
        return lang_ == Language::English
            ? "Directory already exists (policy: treat as error)."
            : j2::string::to_console_encoding("경로가 이미 존재합니다(정책상 오류로 간주).");
    }
    std::string DirectoryMaker::msg_symlink_blocked_() const {
        return lang_ == Language::English
            ? "Operation blocked by symlink."
            : j2::string::to_console_encoding("심볼릭 링크가 존재하며 차단되었습니다.");
    }
    std::string DirectoryMaker::msg_not_directory_(const std::filesystem::path& p) const {
        return lang_ == Language::English
            ? "A non-directory entry exists: " + p.string()
            : j2::string::to_console_encoding("디렉터리가 아닌 항목이 존재합니다: " + p.string());
    }
    std::string DirectoryMaker::msg_state_fail_(const std::filesystem::path& p, const std::error_code& ec) const {
        return lang_ == Language::English
            ? "Failed to query status: " + p.string() + " : " + ec.message()
            : j2::string::to_console_encoding("경로 상태 확인 실패: " + p.string() + " : " + ec.message());
    }
    std::string DirectoryMaker::msg_no_parent_(const std::filesystem::path& p) const {
        return lang_ == Language::English
            ? "Parent does not exist: " + p.string()
            : j2::string::to_console_encoding("부모 경로가 존재하지 않습니다: " + p.string());
    }
    std::string DirectoryMaker::msg_create_fail_(const std::filesystem::path& p, const std::error_code& ec) const {
        return lang_ == Language::English
            ? "Failed to create: " + p.string() + " : " + ec.message()
            : j2::string::to_console_encoding("디렉터리 생성 실패: " + p.string() + " : " + ec.message());
    }
    std::string DirectoryMaker::msg_created_ok_() const {
        return lang_ == Language::English
            ? "Directory tree created."
            : j2::string::to_console_encoding("디렉터리 트리를 성공적으로 생성했습니다.");
    }
    std::string DirectoryMaker::msg_nothing_new_() const {
        return lang_ == Language::English
            ? "Nothing created; already exists."
            : j2::string::to_console_encoding("새로 생성된 디렉터리가 없습니다(이미 존재).");
    }
    std::string DirectoryMaker::msg_fs_exception_(const std::string& what, const std::error_code& ec) const {
        return lang_ == Language::English
            ? "filesystem_error: " + what + " : " + ec.message()
            : j2::string::to_console_encoding("filesystem_error: " + what + " : " + ec.message());
    }
    std::string DirectoryMaker::msg_std_exception_(const std::string& what) const {
        return lang_ == Language::English
            ? "std::exception: " + what
            : j2::string::to_console_encoding("std::exception: " + what);
    }
    std::string DirectoryMaker::msg_unknown_exception_() const {
        return lang_ == Language::English
            ? "Unknown exception."
            : j2::string::to_console_encoding("알 수 없는 예외가 발생했습니다.");
    }

} // namespace j2::directory
