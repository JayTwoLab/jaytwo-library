// GoogleTest 기반 INI 테스트
// - 다양한 조합을 최대한 커버하도록 케이스 구성
// - 파일 I/O 포함: demo_gtest.ini 생성/추가/재로드 테스트

#include <fstream>
#include <string>

#include <gtest/gtest.h> // GoogleTest 

#include "j2_library/j2_library.hpp" // j2 library

// 테스트용 경로(상대 경로 사용)
static const char* kIniPath = "demo_gtest.ini";

// 도우미: 파일 전체 읽기
static std::string ReadAll(const std::string& path) {
    std::ifstream ifs(path, std::ios::binary);
    std::string s;
    if (!ifs) return s;
    ifs.seekg(0, std::ios::end);
    s.resize(static_cast<size_t>(ifs.tellg()));
    ifs.seekg(0, std::ios::beg);
    ifs.read(&s[0], s.size());
    return s;
}

// 테스트 픽스처(Test Fixture) : 테스트 실행 전에 준비해야 하는 공통 환경이나 데이터, 그리고 이를 초기화/정리하는 절차
class IniTest : public ::testing::Test {
protected:
    void SetUp() override { // 각 테스트 시작 전에 자동 호출 (공통 초기화)
        // 1) 기본 파일 저장
        j2::ini::Ini cfg;

        // 문자열(Raw)
        cfg.set_string("general", "msg1", "hello\tworld");
        cfg.set_string("general", "multiline", "line1\nline2");

        // 문자열(Literal)
        cfg.set_string_literal("general", "pattern", R"(\d+\s+\w+)");

        // 빈 문자열(Raw/Literal)
        cfg.set_string("general", "empty_raw", "");
        cfg.set_string_literal("general", "empty_literal", "");

        // 불리언/정수/실수  (명시적 세터 사용)
        cfg.set_bool("feature", "enabled", true);
        cfg.set_int("server", "port", 8080);
        cfg.set_double("math", "pi", 3.14159);

        // 타입 불일치 테스트용
        cfg.set_string("cases", "bad_int", "abc");

        ASSERT_TRUE(cfg.save(kIniPath));

        // 2) 다양한 케이스 재현을 위해 파일에 추가
       {
            std::ofstream app(kIniPath, std::ios::app | std::ios::binary);
            ASSERT_TRUE(static_cast<bool>(app));
            app << "\n; ----- appended test lines below -----\n";
            app << "[append]\n";
            app << "from_file_inline=hello # inline comment\n";
            app << "rawspace=abc   \n";        // 뒤 공백 -> 트림되어 "abc"
            app << "rawtabs=col1\\tcol2\n";    // \t -> 탭으로 복원
            app << "truthy_raw=yes\n";         // bool true 로 인식
            app << "falsey_raw=off\n";         // bool false 로 인식
            app << "float_bad=not_a_double\n"; // double 실패 케이스
       }

        // 3) 재로드
        ASSERT_TRUE(cfg2_.load(kIniPath));
    }

    void TearDown() override { // 각 테스트 종료 후에 자동 호출 (공통 정리)
        // 테스트용 파일 삭제
        std::remove(kIniPath);
    }

    j2::ini::Ini cfg2_; // 재로드된 설정 객체. 각 테스트에서 사용.
};

// [문자열] 존재/부재/빈 문자열/Raw/Literal 동작
TEST_F(IniTest, StringPresenceAndModes) {
    auto msg1 = cfg2_.get_string("general", "msg1");
    ASSERT_TRUE(msg1.has_value());
    EXPECT_NE(msg1->find('\t'), std::string::npos); // 실제 탭 포함

    auto missing = cfg2_.get_string("general", "does_not_exist");
    EXPECT_FALSE(missing.has_value());               // 값 없음

    auto empty_raw = cfg2_.get_string("general", "empty_raw");
    ASSERT_TRUE(empty_raw.has_value());              // 빈 문자열 존재
    EXPECT_TRUE(empty_raw->empty());

    auto empty_lit = cfg2_.get_string("general", "empty_literal");
    ASSERT_TRUE(empty_lit.has_value());
    EXPECT_TRUE(empty_lit->empty());

    auto pattern = cfg2_.get_string("general", "pattern");
    ASSERT_TRUE(pattern.has_value());
    // Literal: 이스케이프 해석 없이 그대로 보존
    EXPECT_EQ(*pattern, R"(\d+\s+\w+)");
}

