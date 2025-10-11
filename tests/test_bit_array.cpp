// test_bit_array.cpp
// GoogleTest 기반 단위 테스트 (비트 단위 처리 주석 확장판)
//
// 이 파일은 bit_array 클래스의 "비트 단위" 동작을 검증합니다.
// - 바이트 내부 비트 순서: [MSB … LSB] (가정)
//   * 하나의 바이트(8비트)에서 가장 왼쪽이 MSB(Most Significant Bit, 최상위 비트), 가장 오른쪽이 LSB(Least Significant Bit, 최하위 비트)라고 가정합니다.
//   * 예: 0b1010'0000 은 MSB가 1, 그 다음 0, 1, 0, …, 마지막 LSB는 0 입니다.
// - 전체 시퀀스 인덱스: index 0 이 "가장 앞(MSB 쪽)" 비트라고 가정합니다.
//   * 즉, data()[0] 바이트의 MSB가 전체 비트열의 index 0 에 대응합니다.
//   * 이 가정은 프로젝트의 bit_array 구현에 따라 달라질 수 있으므로, 만약 구현이 LSB-first 인덱싱이라면 테스트 기대값을 조정해야 합니다.
// - 비트 슬라이싱(get), 병합(merge), 시프트(<<, >>), 연결(+) 등에서 각 연산이 어떤 방향으로 어떤 비트를 이동/복사하는지를 각 테스트에서 단계별로 설명합니다.
//
// ⚠️ 주의: 아래 테스트는 "MSB-first, index 0 == 최상위비트"라는 전제를 사용합니다.
//          구현이 다르면 주석과 기대 결과를 구현에 맞게 바꾸십시오.
//
// 빌드 요구 사항:
//   - C++17 이상
//   - GoogleTest (gtest 또는 gtest_main 링크)
//   - bit_array.hpp 경로는 프로젝트 구조에 맞게 변경하세요.
//
#include <gtest/gtest.h>
#include <cstdint>
#include <vector>
#include <string>

// 프로젝트의 실제 경로로 수정하세요.
#include "j2_library/core/bit_array.hpp"

using j2::core::bit_array;

// ---------------------------------------------
// 유틸: 벡터<bool> 비교 + 디버그 문자열
// ---------------------------------------------

// 벡터<bool> 내용 동일성 검사
static ::testing::AssertionResult VecBoolEq(const std::vector<bool>& a, const std::vector<bool>& b) {
    if (a.size() != b.size())
        return ::testing::AssertionFailure() << "size mismatch: " << a.size() << " vs " << b.size();
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] != b[i]) {
            return ::testing::AssertionFailure() << "bit mismatch at [" << i << "]: " << a[i] << " vs " << b[i];
        }
    }
    return ::testing::AssertionSuccess();
}

// 비트열을 "0101..." 문자열로 변환
static std::string BitsToString(const std::vector<bool>& bits) {
    std::string s;
    s.reserve(bits.size());
    for (bool b : bits) s.push_back(b ? '1' : '0');
    return s;
}

// 바이트 벡터를 "MSB→LSB" 비트열 문자열로 풀어쓰기 (디버깅용)
//   - 각 바이트를 MSB→LSB 순서로 펼칩니다.
static std::string BytesAsBitsMSBFirst(const std::vector<uint8_t>& bytes) {
    std::string s;
    for (uint8_t byte : bytes) {
        for (int bit = 7; bit >= 0; --bit) {
            s.push_back(((byte >> bit) & 0x1) ? '1' : '0');
        }
        s.push_back(' '); // 바이트 경계 가독성
    }
    return s;
}

// ---------------------------------------------
// 테스트 시작
// ---------------------------------------------

