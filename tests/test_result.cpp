#include "j2_library/result/result.hpp"

#include <gtest/gtest.h>
#include <string>
#include <variant>

// 성공 케이스 2종류 정의
enum class basic_status { ok, created };
struct user_profile { int user_id; std::string user_name; };

// 실패 케이스 2종류 정의
enum class network_err { timeout, unreachable };
struct validation_err { std::string field_name; std::string err_msg; };

// 사용자 Result 타입 정의 (std::variant 두 개를 명시적으로 전달)
using my_result = j2::result::result_container<
    std::variant<basic_status, user_profile>, // 성공 케이스: Enum과 Struct
    std::variant<network_err, validation_err> // 실패 케이스: Enum과 Struct
>;

// my_result의 코든 결과 분기가 되는 코드는 다음과 같다.
// 
// auto res = my_result::success(basic_status::ok);
// 
// res.match(
//     j2::result::overload{
//         [&](basic_status s) { /* 성공 케이스: Enum */ },
//         [](const user_profile& p) { /* 성공 케이스: Struct */ }
//     },
//     j2::result::overload{
//         [](network_err e) { /* 실패 케이스: Enum */ },
//         [](const validation_err& v) { /* 실패 케이스: Struct */ }
//     }
// );

// --- Test Cases ---

// 1. 성공 케이스 (Enum) 검증
TEST(ResultTest, SuccessWithEnum) {
    auto res = my_result::success(basic_status::ok);

    EXPECT_TRUE(res.is_success());

    bool called = false;
    res.match(
        j2::result::overload{
            [&](basic_status s) {
                EXPECT_EQ(s, basic_status::ok);
                called = true;
            },
            [](const user_profile&) { FAIL() << "Should not reach here"; }
        },
        [](auto&&) { FAIL() << "Should not reach failure block"; }
    );
    EXPECT_TRUE(called);
}

// 2. 성공 케이스 (Struct 데이터) 검증
TEST(ResultTest, SuccessWithStruct) {
    auto res = my_result::success(user_profile{ 123, "test_user" });

    EXPECT_TRUE(res.is_success());

    res.match(
        j2::result::overload{
            [](basic_status) { FAIL(); },
            [](const user_profile& p) {
                EXPECT_EQ(p.user_id, 123);
                EXPECT_EQ(p.user_name, "test_user");
            }
        },
        [](auto&&) { FAIL(); }
    );
}

// 3. 실패 케이스 (Enum) 검증
TEST(ResultTest, FailWithEnum) {
    auto res = my_result::fail(network_err::timeout);

    EXPECT_FALSE(res.is_success());

    res.match(
        [](auto&&) { FAIL(); },
        j2::result::overload{
            [](network_err e) { EXPECT_EQ(e, network_err::timeout); },
            [](const validation_err&) { FAIL(); }
        }
    );
}

// 4. 실패 케이스 (Struct 데이터) 검증
TEST(ResultTest, FailWithStruct) {
    auto res = my_result::fail(validation_err{ "email", "invalid" });

    EXPECT_FALSE(res.is_success());

    res.match(
        [](auto&&) { FAIL(); },
        j2::result::overload{
            [](network_err) { FAIL(); },
            [](const validation_err& v) {
                EXPECT_EQ(v.field_name, "email");
                EXPECT_EQ(v.err_msg, "invalid");
            }
        }
    );
}

