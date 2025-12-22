#include <gtest/gtest.h>
#include <string>
#include <stdexcept>
#include <fstream>

#include "j2_library/expected/expected.hpp"

using j2::expected::expected; // expected 템플릿 클래스
using j2::expected::unexpected_value; // unexpected_value 템플릿 클래스

/* ===== 안전한 별칭 ===== */
using exp_int_str = expected<int, std::string>; // 성공 값은 int, 실패 값은 std::string
using err_str = unexpected_value<std::string>; // 실패 값 타입 별칭

/* =========================
 * 성공 값
 * ========================= */
TEST(expected_test, success_value)
{
    exp_int_str r{ 42 }; // 성공 값 생성

    EXPECT_TRUE(r.has_value()); // 성공 상태 확인
    EXPECT_TRUE(static_cast<bool>(r)); // 불리언 컨텍스트에서 true
    EXPECT_EQ(r.value(), 42); // 성공 값 확인
}

/* =========================
 * 실패 값
 * ========================= */
TEST(expected_test, error_value)
{
    err_str err{ "error occurred" };
    exp_int_str r{ err }; // 실패 값 생성

    EXPECT_FALSE(r.has_value()); // 실패 상태 확인
    EXPECT_FALSE(static_cast<bool>(r)); // 불리언 컨텍스트에서 false
    EXPECT_EQ(r.error(), "error occurred"); // 실패 값 확인
}

/* =========================
 * value() 예외
 * ========================= */
TEST(expected_test, value_throws_when_error)
{
    err_str err{ "bad" };
    exp_int_str r{ err }; // 실패 값 생성

    EXPECT_THROW(r.value(), std::logic_error); // value() 호출 시 예외 발생
}

/* =========================
 * error() 예외
 * ========================= */
TEST(expected_test, error_throws_when_value)
{
    exp_int_str r{ 10 }; // 성공 값 생성

    EXPECT_THROW(r.error(), std::logic_error); // error() 호출 시 예외 발생
}

/* =========================
 * 함수 반환 예제
 * ========================= */
static exp_int_str parse_number(const std::string& s)
{
    if (s == "123")
        return exp_int_str{ 123 }; // 성공 값 반환

    return exp_int_str{ err_str{"not a number"} }; // 실패 값 반환 
}

TEST(expected_test, function_return_success)
{
    auto r = parse_number("123"); // 성공 값 반환

    ASSERT_TRUE(r); // 성공 상태 확인
    EXPECT_EQ(r.value(), 123); // 성공 값 확인
}

TEST(expected_test, function_return_error)
{
    auto r = parse_number("abc"); // 실패 값 반환

    ASSERT_FALSE(r); // 실패 상태 확인
    EXPECT_EQ(r.error(), "not a number"); // 실패 값 확인
}

//-----------------------------------------
// 파일 열기 함수 예제
using file_result = j2::expected::expected<std::ifstream, std::string>;

file_result open_file(const std::string& path)
{
    std::ifstream f(path);
    if (!f)
        return j2::expected::unexpected_value<std::string>{"file open failed"}; // 실패 반환

    return f; // 성공적으로 열린 파일 반환
}

bool example_file_open()
{
    auto r = open_file("data.txt");
    if (!r)
    {
        // log_error(r.error());
        return false;
    }

    auto& file = r.value();
    return true;
}
//-----------------------------------------