TEST(BitArray, BasicConstructionAndSize) {
    // [시나리오]
    // - 바이트 {1100'0000} (=0xC0) 을 3비트로만 해석
    // - MSB-first 가정이므로 index 0 = '1'(MSB), index 1 = '1', index 2 = '0'
    bit_array a1({ 0b11000000 }, 3);
    EXPECT_EQ(a1.size(), 3u);
    EXPECT_EQ(a1.data().size(), 1u);

    // 전체 8비트 사용
    bit_array a2({ 0b11000000 }, 8);
    EXPECT_EQ(a2.size(), 8u);
    EXPECT_EQ(a2.data().size(), 1u);

    // 2바이트(16비트) 사용
    // 바이트열: [1100'0011] [1010'0110]
    //            ^MSB ... LSB^  ^MSB ... LSB^
    bit_array a3({ 0b11000011, 0b10100110 }, 16);
    EXPECT_EQ(a3.size(), 16u);
    EXPECT_EQ(a3.data().size(), 2u);
}

TEST(BitArray, GetSubArray_MSBIndexing) {
    // src: [1100'0011] [1010'0110] (총 16비트)
    // idx:  0 1 2 3 4 5 6 7 | 8 9 10 11 12 13 14 15
    // bit:  1 1 0 0 0 0 1 1 | 1 0 1 0  0  1  1  0     (MSB-first 가정)
    bit_array src({ 0b11000011, 0b10100110 }, 16);

    // 앞의 4비트 [0..3]: 1 1 0 0
    auto sub0 = src.get(0, 4).toArray();
    std::vector<bool> expect0 = { 1,1,0,0 };
    EXPECT_TRUE(VecBoolEq(sub0, expect0)) << "got=" << BitsToString(sub0);

    // 중간 8비트 [4..11]: 0 0 1 1 1 0 1 0  (= 0011 1010)
    // - 구간 해석:
    //   [4]=0, [5]=0, [6]=1, [7]=1, [8]=1, [9]=0, [10]=1, [11]=0
    auto sub1 = src.get(4, 8).toArray();
    std::vector<bool> expect1 = { 0,0,1,1, 1,0,1,0 };
    EXPECT_TRUE(VecBoolEq(sub1, expect1))
        << "src(bytes)=" << BytesAsBitsMSBFirst(src.data())
        << ", sub1=" << BitsToString(sub1);
}

TEST(BitArray, MergeByOffset_MSBIndexing) {
    // 병합(merge): 대상(dst)의 특정 오프셋부터 다른 비트열(ins)을 "위치 그대로" 덮어쓰기
    // dst 초기 상태 (16비트 0): [0000'0000] [0000'0000]
    bit_array dst({ 0x00, 0x00 }, 16);

    // 삽입 소스 ins (8비트): [1111'1100]
    // index(0..7) = 1 1 1 1 1 1 0 0
    bit_array ins({ 0b11111100 }, 8);

    // 오프셋 4부터 ins(8비트)를 복사
    // dst 비트 인덱스 [4..11] 위치에 ins[0..7]가 순서대로 들어갑니다(MSB-first 인덱싱).
    ASSERT_TRUE(dst.merge(ins, 4));

    // 검증: dst[4..11] 구간을 추출하여 ins 비트열과 동일해야 함
    auto merged_slice = dst.get(4, 8).toArray();
    auto ins_bits = ins.toArray();
    EXPECT_TRUE(VecBoolEq(merged_slice, ins_bits))
        << "dst(bytes)=" << BytesAsBitsMSBFirst(dst.data())
        << ", merged_slice=" << BitsToString(merged_slice)
        << ", ins=" << BitsToString(ins_bits);

    // 범위를 벗어나면 실패해야 함
    bit_array tooSmall({ 0x00 }, 4);     // 4비트짜리 대상
    EXPECT_FALSE(tooSmall.merge(ins, 12)); // 오프셋 12부터 8비트는 불가능
}

TEST(BitArray, ConcatOperatorPlus_MSBIndexing) {
    // 연결(+)는 [좌측 비트열 | 우측 비트열] 순서로 이어붙입니다.
    // a: 4비트 1010
    bit_array a({ 0b10100000 }, 4);  // 바이트로는 1010'0000 이지만 size=4 이므로 유효비트는 앞의 4개만
    // b: 4비트 1100
    bit_array b({ 0b11000000 }, 4);
    // c: a+b → 8비트 1010 1100
    bit_array c = a + b;

    EXPECT_EQ(c.size(), 8u);
    EXPECT_TRUE(VecBoolEq(c.get(0, 4).toArray(), a.toArray())); // c[0..3] == a
    EXPECT_TRUE(VecBoolEq(c.get(4, 4).toArray(), b.toArray())); // c[4..7] == b
}

