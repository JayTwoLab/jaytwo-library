#include <gtest/gtest.h>
#include <string>
#include <stdexcept>
#include <fstream>

#include "j2_library/expected/expected.hpp"

/* =========================
 * 성공 값
 * ========================= */
TEST(expected_test, success_value)
{
    j2::expected::expected<int, std::string> r{ 42 }; // 성공 값 생성

    EXPECT_TRUE(r.has_value()); // 성공 상태 확인
    EXPECT_TRUE(static_cast<bool>(r)); // 불리언 컨텍스트에서 true
    EXPECT_EQ(r.value(), 42); // 성공 값 확인
}

/* =========================
 * 실패 값
 * ========================= */
TEST(expected_test, error_value)
{
    j2::expected::unexpected_value<std::string> err{ "error occurred" };
    j2::expected::expected<int, std::string> r{ err }; // 실패 값 생성

    EXPECT_FALSE(r.has_value()); // 실패 상태 확인
    EXPECT_FALSE(static_cast<bool>(r)); // 불리언 컨텍스트에서 false
    EXPECT_EQ(r.error(), "error occurred"); // 실패 값 확인
}

/* =========================
 * value() 예외
 * ========================= */
TEST(expected_test, value_throws_when_error)
{
    j2::expected::unexpected_value<std::string> err{ "bad" };
    j2::expected::expected<int, std::string> r{ err }; // 실패 값 생성

    EXPECT_THROW(r.value(), std::logic_error); // value() 호출 시 예외 발생
}

/* =========================
 * error() 예외
 * ========================= */
TEST(expected_test, error_throws_when_value)
{
    j2::expected::expected<int, std::string> r{ 10 }; // 성공 값 생성

    EXPECT_THROW(r.error(), std::logic_error); // error() 호출 시 예외 발생
}

/* =========================
 * 함수 반환 예제
 * ========================= */
static j2::expected::expected<int, std::string> parse_number_123(const std::string& s)
{
    if (s == "123")
        return j2::expected::expected<int, std::string>{ 123 }; // 성공 값 반환

    return j2::expected::expected<int, std::string>{ j2::expected::unexpected_value<std::string>{"not a number"} }; // 실패 값 반환 
}

TEST(expected_test, function_return_success)
{
    auto r = parse_number_123("123"); // 성공 값 반환

    ASSERT_TRUE(r); // 성공 상태 확인
    EXPECT_EQ(r.value(), 123); // 성공 값 확인
}

TEST(expected_test, function_return_error)
{
    auto r = parse_number_123("abc"); // 실패 값 반환

    ASSERT_FALSE(r); // 실패 상태 확인
    EXPECT_EQ(r.error(), "not a number"); // 실패 값 확인
}

//-----------------------------------------
// 파일 열기 함수 예제

// 성공 시 std::ifstream 반환, 실패 시 std::string 오류 메시지 반환
j2::expected::expected<std::ifstream, std::string> open_file(const std::string& path)
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
        // log_error(r.error()); // 실패 처리. 문자열 오류 메시지 사용.
        return false;
    }

    // auto& file = r.value(); // 성공적으로 열린 파일 사용

    return true;
}
//-----------------------------------------


