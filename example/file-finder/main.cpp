// -------- main --------

int main();

#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <fstream>

#include <j2_library/j2_library.hpp>

namespace fs = std::filesystem;

// 함수 선언부 (main에서 사용하는 함수들)
static void dump_tree(const fs::path& base);
static void test_create_picture_and_file_info(const fs::path& base);
static void test_non_recursive_basic(const fs::path& base);
static void test_recursive_case_sensitive(const fs::path& base);
static void test_windows_wide_literal(const fs::path& base);
static void test_windows_u8path_literal(const fs::path& base);
static void test_case_sensitive_negative(const fs::path& base);
static void test_not_found(const fs::path& base);
static void test_relative_hello_demo();
static void test_korean_name_variants(const fs::path& base);
static void test_nonrec_vs_rec(const fs::path& base);

// -------- main --------

int main() {
    // 기준 경로를 CMake에서 주입하거나, 없으면 실행 디렉토리로 대체
#ifndef CMAKE_SOURCE_DIR_PATH
#define CMAKE_SOURCE_DIR_PATH "."
#endif
    fs::path base = fs::path(CMAKE_SOURCE_DIR_PATH);

    dump_tree(base);

    // 상대 경로로 picture.txt를 생성하고 j2::file::file_info 예제를 실행
    test_create_picture_and_file_info(base);

    test_non_recursive_basic(base);
    test_recursive_case_sensitive(base);
    test_windows_wide_literal(base);
    test_windows_u8path_literal(base);
    test_case_sensitive_negative(base);
    test_not_found(base);
    test_relative_hello_demo();
    test_korean_name_variants(base);
    test_nonrec_vs_rec(base);

    return 0;
}

// 이하 기존 함수 정의부 (생략 없이 기존 코드 그대로 유지)

// ... (아래에 기존의 모든 함수 정의가 이어집니다)
#if defined(_WIN32)

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <io.h>
#include <fcntl.h>

// fs::path → UTF-8 문자열
static std::string path_to_utf8(const fs::path& p) {
    // 윈도우에선 네이티브가 UTF-16이므로 wstring → UTF-8
    return j2::encoding::utf16_to_utf8(p.wstring());
}

// stdout이 실제 콘솔인지 판별 (콘솔이면 TRUE)
static bool is_stdout_console() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h == INVALID_HANDLE_VALUE || h == nullptr) return false;
    DWORD mode = 0;
    if (!GetConsoleMode(h, &mode)) return false;
    return true;
}

// UTF-8 문자열 출력(플랫폼 통합)
// - 윈도우 콘솔: WriteConsoleW(UTF-16 변환 후) 사용 → 한글 안전
// - 그 외: UTF-8 바이트 그대로 std::cout으로 출력
static void print_u8(const std::string& s) {
    if (is_stdout_console()) {
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        if (h && h != INVALID_HANDLE_VALUE) {
            std::wstring w = j2::encoding::utf8_to_utf16(s);

            DWORD written = 0;
            if (!w.empty()) {
                WriteConsoleW(h, w.c_str(), static_cast<DWORD>(w.size()), &written, nullptr);
            }
            return;
        }
    }
    // 콘솔이 아니면(리다이렉트/파이프), UTF-8 바이트 출력
    std::cout << s;
}

static void println_u8(const std::string& s) {
    print_u8(s);
    print_u8("\n");
}

#else
// POSIX: fs::path → UTF-8 문자열 (대부분 path.string()이 UTF-8)
static std::string path_to_utf8(const fs::path& p) {
    return p.string();
}
static void print_u8(const std::string& s) { std::cout << s; }
static void println_u8(const std::string& s) { std::cout << s << "\n"; }
#endif

// -------- 출력 유틸 --------

// 테스트 케이스 결과 한 줄 출력
static void print_case(const std::string& title_utf8, bool ok, bool expected) {
    std::string line = title_utf8 + " : ";
    line += (ok ? "TRUE" : "FALSE");
    line += " (expected ";
    line += (expected ? "TRUE" : "FALSE");
    line += ")  ";
    line += ((ok == expected) ? "[PASS]" : "[FAIL]");
    println_u8(line);
}

