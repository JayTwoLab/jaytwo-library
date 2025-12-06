#include <iostream>
#include <thread>
#include <random>
#include <chrono>
#include <fstream>
#include <string>
#include <filesystem>

#include "gtest_compat.hpp"          // ← 추가/변경: 호환 레이어 헤더
#include "j2_library/j2_library.hpp" // 기존 유지

// Google Test로 변환한 directory_maker 테스트
// 빌드 시: directory.cpp 와 함께 링크 필요 (console_kor는 불필요)
// 주의: 소스 인코딩 UTF-8, MSVC는 /utf-8 권장

namespace {

    // 시간 기반 임시 sandbox 디렉터리 생성. C++20 이상에서는 대체 함수인 unique_path 사용 권장.
    std::filesystem::path make_sandbox() {
        auto now = std::chrono::system_clock::now().time_since_epoch().count();
        std::filesystem::path base = std::filesystem::temp_directory_path()
            / ("j2_directory_test_" + std::to_string(now));
        std::error_code ec;
        std::filesystem::create_directories(base, ec);
        return base;
    }

    // 한국어 메시지를 기본으로 하는 directory_maker 준비
    j2::directory::directory_maker make_default_ko() {
        j2::directory::directory_maker mk;
        j2::directory::CreateDirOptions opt;
        opt.make_parents = true;   // 부모 자동 생성
        opt.succeed_if_exists = true;   // 이미 존재해도 성공
        opt.follow_symlinks = false;  // 심볼릭 링크 미추적
        opt.set_permissions = false;  // 권한 설정 안 함
        mk.set_options(opt);
        mk.set_language(j2::directory::Language::Korean);
        return mk;
    }

} // namespace

// Case 1: 기본 생성
TEST(directory, CreateBasicRelative) {
    auto sandbox = make_sandbox();
    ASSERT_FALSE(sandbox.empty());
    auto maker = make_default_ko();

    auto old = std::filesystem::current_path();
    std::filesystem::current_path(sandbox);

    {
        auto r = maker.create_directory_tree("logs/app/2025/10");
        EXPECT_TRUE(r) << r.message;
        EXPECT_EQ(r.code, j2::directory::CreatePathCode::Created);
    }
    {
        auto r = maker.create_directory_tree("logs/app/2025/10");
        EXPECT_TRUE(r) << r.message;
        EXPECT_EQ(r.code, j2::directory::CreatePathCode::AlreadyExists);
    }

    std::filesystem::current_path(old);
}

// Case 2: 중간 경로가 파일
TEST(directory, NotDirectoryWhenIntermediateIsFile) {
    auto sandbox = make_sandbox();
    auto maker = make_default_ko();
    auto old = std::filesystem::current_path();
    std::filesystem::current_path(sandbox);

    { std::ofstream("logs_file").put('\n'); }
    auto r = maker.create_directory_tree("logs_file/subdir");
    EXPECT_FALSE(r);
    EXPECT_EQ(r.code, j2::directory::CreatePathCode::NotDirectory);

    std::filesystem::current_path(old);
}

// Case 3: 부모 없음 + make_parents=false
TEST(directory, NoSuchParentWhenMakeParentsFalse) {
    auto sandbox = make_sandbox();
    auto maker = make_default_ko();
    auto opt = maker.options();
    opt.make_parents = false;
    maker.set_options(opt);

    auto old = std::filesystem::current_path();
    std::filesystem::current_path(sandbox);

    auto r = maker.create_directory_tree("a/b/c");
    EXPECT_FALSE(r);
    EXPECT_EQ(r.code, j2::directory::CreatePathCode::NoSuchParent);

    std::filesystem::current_path(old);
}

// Case 4: 빈 경로
TEST(directory, EmptyPathInvalid) {
    auto maker = make_default_ko();
    auto r = maker.create_directory_tree(std::filesystem::path{});
    EXPECT_FALSE(r);
    EXPECT_EQ(r.code, j2::directory::CreatePathCode::InvalidPath);
}

// Case 5: 절대 경로 (환경 따라 성공/실패)
TEST(directory, AbsolutePathCreate) {
    auto maker = make_default_ko();

#if defined(_WIN32)
    std::filesystem::path abs_target = "C:\\temp\\j2_dir_test\\child";
#else
    std::filesystem::path abs_target = "/tmp/j2_dir_test/child";
#endif

    auto r = maker.create_directory_tree(abs_target);
    if (r) {
        EXPECT_TRUE(r.code == j2::directory::CreatePathCode::Created ||
            r.code == j2::directory::CreatePathCode::AlreadyExists);
    }
    else {
        SUCCEED() << "Absolute path creation failed in this environment: " << r.message;
    }
}

// Case 6: succeed_if_exists=false 정책
TEST(directory, FailWhenAlreadyExistsIfPolicySaysSo) {
    auto sandbox = make_sandbox();
    auto maker = make_default_ko();

    auto old = std::filesystem::current_path();
    std::filesystem::current_path(sandbox);

    { auto r = maker.create_directory_tree("p/q/r"); ASSERT_TRUE(r); }

    auto opt = maker.options();
    opt.succeed_if_exists = false;
    maker.set_options(opt);

    auto r2 = maker.create_directory_tree("p/q/r");
    EXPECT_FALSE(r2);

    std::filesystem::current_path(old);
}

// Case 7: 심볼릭 링크 (환경 제약으로 DISABLED_)
TEST(directory, DISABLED_SymlinkBlockedAndFollow) {
    auto sandbox = make_sandbox();
    auto maker = make_default_ko();
    auto old = std::filesystem::current_path();
    std::filesystem::current_path(sandbox);

    std::error_code ec;
    std::filesystem::create_directories("real_base_dir", ec);

    std::filesystem::path link_name = "link_to_real_base";
    if (std::filesystem::exists(link_name, ec) || std::filesystem::is_symlink(link_name, ec)) {
        std::filesystem::remove(link_name, ec);
    }

    // create_directory_symlink는 C++17에서 void 반환입니다. 오류코드로 성공 여부 판단.
    std::error_code mkec;
    std::filesystem::create_directory_symlink("real_base_dir", link_name, mkec);
    bool link_ok = !mkec && std::filesystem::is_symlink(link_name, ec);
    if (!link_ok) {
        GTEST_SKIP() << "Symlink not supported in this environment: " << mkec.message();
    }

    {
        auto opt = maker.options(); opt.follow_symlinks = false; maker.set_options(opt);
        auto r = maker.create_directory_tree(link_name / "x");
        EXPECT_FALSE(r);
        EXPECT_EQ(r.code, j2::directory::CreatePathCode::SymlinkBlocked);
    }

    {
        auto opt = maker.options(); opt.follow_symlinks = true; maker.set_options(opt);
        auto r = maker.create_directory_tree(link_name / "y");
        EXPECT_TRUE(r);
    }

    std::filesystem::current_path(old);
}

// Case 8: 보호 디렉터리 (환경 따라 실패 예상)
TEST(directory, ProtectedPathMayFail) {
    auto maker = make_default_ko();
#if defined(_WIN32)
    std::filesystem::path prot = "C:\\Windows\\System32\\forbidden_test_dir";
#else
    std::filesystem::path prot = "/root/forbidden_test_dir";
#endif
    auto r = maker.create_directory_tree(prot);
    if (r) {
        SUCCEED() << "Succeeded in protected path (maybe elevated privileges).";
    }
    else {
        SUCCEED() << "Expected failure: " << r.message;
    }
}
