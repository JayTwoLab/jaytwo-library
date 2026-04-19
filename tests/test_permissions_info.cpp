#include <fstream>
#include <filesystem>
#include <string>

#include "gtest_compat.hpp"
#include "j2_library/j2_library.hpp"

namespace {

    // 시간 기반 임시 파일 경로 생성
    std::filesystem::path make_temp_file_path() {
        auto now = std::chrono::system_clock::now().time_since_epoch().count();
        return std::filesystem::temp_directory_path() / ("j2_perm_test_" + std::to_string(now));
    }

} // namespace

TEST(permissions, NonexistentPath) {
    auto p = make_temp_file_path();
    // 확실히 존재하지 않도록 한 이름 사용
    std::filesystem::remove(p);
    j2::file::permissions_info info(p.string());
    EXPECT_FALSE(info.exists());
    // 존재하지 않으면 기본값으로 모두 false -> "---------"
    EXPECT_EQ(info.to_string(), "---------");
}

#if !defined(_WIN32)
// POSIX 전용: chmod로 모드 비트 설정 후 문자열 비교
TEST(permissions, PosixModeBits) {
    auto p = make_temp_file_path();
    {
        std::ofstream ofs(p.string());
        ofs << "hello\n";
    }
    // 0754 -> owner=rwx, group=r-x, others=r--
    int rc = ::chmod(p.string().c_str(), 0754);
    if (rc != 0) {
        // 환경에 따라 chmod 권한 제한이 있을 수 있음 -> 스킵
        GTEST_SKIP() << "chmod not permitted in this environment";
    }

    j2::file::permissions_info info(p.string());
    EXPECT_TRUE(info.exists());
    EXPECT_EQ(info.to_string(), "rwxr-xr--");

    std::filesystem::remove(p);
}
#else
// Windows: ACL 처리가 환경마다 달라서 기본 동작(예외 없이 호출되는지)만 검증
TEST(permissions, WindowsBasic) {

    // 테스트를 위한 임시 파일 생성
    auto p = make_temp_file_path();
    {
        std::ofstream ofs(p.string());
        ofs << "hello\n";
    }

    // 파일 권한 정보 조회.
    j2::file::permissions_info info(p.string());
    
    EXPECT_TRUE(info.exists()); // 파일 존재 여부 점검

    // to_string은 항상 9글자(rwxrwxrwx 형태의 자리) 반환하도록 구현되어 있음
    EXPECT_EQ(info.to_string().size(), 9u);

    std::filesystem::remove(p); // 임시 파일 정리
}
#endif
