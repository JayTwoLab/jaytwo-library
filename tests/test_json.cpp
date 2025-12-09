// test_json.cpp
// GoogleTest를 이용해 j2::json 유틸리티를 검증하는 테스트 모음
// - exists(path): 경로 존재 여부
// - get_string/get_bool/get_int/get_double: 타입 안전 value_or 래퍼 (예외 없음)
// 검증 항목:
//   * 경로 존재/미존재
//   * 타입 일치/불일치 시 기본값 반환
//   * 숫자 변환: 정수/부동, 정수성(소수점), 범위 초과, NaN/Inf 거부
//   * JSON Pointer 이스케이프(~0, ~1) 및 배열 인덱싱
//   * null 처리(기본값 반환)

#include <gtest/gtest.h>
#include "j2_library/json/json.hpp"

// 별칭은 헤더(json.hpp)에서 이미 다음과 같이 정의되어 있음:
// using nj     = nlohmann::json;
// namespace jj = j2::json;

TEST(JsonExists, BasicTrueFalse) {
    // 준비: 간단한 설정 JSON
    nj j = {
        {"config", {
            {"database", {
                {"host", "db.local"},
                {"port", 5432},
                {"ssl",  true}
            }}
        }}
    };

    // 존재하는 경로는 true
    EXPECT_TRUE(jj::exists(j, "/config/database/host"));
    EXPECT_TRUE(jj::exists(j, "/config/database/port"));
    EXPECT_TRUE(jj::exists(j, "/config/database/ssl"));

    // 존재하지 않는 경로는 false
    EXPECT_FALSE(jj::exists(j, "/config/database/user"));
    EXPECT_FALSE(jj::exists(j, "/config/does_not_exist"));

    // 중간 노드까지만 있고 리프가 없으면 false
    EXPECT_FALSE(jj::exists(j, "/config/database/port/x"));
}

TEST(JsonExists, EscapedKeysWithTildeAndSlash) {
    // 준비: 키에 '/' 와 '~' 가 포함된 경우 JSON Pointer 이스케이프 필요
    // JSON Pointer 규칙:
    //   '~' -> '~0', '/' -> '~1'
    nj j;
    j["config"]["weird"]["a/b"] = 1;   // '/' 포함
    j["config"]["weird"]["c~d"] = 2;   // '~' 포함

    // "/config/weird/a~1b" 가 "a/b" 키를 가리킴
    EXPECT_TRUE(jj::exists(j, "/config/weird/a~1b"));
    // "/config/weird/c~0d" 가 "c~d" 키를 가리킴
    EXPECT_TRUE(jj::exists(j, "/config/weird/c~0d"));

    // 잘못된 이스케이프 혹은 오타는 false
    EXPECT_FALSE(jj::exists(j, "/config/weird/a/ b")); // 공백 포함 오타
    EXPECT_FALSE(jj::exists(j, "/config/weird/a~0b")); // 잘못된 이스케이프
}

TEST(JsonGetString, MatchAndMismtachAndNull) {
    nj j;
    j["config"]["database"]["host"] = "db.local";
    j["config"]["database"]["port"] = 5432;      // 문자열 아님
    j["config"]["database"]["note"] = nullptr;   // null

    // 타입 일치: 문자열 반환
    EXPECT_EQ(jj::get_string(j, "/config/database/host", "127.0.0.1"), "db.local");

    // 타입 불일치: 기본값 반환
    EXPECT_EQ(jj::get_string(j, "/config/database/port", "127.0.0.1"), "127.0.0.1");

    // null: 기본값 반환
    EXPECT_EQ(jj::get_string(j, "/config/database/note", "no-note"), "no-note");

    // 존재하지 않음: 기본값 반환
    EXPECT_EQ(jj::get_string(j, "/config/database/user", "postgres"), "postgres");
}

TEST(JsonGetBool, MatchAndMismatch) {
    nj j;
    j["feature"]["enabled"] = true;
    j["feature"]["flag_as_number"] = 1;  // bool이 아님(숫자)
    j["feature"]["text"] = "true";       // bool이 아님(문자열)

    // 타입 일치: 불리언 반환
    EXPECT_TRUE(jj::get_bool(j, "/feature/enabled", false));

    // 타입 불일치: 기본값 반환
    EXPECT_FALSE(jj::get_bool(j, "/feature/flag_as_number", false));
    EXPECT_TRUE(jj::get_bool(j, "/feature/flag_as_number", true));  // 기본값만 바꾸면 그대로 반영
    EXPECT_FALSE(jj::get_bool(j, "/feature/text", false));

    // 존재하지 않음: 기본값 반환
    EXPECT_TRUE(jj::get_bool(j, "/feature/missing", true));
}

TEST(JsonGetInt, FromIntAndIntegralFloat) {
    nj j;
    j["config"]["port_int"] = 5432;   // 정수
    j["config"]["port_float_integral"] = 6000.0; // 소수부 0.0인 부동소수

    // 정수는 그대로
    EXPECT_EQ(jj::get_int(j, "/config/port_int", 0), 5432);

    // 소수부가 0.0이면 정수로 캐스팅 허용
    EXPECT_EQ(jj::get_int(j, "/config/port_float_integral", 0), 6000);
}