// [문자열] inline 주석, 트림, \t 복원
TEST_F(IniTest, StringInlineCommentTrimAndTab) {
    auto inlinev = cfg2_.get_string("append", "from_file_inline");
    ASSERT_TRUE(inlinev.has_value());
    EXPECT_EQ(*inlinev, "hello"); // '# ...'는 주석으로 제거

    auto rawspace = cfg2_.get_string("append", "rawspace");
    ASSERT_TRUE(rawspace.has_value());
    EXPECT_EQ(*rawspace, "abc");  // 뒤 공백 트림

    auto rawtabs = cfg2_.get_string("append", "rawtabs");
    ASSERT_TRUE(rawtabs.has_value());
    // 파일에는 "col1\\tcol2" 이었지만, 메모리에는 탭으로 복원
    EXPECT_EQ(*rawtabs, std::string("col1\tcol2"));
}

// [불리언] true/false, truthy/falsey 텍스트
TEST_F(IniTest, BooleanParsing) {
    auto enabled = cfg2_.get_bool("feature", "enabled");
    ASSERT_TRUE(enabled.has_value());
    EXPECT_TRUE(*enabled);

    auto truthy = cfg2_.get_bool("append", "truthy_raw");
    ASSERT_TRUE(truthy.has_value());
    EXPECT_TRUE(*truthy);

    auto falsey = cfg2_.get_bool("append", "falsey_raw");
    ASSERT_TRUE(falsey.has_value());
    EXPECT_FALSE(*falsey);

    // 문자열 키를 bool로 시도 -> 실패(nullopt)
    auto bad_bool = cfg2_.get_bool("general", "msg1");
    EXPECT_FALSE(bad_bool.has_value());
}

// [정수] 성공/실패
TEST_F(IniTest, IntegerParsing) {
    auto port = cfg2_.get_int("server", "port");
    ASSERT_TRUE(port.has_value());
    EXPECT_EQ(*port, 8080);

    auto bad_int = cfg2_.get_int("cases", "bad_int");
    EXPECT_FALSE(bad_int.has_value());
}

// [실수] 성공/실패
TEST_F(IniTest, DoubleParsing) {
    auto pi = cfg2_.get_double("math", "pi");
    ASSERT_TRUE(pi.has_value());
    EXPECT_NEAR(*pi, 3.14159, 1e-12);

    auto bad_dbl = cfg2_.get_double("append", "float_bad");
    EXPECT_FALSE(bad_dbl.has_value());

    auto str_to_double = cfg2_.get_double("general", "msg1");
    EXPECT_FALSE(str_to_double.has_value());
}

// [존재 확인] 섹션/키 존재성
TEST_F(IniTest, ExistenceChecks) {
    EXPECT_TRUE(cfg2_.has_section("general"));
    EXPECT_FALSE(cfg2_.has_section("no_section"));

    EXPECT_TRUE(cfg2_.has("general", "msg1"));
    EXPECT_FALSE(cfg2_.has("general", "nope"));
}

// [저장형식 점검] 파일 내용이 규칙대로 기록되는지 확인
TEST_F(IniTest, FileFormatAfterSave) {
    // 메시지 일부 수정 후 저장
    j2::ini::Ini tmp = cfg2_;
    // Raw 문자열에 탭/개행 포함
    tmp.set_string("general", "msg1", "a\tb\nc");
    // Literal 문자열: 백슬래시/문자 그대로
    tmp.set_string_literal("general", "pattern", R"(C:\path\with\backslash)");
    ASSERT_TRUE(tmp.save(kIniPath));

    std::string text = ReadAll(kIniPath);
    // Raw: '...' + \t \n 이스케이프
    EXPECT_NE(text.find("msg1='a\\tb\\nc'"), std::string::npos);
    // Literal: "..." (큰따옴표 사용)
    EXPECT_NE(text.find("pattern=\"C:\\path\\with\\backslash\""),
        std::string::npos);
}