// 디렉터리 1레벨 나열(경로는 UTF-8로 출력)
static void dump_tree(const fs::path& base) {
    std::string head = "== canonical(base) = \"" + path_to_utf8(fs::canonical(base)) + "\"";
    println_u8(head);
    for (auto& p : fs::directory_iterator(base)) {
        std::string line = "  - " + path_to_utf8(p.path());
        println_u8(line);
    }
    println_u8("-------------------------");
}

// 새로 추가: 상대 경로로 picture.txt 생성 후 j2::file::file_info 사용 예제
static void test_create_picture_and_file_info(const fs::path& base) {
    println_u8(u8"[X] 상대경로로 picture.txt 생성 및 j2::file::file_info 사용 예제 시작");
    try {
        fs::path dir = base / "assets";
        fs::create_directories(dir);
        fs::path full = dir / "picture.txt";

        // 파일 생성 (바이너리/텍스트 둘 다 가능; 예제는 텍스트)
        {
            std::ofstream ofs(full, std::ios::binary);
            ofs << "This is a placeholder for picture.txt\n";
        }

        // 상대 경로 예제를 위해 작업 디렉터리를 base로 잠시 변경
        fs::path old_cwd = fs::current_path();
        fs::current_path(base);

        // 상대 경로(예: "assets/picture.txt")와 절대 경로 모두로 file_info 생성
        j2::file::file_info fi_rel("assets/picture.txt");
        j2::file::file_info fi_abs(full.string());

        // 출력 헬퍼
        auto print_info = [&](const std::string& title, const j2::file::file_info& fi) {
            println_u8(title);
            println_u8(std::string(u8"  exists: ") + (fi.exists() ? "TRUE" : "FALSE"));
            println_u8(std::string(u8"  is_file: ") + (fi.is_file() ? "TRUE" : "FALSE"));
            println_u8(std::string(u8"  is_dir: ") + (fi.is_dir() ? "TRUE" : "FALSE"));
            println_u8(std::string(u8"  file_name: ") + fi.file_name());
            println_u8(std::string(u8"  extension: ") + fi.extension());
            println_u8(std::string(u8"  parent_path: ") + fi.parent_path());
            println_u8(std::string(u8"  absolute_path: ") + fi.absolute_path());
            try {
                println_u8(std::string(u8"  file_size: ") + std::to_string(fi.file_size()) + "B");
            } catch (...) {
                println_u8(std::string(u8"  file_size: <error>"));
            }
        };

        print_info(u8"  - file_info(\"assets/picture.txt\")", fi_rel);
        print_info(std::string(u8"  - file_info(\"") + full.string() + "\")", fi_abs);

        // 작업 디렉터리 복원
        fs::current_path(old_cwd);
    } catch (const std::exception& ex) {
        println_u8(std::string(u8"[X] 예외 발생: ") + ex.what());
    }
    println_u8(u8"[X] 예제 종료");
}

// -------- 테스트 함수들 --------

// (A) 비재귀 + 대소문자 무시(기본) — base 바로 아래 world.txt
static void test_non_recursive_basic(const fs::path& base) {
    bool ok = j2::file::exists(base, fs::path("world.txt")); // recursive=false, case_insensitive=true
    print_case(u8"[A] 비재귀/대소문자무시 world.txt", ok, /*expected*/true);
}

// (B) 재귀 + 대소문자 구분 — picture.txt
static void test_recursive_case_sensitive(const fs::path& base) {
    bool ok = j2::file::exists(base, fs::path("picture.txt"),
        /*recursive*/true, /*case_insensitive*/false);
    print_case(u8"[B] 재귀/대소문자구분 picture.txt", ok, /*expected*/true);
}

