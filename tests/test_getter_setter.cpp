#include <gtest/gtest.h>
#include <string>
#include "j2_library/macro/setter_getter.hpp"

// 매크로를 이용해 테스트용 구조체 정의
struct TestConfig {
    DEFINE_SETTER_GETTER(std::string, name)
    DEFINE_SETTER_GETTER(int, width)
    DEFINE_SETTER_GETTER(int, height)
};
 
// Test Suite: GetterSetter
TEST(GetterSetter, DefaultInitialization) {
    TestConfig cfg;

    EXPECT_EQ(cfg.name(), "");
    EXPECT_EQ(cfg.width(), 0);
    EXPECT_EQ(cfg.height(), 0);
}

TEST(GetterSetter, SetterAndGetterWork) {
    TestConfig cfg;

    cfg.name("Main Window")
        .width(1280)
        .height(720);

    EXPECT_EQ(cfg.name(), "Main Window");
    EXPECT_EQ(cfg.width(), 1280);
    EXPECT_EQ(cfg.height(), 720);
}

TEST(GetterSetter, RValueSetterWorks) {
    TestConfig cfg;

    std::string temp = "Hello";
    cfg.name(std::move(temp)); // Rvalue 세터 호출

    EXPECT_EQ(cfg.name(), "Hello");
    EXPECT_TRUE(temp.empty());
}

TEST(GetterSetter, NonConstReferenceGetter) {
    TestConfig cfg;
    cfg.width(100);

    cfg.width() = 200;

    EXPECT_EQ(cfg.width(), 200);
}

TEST(GetterSetter, ConstReferenceGetter) {
    TestConfig cfg;
    cfg.height(300);

    const TestConfig& constCfg = cfg;

    EXPECT_EQ(constCfg.height(), 300);
}
