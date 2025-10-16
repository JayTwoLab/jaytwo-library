// Google Test 기반 UUID v4 단위 테스트
// - 대상: j2::uuid::UuidV4
// - 검증 항목:
//   1) 형식 검증(8-4-4-4-12, 소문자 hex, 하이픈 위치/개수)
//   2) 버전/바리언트 비트(RFC 4122: version=4, variant=10xx xxxx)
//   3) 소규모 대량 생성 시 중복 없음(예: 20,000개)
//   4) 멀티스레드 생성 시 중복 없음 및 안전성
//
// 빌드 예시:
//   - gtest_main 사용 시: 별도 main 불필요(링크에 gtest_main 포함)
//   - 직접 main 사용 시: 아래의 gtest_main 주석을 해제하거나
//     프로젝트 공용 테스트 main을 사용하세요.

#include <gtest/gtest.h>
#include <regex>
#include <string>
#include <unordered_set>
#include <vector>
#include <thread>
#include <mutex>

#include "j2_library/uuid/uuid_v4.hpp"
#include "j2_library/string/to_console_encoding.hpp"

using j2::uuid::UuidV4;

// UUID 정규식: 소문자 hex와 하이픈 형식을 강제한다.
static const std::regex kUuidRegex(
    R"(^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$)");

TEST(UuidV4, FormatAndLength) {
    // 5개 정도 생성해 형식과 길이를 확인한다.
    for (int i = 0; i < 5; ++i) {
        const std::string id = UuidV4::generate();

        // 길이는 하이픈 포함 36자여야 한다.
        EXPECT_EQ(id.size(), 36u) << j2::string::to_console_encoding(u8"UUID 길이는 36자여야 합니다.");

        // 정규식(소문자 hex, 8-4-4-4-12)을 만족해야 한다.
        EXPECT_TRUE(std::regex_match(id, kUuidRegex))
            << j2::string::to_console_encoding(u8"UUID 형식이 올바르지 않습니다: ") << id;

        // 하이픈 위치가 정확해야 한다(0-index 기준 8,13,18,23).
        EXPECT_EQ(id[8], '-') << j2::string::to_console_encoding(u8"하이픈 위치(8)가 올바르지 않습니다.");
        EXPECT_EQ(id[13], '-') << j2::string::to_console_encoding(u8"하이픈 위치(13)가 올바르지 않습니다.");
        EXPECT_EQ(id[18], '-') << j2::string::to_console_encoding(u8"하이픈 위치(18)가 올바르지 않습니다.");
        EXPECT_EQ(id[23], '-') << j2::string::to_console_encoding(u8"하이픈 위치(23)가 올바르지 않습니다.");
    }
}

TEST(UuidV4, VersionAndVariantBits) {
    // 버전 nibble 은 문자열 14번째(0-index) 문자여야 한다.
    // 바리언트 nibble 은 19번째 문자여야 한다.
    for (int i = 0; i < 20; ++i) {
        const std::string id = UuidV4::generate();

        // version=4 (문자 '4') 확인
        ASSERT_EQ(id.size(), 36u);
        EXPECT_EQ(id[14], '4') << j2::string::to_console_encoding(u8"버전 nibble(14번째)은 '4'여야 합니다: ") << id;

        // variant=10xx xxxx → 첫 nibble은 8,9,a,b 중 하나여야 한다.
        const char v = id[19];
        const bool ok = (v == '8' || v == '9' || v == 'a' || v == 'b');
        EXPECT_TRUE(ok) << j2::string::to_console_encoding(u8"바리언트 nibble(19번째)은 8/9/a/b 여야 합니다: ") << id;
    }
}

TEST(UuidV4, ManyNoDuplicate) {
    // 실행 시간과 메모리를 고려해 20,000개로 검증한다.
    // 실제 충돌 확률은 극히 낮다(2^122 규모의 공간).
    constexpr int kN = 20000;
    std::unordered_set<std::string> seen;
    seen.reserve(kN * 2);

    for (int i = 0; i < kN; ++i) {
        auto id = UuidV4::generate();
        auto [it, inserted] = seen.insert(std::move(id));
        if (!inserted) {
            ADD_FAILURE() << j2::string::to_console_encoding(u8"중복 UUID 발견: ") << *it;
            break;
        }
    }

    EXPECT_EQ(static_cast<int>(seen.size()), kN)
        << j2::string::to_console_encoding(u8"중복으로 인해 수량이 줄었습니다.");
}

TEST(UuidV4, MultithreadedNoDuplicate) {
    // 멀티스레드 환경에서 각 스레드가 독립적으로 UUID를 생성한다.
    // 각 스레드 결과를 합쳐 중복 여부를 확인한다.
    const int threads = std::max(4u, std::thread::hardware_concurrency());
    const int per_thread = 5000;  // 총 threads * per_thread 생성
    std::vector<std::vector<std::string>> buckets(threads);

    std::vector<std::thread> ths;
    ths.reserve(threads);

    for (int t = 0; t < threads; ++t) {
        ths.emplace_back([&, t] {
            auto& vec = buckets[t];
            vec.reserve(per_thread);
            for (int i = 0; i < per_thread; ++i) {
                vec.emplace_back(UuidV4::generate());
            }
            });
    }

    for (auto& th : ths) th.join();

    // 메인 스레드에서 집계하여 중복 확인
    std::unordered_set<std::string> seen;
    seen.reserve(threads * per_thread * 2);

    for (const auto& vec : buckets) {
        for (const auto& id : vec) {
            auto [it, inserted] = seen.insert(id);
            if (!inserted) {
                ADD_FAILURE() << j2::string::to_console_encoding(u8"멀티스레드 중복 UUID 발견: ") << *it;
                // 계속 진행하여 추가 중복도 탐지 가능
            }
        }
    }

    EXPECT_EQ(static_cast<size_t>(threads * per_thread), seen.size())
        << j2::string::to_console_encoding(u8"멀티스레드 생성에서 중복이 발생했습니다.");
}

// 필요 시 gtest_main을 링크하지 않을 경우에만 사용
// 아래 주석을 해제하면 이 파일 단독으로 실행 가능해집니다.
// #include <gtest/gtest.h>
// int main(int argc, char** argv) {
//   ::testing::InitGoogleTest(&argc, argv);
//   return RUN_ALL_TESTS();
// }
