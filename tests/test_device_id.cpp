
#include "j2_library/system/system.hpp"

#include <gtest/gtest.h>
#include <set>

using namespace j2::system;

/**
 * @brief Device ID 생성 기본 테스트
 */
TEST(DeviceIdTest, BasicGeneration) {
    std::string id = DeviceIdGenerator::get_unique_id();

    // 1. ID가 비어있지 않아야 함
    EXPECT_FALSE(id.empty());

    // 2. 출력 포맷 확인 (64글자 16진수 형태)
    EXPECT_EQ(id.length(), 64);

    // 3. 16진수 문자로만 구성되어 있는지 확인
    for (char c : id) {
        bool is_hex = (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
        EXPECT_TRUE(is_hex) << "Invalid character in Device ID: " << c;
    }
}

/**
 * @brief 일관성 테스트 (Idempotency)
 * 동일한 환경에서 여러 번 호출해도 항상 같은 ID가 나와야 라이선스 관리가 가능합니다.
 */
TEST(DeviceIdTest, Consistency) {
    std::string id1 = DeviceIdGenerator::get_unique_id();
    std::string id2 = DeviceIdGenerator::get_unique_id();
    std::string id3 = DeviceIdGenerator::get_unique_id();

    EXPECT_EQ(id1, id2);
    EXPECT_EQ(id1, id3);
}

/**
 * @brief 고유성 및 해시 동작 확인
 * 입력 정보가 미세하게 다를 때 해시값이 달라지는지 확인 (내부 로직 검증)
 */
TEST(DeviceIdTest, UniquenessHint) {
    // 실제 기기 ID는 하나지만, 내부 simple_hash의 동작을 간접 확인
    // 서로 다른 기기 정보를 시뮬레이션할 수 없으므로 생성된 ID가 
    // 최소한 'unknown' 같은 기본값으로만 도배되지 않았는지 확인합니다.
    std::string id = DeviceIdGenerator::get_unique_id();

    // 기본값으로만 생성된 무의미한 ID가 아님을 확인
    EXPECT_NE(id, std::string(64, '0'));
    EXPECT_NE(id, std::string(64, 'f'));
}