TEST(BitArray, DataAndSetBytes_AllocationAndZeroing) {
    // setBytes(N): 내부 저장 바이트 수를 N으로 재할당하고 모두 0으로 채운다고 가정
    bit_array a;
    a.setBytes(4); // 4바이트 = 32비트 (모두 0)
    EXPECT_EQ(a.size(), 32u);
    for (auto v : a.data()) EXPECT_EQ(v, 0);

    // 다시 1바이트 = 8비트로 축소
    a.setBytes(1);
    EXPECT_EQ(a.size(), 8u);
    ASSERT_EQ(a.data().size(), 1u);
    EXPECT_EQ(a.data()[0], 0);
}

TEST(BitArray, FromBytesAndToArray_BitZeroFill) {
    // from({0x12, 0x34}, 16):
    // 0x12 = 0001'0010, 0x34 = 0011'0100  (MSB→LSB 나열)
    // 총 16비트 그대로 사용
    bit_array a;
    a.from({ 0x12, 0x34 }, 16);
    EXPECT_EQ(a.size(), 16u);
    auto bytes = a.data();
    ASSERT_EQ(bytes.size(), 2u);
    EXPECT_EQ(bytes[0], 0x12);
    EXPECT_EQ(bytes[1], 0x34);

    // setBits(10): 10비트를 확보하며 기본값은 0으로 채워진다고 가정
    bit_array b;
    b.setBits(10);
    auto bits = b.toArray();
    EXPECT_EQ(bits.size(), 10u);
    for (bool v : bits) EXPECT_FALSE(v);
}

TEST(BitArray, ShiftLeftAndRight_MSBIndexing) {
    // a = 8비트 0000 1111
    // idx: 0 1 2 3 4 5 6 7
    // bit: 0 0 0 0 1 1 1 1
    bit_array a({ 0x0F }, 8);

    // << 4 (왼쪽 쉬프트 4): 각 비트를 더 작은 인덱스로 이동시키고, 하위(오른쪽) 비트는 0으로 채움
    // 결과 기대: 1111 0000  (= index 0..3 = 1, index 4..7 = 0)
    auto L = (a << 4).toArray();
    std::vector<bool> expectL = { 1,1,1,1,0,0,0,0 };
    EXPECT_TRUE(VecBoolEq(L, expectL)) << "L=" << BitsToString(L);

    // >> 2 (오른쪽 쉬프트 2): 각 비트를 더 큰 인덱스로 이동시키고, 상위(왼쪽) 비트는 0으로 채움
    // 원본 a 기준 기대: 0000 0011  (= index 6..7 = 1, 나머지 0)
    auto R = (a >> 2).toArray();
    std::vector<bool> expectR = { 0,0,0,0,0,0,1,1 };
    EXPECT_TRUE(VecBoolEq(R, expectR)) << "R=" << BitsToString(R);
}

TEST(BitArray, ReverserRoundtrip_MSBIndexing) {
    // reverser(): 비트열의 순서를 "완전히 뒤집는" 연산이라고 가정
    // 예) [b0 b1 b2 ... bN-1] → [bN-1 ... b2 b1 b0]
    // 라운드트립(두 번 적용)하면 원상 복구되어야 함.
    bit_array a({ 0x34, 0x12 }, 10);
    auto before = a.toArray();
    a.reverser();  // 1차 반전
    a.reverser();  // 2차 반전 → 원복
    auto after = a.toArray();
    EXPECT_TRUE(VecBoolEq(before, after))
        << "before=" << BitsToString(before) << ", after=" << BitsToString(after);
}

// gtest_main 을 링크하지 않는 경우를 대비한 메인 함수 예시 (주석 처리)
// int main(int argc, char** argv) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }
