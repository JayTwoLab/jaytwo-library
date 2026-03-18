// tests/test_crypt.cpp
// GoogleTest 기반 AES 암복호화 기본 동작 검증
//
// 빌드: 프로젝트의 기존 테스트 설정(GTest)을 사용하므로 별도 main 정의 불필요

#include <gtest/gtest.h>
#include <string>

#include "j2_library/crypt/cipher.hpp"

TEST(Crypt, EncryptDecrypt_Basic) {
    j2::crypt::cipher cipher;
    cipher.set_key("jaytwo-2026-key");
    // cipher.set_key("0123456789123456"); // 16 bytes key for AES-128

    const std::string original = "Standard AES-128 Implementation Test!";
    const std::string encrypted = cipher.encrypt(original);

    // 암호화 결과는 비어있지 않아야 하고, 16바이트 블록이 2자리 헥스 문자열로 표현되므로 짝수 길이여야 한다.
    ASSERT_FALSE(encrypted.empty());
    EXPECT_EQ(0u, encrypted.size() % 2u);

    const std::string decrypted = cipher.decrypt(encrypted);
    EXPECT_EQ(original, decrypted);
}
