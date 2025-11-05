// UTF-8 한글 출력이 윈도우 콘솔/리눅스 터미널 모두에서 정상 동작하도록 구성한 통합 예제
// - 윈도우 콘솔: WriteConsoleW(UTF-16) 사용
// - 윈도우 리다이렉트/파이프: UTF-8 바이트로 출력
// - POSIX(리눅스/맥): UTF-8 바이트로 출력
// - j2::file::exists() 테스트 케이스들을 모두 포함
//
// 빌드 전제:
//   - 소스 인코딩은 UTF-8
//   - MSVC: /utf-8 권장
//
// 주의:
//   - Windows에서 비ASCII 경로는 fs::u8path(u8"...") 또는 L"..." 만 사용할 것
//   - fs::path("한글")은 예외 발생 가능(ACP 변환 실패)

#include <iostream>
#include <string>
#include <filesystem>
#include <vector>

#include <j2_library/j2_library.hpp>

namespace fs = std::filesystem;

#if defined(_WIN32)
#define NOMINMAX
#include <windows.h>
#include <io.h>
#include <fcntl.h>

// UTF-8 → UTF-16 변환
static std::wstring utf8_to_wide(const std::string& s) {
    if (s.empty()) return std::wstring();
    int wlen = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, s.c_str(),
        static_cast<int>(s.size()), nullptr, 0);
    if (wlen <= 0) return L"";
    std::wstring w(wlen, L'\0');
    MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, s.c_str(),
        static_cast<int>(s.size()), &w[0], wlen);
    return w;
}

// UTF-16 → UTF-8 변환
static std::string wide_to_utf8(const std::wstring& w) {
    if (w.empty()) return std::string();
    int len = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), static_cast<int>(w.size()),
        nullptr, 0, nullptr, nullptr);
    if (len <= 0) return std::string();
    std::string s(len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, w.c_str(), static_cast<int>(w.size()),
        &s[0], len, nullptr, nullptr);
    return s;
}

// fs::path → UTF-8 문자열
static std::string path_to_utf8(const fs::path& p) {
    // 윈도우에선 네이티브가 UTF-16이므로 wstring → UTF-8
    return wide_to_utf8(p.wstring());
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
            std::wstring w = utf8_to_wide(s);
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

// (C) 윈도우: 와이드 리터럴 — 보고서.txt
static void test_windows_wide_literal(const fs::path& base) {
#if defined(_WIN32)
    bool ok = j2::file::exists(base, L"보고서.txt", /*recursive*/false);
    print_case(u8"[C] 비재귀/와이드 리터럴 '보고서.txt'", ok, /*expected*/true);
#else
    (void)base;
    print_case(u8"[C] (윈도우 전용 테스트)", true, /*expected*/true);
#endif
}

// (D) 윈도우: UTF-8 리터럴 + u8path — 보고서_최종본.txt
static void test_windows_u8path_literal(const fs::path& base) {
#if defined(_WIN32)
    bool ok = j2::file::exists(base, fs::u8path(u8"보고서_최종본.txt"));
    print_case(u8"[D] 비재귀/u8path '보고서_최종본.txt'", ok, /*expected*/true);
#else
    (void)base;
    print_case(u8"[D] (윈도우 전용 테스트)", true, /*expected*/true);
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

// -------- main --------

int main() {
    // 기준 경로를 CMake에서 주입하거나, 없으면 실행 디렉토리로 대체
#ifndef CMAKE_SOURCE_DIR_PATH
#define CMAKE_SOURCE_DIR_PATH "."
#endif
    fs::path base = fs::path(CMAKE_SOURCE_DIR_PATH);

    dump_tree(base);

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