TEST(JsonGetInt, RejectFractionalFloatAndWrongType) {
    nj j;
    j["config"]["port_float_fraction"] = 6000.5; // 소수부 존재
    j["config"]["text"] = "7000";                // 문자열
    j["config"]["flag"] = true;                  // bool

    // 소수부 존재 -> 정수성 불만족, 기본값
    EXPECT_EQ(jj::get_int(j, "/config/port_float_fraction", 1234), 1234);

    // 문자열/불리언 -> 타입 불일치, 기본값
    EXPECT_EQ(jj::get_int(j, "/config/text", 4321), 4321);
    EXPECT_EQ(jj::get_int(j, "/config/flag", 1), 1);
}

TEST(JsonGetInt, OutOfRange) {
    nj j;
    // int 범위를 초과하는 큰 수: 2147483648(= INT_MAX + 1)
    j["big"]["beyond_int_max"] = 2147483648ull;
    // 음수로도 범위 밖을 만들 수 있지만 여기서는 양수 초과만 검사
    // (MSVC/nlohmann에서 number_unsigned로 잡히도록 ull 사용)

    // 범위 초과 -> 기본값
    EXPECT_EQ(jj::get_int(j, "/big/beyond_int_max", -1), -1);
}

TEST(JsonGetDouble, FromIntAndFloat) {
    nj j;
    j["val"]["i"] = 10;       // 정수여도 double 로 안전 변환 허용
    j["val"]["d"] = 3.25;     // 부동소수

    // 정수 -> 10.0
    EXPECT_DOUBLE_EQ(jj::get_double(j, "/val/i", -1.0), 10.0);
    // 부동소수 -> 3.25
    EXPECT_DOUBLE_EQ(jj::get_double(j, "/val/d", -1.0), 3.25);
}

TEST(JsonGetDouble, RejectNaNAndInfAndWrongType) {
    nj j;

    // nlohmann::json 은 C++ 값으로 NaN/Inf 저장은 가능하나 JSON 리터럴로 표현되지는 않음.
    // 테스트에서는 값 주입을 통해 생성한다.
    double qnan = std::numeric_limits<double>::quiet_NaN();
    double pinf = std::numeric_limits<double>::infinity();
    j["val"]["nan"] = qnan;
    j["val"]["inf"] = pinf;

    j["val"]["text"] = "3.14";  // 문자열
    j["val"]["flag"] = false;   // 불리언

    // NaN/Inf 는 try_number_cast 에서 거부되어 기본값 반환
    EXPECT_DOUBLE_EQ(jj::get_double(j, "/val/nan", 1.5), 1.5);
    EXPECT_DOUBLE_EQ(jj::get_double(j, "/val/inf", 2.5), 2.5);

    // 문자열/불리언 -> 기본값
    EXPECT_DOUBLE_EQ(jj::get_double(j, "/val/text", 9.9), 9.9);
    EXPECT_DOUBLE_EQ(jj::get_double(j, "/val/flag", 8.8), 8.8);
}

TEST(JsonArrayPaths, Indexing) {
    // 배열 인덱싱은 JSON Pointer에서 "/키/인덱스" 형태로 접근
    nj j;
    j["arr"] = nj::array({ 10, 20, 30 });

    // 존재 확인
    EXPECT_TRUE(jj::exists(j, "/arr/0"));
    EXPECT_TRUE(jj::exists(j, "/arr/2"));
    EXPECT_FALSE(jj::exists(j, "/arr/3")); // 범위 밖

    // 정수 읽기
    EXPECT_EQ(jj::get_int(j, "/arr/1", -1), 20);

    // 타입 불일치 시 기본값 (여기서는 의도적으로 문자열로 치환)
    j["arr"][1] = "twenty";
    EXPECT_EQ(jj::get_int(j, "/arr/1", -1), -1);
}

TEST(JsonDeepPaths, NestedStructureAndNull) {
    // 깊은 중첩 + null 처리
    nj j = {
        {"a", {
            {"b", {
                {"c", {
                    {"d", "leaf"},
                    {"n", nullptr}
                }}
            }}
        }}
    };

    // 정해진 깊은 경로의 문자열
    EXPECT_EQ(jj::get_string(j, "/a/b/c/d", "x"), "leaf");

    // null 은 기본값
    EXPECT_EQ(jj::get_string(j, "/a/b/c/n", "null-as-default"), "null-as-default");

    // 존재하지 않으면 기본값
    EXPECT_EQ(jj::get_string(j, "/a/b/x", "missing"), "missing");
}

// 메인 함수를 따로 정의할 필요는 없음.
// GoogleTest에서는 gtest_main 라이브러리를 링크하면 자동으로 main 제공.
// 만약 직접 main이 필요하다면 아래를 사용하세요.
/*
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
*/
