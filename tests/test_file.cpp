#include <iostream>
#include <fstream>
#include <gtest/gtest.h>

#include "j2_library/file/file.hpp"

// 테스트: 경로와 이름이 정상적으로 얻어지는지
TEST(ExecutablePathTest, PathAndDirNotEmpty) {
    auto path = j2::file::executable_path(); // 경로 + 파일명 (표준 path 객체 반환)
    auto dir = j2::file::executable_dir(); // 경로만 (표준 path 객체 반환)
    // NOTE: path 객체는 문자열로 변환 가능 : path.string() 또는 path.u8string()

    EXPECT_FALSE(path.empty()); // 예> "C:\Program Files\Hello.exe"
    EXPECT_FALSE(dir.empty()); // 예> "C:\Program Files"
    EXPECT_EQ(dir, path.parent_path());
}

TEST(ExecutablePathTest, PathStringVersions) {
    auto s = j2::file::executable_path_string();   // 경로 + 파일명 (문자열 반환)
    auto ws = j2::file::executable_path_wstring();  // 경로 + 파일명 (unicode 문자열 반환)
    auto ds = j2::file::executable_dir_string();    // 경로만 (문자열 반환)
    auto dws = j2::file::executable_dir_wstring();   // 경로만 (unicode 문자열 반환)

    EXPECT_FALSE(s.empty()); // 예> "C:\Program Files\Hello.exe"
    EXPECT_FALSE(ws.empty()); // 예> L"C:\Program Files\Hello.exe"
    EXPECT_FALSE(ds.empty()); // 예> "C:\Program Files"
    EXPECT_FALSE(dws.empty()); // 예> L"C:\Program Files"
}

TEST(ExecutableNameTest, FilenameAndStem) {
    auto name_with_ext = j2::file::executable_name(j2::file::NameType::Filename); // 확장자 포함된 파일명. 예> Hello.exe
    auto name_stem = j2::file::executable_name(j2::file::NameType::Stem); // 확장자 없는 파일명. 예> Hello

    EXPECT_FALSE(name_with_ext.empty());
    EXPECT_FALSE(name_stem.empty());

    // 플랫폼에 관계없이 filesystem 의미론으로 검증
    std::filesystem::path p{ name_with_ext };
#if defined(_WIN32)
    EXPECT_EQ(p.stem().u8string(), name_stem);
#else
    EXPECT_EQ(p.stem().string(), name_stem);
#endif

}

TEST(ExecutableNameTest, WstringVersions) {
    auto name_with_ext_w = j2::file::executable_name_w(j2::file::NameType::Filename); // 확장자 포함된 유니코드 파일명. 예> Hello.exe
    auto name_stem_w = j2::file::executable_name_w(j2::file::NameType::Stem); // 확장자 없는 유니코드 파일명. 예> Hello

    EXPECT_FALSE(name_with_ext_w.empty());
    EXPECT_FALSE(name_stem_w.empty());
}

TEST(FileReadTest, ReadFileInSubdir) {
    // 1. 경로 객체 얻기
    auto base_dir = j2::file::executable_dir(); // std::filesystem::path
    auto file_path = base_dir / "test1" / "t1.txt"; // 하위 경로 결합. base_dir 경로 밑의 test1/t1.txt 

    // 2. 문자열 경로 얻기
    std::string file_path_str = file_path.string();
    std::cout << "Path: " << file_path << std::endl;
     
    // 3. 파일 내용 읽기
    // std::ifstream ifs(file_path);
    // ASSERT_TRUE(ifs.is_open()) << "파일 열기 실패: " << file_path_str;
    // std::stringstream buffer;
    // buffer << ifs.rdbuf();
    // std::string file_content = buffer.str();

    // 4. 검증 (예시: 파일이 비어있지 않은지)
    // EXPECT_FALSE(file_content.empty());
    // std::cout << "Content: " << file_content << std::endl;

}