// (C) 와이드 리터럴 — 보고서.txt
static void test_windows_wide_literal(const fs::path& base) {
#if defined(_WIN32)
    bool ok = j2::file::exists(base, L"보고서.txt", /*recursive*/false);
    print_case(u8"[C] 비재귀/와이드 리터럴 '보고서.txt'", ok, /*expected*/true);
#else
    // POSIX: 로캘이 UTF-8이면 대개 통과. (비결정성 감수)
    bool ok = j2::file::exists(base, fs::path(L"보고서.txt"), /*recursive*/false);
    print_case(u8"[C] POSIX/와이드 '보고서.txt' (로캘 UTF-8 가정)", ok, /*expected*/true);
#endif
}

// (D) UTF-8 리터럴 + u8path — 보고서_최종본.txt
static void test_windows_u8path_literal(const fs::path& base) {
#if defined(_WIN32)
    bool ok = j2::file::exists(base, fs::u8path(u8"보고서_최종본.txt"));
    print_case(u8"[D] 비재귀/u8path '보고서_최종본.txt'", ok, /*expected*/true);
#else
    // POSIX: 일반 path("…")와 결과가 같을 것이므로 그냥 검증해도 됨
    bool ok = j2::file::exists(base, fs::u8path(u8"보고서_최종본.txt"));
    print_case(u8"[D] POSIX/u8path '보고서_최종본.txt'", ok, /*expected*/true);
#endif
}


// (E) 대소문자 구분 네거티브 — World.TXT(없다고 가정)
static void test_case_sensitive_negative(const fs::path& base) {
    bool ok = j2::file::exists(base, fs::path("World.TXT"),
        /*recursive*/false, /*case_insensitive*/false);
    print_case(u8"[E] 비재귀/대소문자구분 'World.TXT'", ok, /*expected*/false);
}

// (F) 존재하지 않는 파일 네거티브
static void test_not_found(const fs::path& base) {
    bool ok = j2::file::exists(base, fs::path("this_file_does_not_exist_123.txt"));
    print_case(u8"[F] 비재귀/부재 파일", ok, /*expected*/false);
}

// (G) 상대 경로 ./hello 데모(환경에 따라 결과 다름)
static void test_relative_hello_demo() {
    bool ok = j2::file::exists(fs::path("./hello"), fs::path("world.txt"));
    std::string line = std::string(u8"[G] ./hello 기준 world.txt : ") + (ok ? "TRUE" : "FALSE")
        + u8"  (실제 ./hello/world.txt 유무에 따라 달라집니다)";
    println_u8(line);
}

// (H) 한글 파일명: 위험한 좁은 문자열은 SKIP, u8path/L""만 테스트
static void test_korean_name_variants(const fs::path& base) {
#if defined(_WIN32)
    unsigned acp = GetACP();
    println_u8("[H] Windows ACP=" + std::to_string(acp) + " (65001=UTF-8)");
    println_u8(u8"[H1] Windows fs::path(\"한글\"): SKIP (unsafe on Windows)");

    bool b = j2::file::exists(base, fs::u8path(u8"보고서_최종본.txt"));
    print_case(u8"[H2] Windows u8path(u8\"보고서_최종본.txt\")", b, /*expected*/true);

    bool c = j2::file::exists(base, fs::path(L"보고서_최종본.txt"));
    print_case(u8"[H3] Windows L\"보고서_최종본.txt\"", c, /*expected*/true);
#else
    bool a = j2::file::exists(base, fs::path("보고서_최종본.txt"));
    bool b = j2::file::exists(base, fs::u8path(u8"보고서_최종본.txt"));
    print_case(u8"[H] POSIX '보고서_최종본.txt' narrow/u8path", (a && b), /*expected*/true);
#endif
}

// (I) 비재귀 vs 재귀 비교
static void test_nonrec_vs_rec(const fs::path& base) {
    bool nonrec = j2::file::exists(base, fs::path("world.txt"), /*recursive*/false);
    bool rec = j2::file::exists(base, fs::path("world.txt"), /*recursive*/true);
    std::string line = std::string(u8"[I] world.txt nonrec=") + (nonrec ? "TRUE" : "FALSE")
        + ", rec=" + (rec ? "TRUE" : "FALSE")
        + u8"  (하위 폴더에 있으면 nonrec=FALSE, rec=TRUE 가 됩니다)";
    println_u8(line);
}
