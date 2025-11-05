// hello-file-finder/main.cpp
#include <iostream>

// ✅ 반드시 추가: j2::file 네임스페이스를 선언하는 헤더
#include <j2_library/file/file_finder.hpp>

int main() {
    namespace fs = std::filesystem;

    std::string cmakePath = CMAKE_SOURCE_DIR_PATH; // CMake에서 전달된 매크로 → 문자열
    std::filesystem::path basePath(cmakePath); // filesystem::path 로 감싸기

    // 기준 경로 출력 및 하위 항목 나열
    {
        std::cout << std::filesystem::canonical(basePath) << "\n";
        for (auto& p : std::filesystem::recursive_directory_iterator(basePath))
            std::cout << p.path() << "\n";
        std::cout << "-------------------------\n";
    }

    // (A) 비재귀 + 대소문자 무시(기본값)
    {
        // bool ok = j2::file::exists(fs::path("./hello"), fs::path("world.txt"));
        bool ok = j2::file::exists(basePath, fs::path("world.txt"));
        std::cout << "[A] world.txt: " << (ok ? "exist" : "not exist") << "\n";
    }

    // (B) 재귀 + 대소문자 구분
    {
        // bool ok = j2::file::exists(fs::path("./hello"), fs::path("picture.txt"),
        bool ok = j2::file::exists(basePath, fs::path("picture.txt"),
            /*recursive=*/true, /*case_insensitive=*/false);
        std::cout << "[B] picture.txt: " << (ok ? "exist" : "not exist") << "\n";
    }

#if defined(_WIN32)

    std::locale::global(std::locale(""));  // 콘솔 로케일 설정
    std::wcout.imbue(std::locale());

    // (C) 윈도우 안전: 와이드 리터럴
    {
        // bool ok = j2::file::exists(L"./hello", L"보고서.txt", /*recursive=*/false);
        bool ok = j2::file::exists(basePath, L"보고서.txt", /*recursive=*/false);
        std::wcout << L"[C] L'보고서.txt': " << (ok ? L"존재" : L"없음") << L"\n";
    }

    // (D) 윈도우 안전: UTF-8 리터럴 → u8path
    {
        // bool ok = j2::file::exists(fs::u8path(u8"./hello"), fs::u8path(u8"보고서_최종본.txt"));
        bool ok = j2::file::exists(basePath, fs::u8path(u8"보고서_최종본.txt"));
        std::cout << "[D] u8path: " << (ok ? "exist" : "not exist") << "\n";
        // 윈도 콘솔에서는 utf8 문자열이 깨짐. 
    }
#endif
}
