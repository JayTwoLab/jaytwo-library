#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <iostream>

#include "j2_library/broker/object_broker.hpp"

namespace {

struct Foo {
    explicit Foo(int v)
        : value(v) {}
    int value;
};

class config_service {
public:
    void load() { std::cout << "success to load configuration" << std::endl; }
};

} // namespace

TEST(ObjectBroker, RegisterAndGetDefault) {
    j2::broker::object_broker::clear();

    auto inst = std::make_shared<Foo>(42);
    j2::broker::object_broker::register_instance<Foo>(inst);

    auto got = j2::broker::object_broker::get<Foo>();
    ASSERT_NE(got, nullptr);
    EXPECT_EQ(got->value, 42);

    j2::broker::object_broker::clear();
}

TEST(ObjectBroker, RegisterAndGetNamed) {
    j2::broker::object_broker::clear();

    auto defaultInst = std::make_shared<Foo>(1);
    auto namedInst = std::make_shared<Foo>(7);

    j2::broker::object_broker::register_instance<Foo>(defaultInst);
    j2::broker::object_broker::register_instance<Foo>("special", namedInst);

    auto gotDefault = j2::broker::object_broker::get<Foo>();
    auto gotNamed = j2::broker::object_broker::get<Foo>("special");

    ASSERT_NE(gotDefault, nullptr);
    ASSERT_NE(gotNamed, nullptr);
    EXPECT_EQ(gotDefault->value, 1);
    EXPECT_EQ(gotNamed->value, 7);

    j2::broker::object_broker::clear();
}

TEST(ObjectBroker, UnregisterInstance) {
    j2::broker::object_broker::clear();

    auto namedInst = std::make_shared<Foo>(99);
    j2::broker::object_broker::register_instance<Foo>("to_remove", namedInst);

    auto got = j2::broker::object_broker::get<Foo>("to_remove");
    ASSERT_NE(got, nullptr);
    EXPECT_EQ(got->value, 99);

    j2::broker::object_broker::unregister_instance<Foo>("to_remove");
    auto after = j2::broker::object_broker::get<Foo>("to_remove");
    EXPECT_EQ(after, nullptr);

    j2::broker::object_broker::clear();
}

TEST(ObjectBroker, ClearRemovesAll) {
    j2::broker::object_broker::clear();

    auto a = std::make_shared<Foo>(5);
    auto b = std::make_shared<Foo>(6);

    j2::broker::object_broker::register_instance<Foo>(a);
    j2::broker::object_broker::register_instance<Foo>("other", b);

    EXPECT_NE(j2::broker::object_broker::get<Foo>(), nullptr);
    EXPECT_NE(j2::broker::object_broker::get<Foo>("other"), nullptr);

    j2::broker::object_broker::clear();

    EXPECT_EQ(j2::broker::object_broker::get<Foo>(), nullptr);
    EXPECT_EQ(j2::broker::object_broker::get<Foo>("other"), nullptr);
}

TEST(ObjectBroker, SameInstanceMultipleNames) {
    j2::broker::object_broker::clear();

    // 동일한 shared_ptr 인스턴스를 여러 이름으로 등록
    auto sharedInst = std::make_shared<Foo>(123);
    j2::broker::object_broker::register_instance<Foo>(sharedInst);                   // "__default__"
    j2::broker::object_broker::register_instance<Foo>("alias_a", sharedInst);
    j2::broker::object_broker::register_instance<Foo>("alias_b", sharedInst);

    // 각 이름으로 조회했을 때 동일한 포인터가 반환되는지 확인
    auto gotDefault = j2::broker::object_broker::get<Foo>();
    auto gotA = j2::broker::object_broker::get<Foo>("alias_a");
    auto gotB = j2::broker::object_broker::get<Foo>("alias_b");

    ASSERT_NE(gotDefault, nullptr);
    ASSERT_NE(gotA, nullptr);
    ASSERT_NE(gotB, nullptr);

    // 값과 포인터 동일성 검사
    EXPECT_EQ(gotDefault->value, 123);
    EXPECT_EQ(gotA->value, 123);
    EXPECT_EQ(gotB->value, 123);

    // 같은 shared_ptr 객체를 가리키는지(포인터 동일성) 확인
    EXPECT_EQ(gotDefault, gotA);
    EXPECT_EQ(gotA, gotB);

    j2::broker::object_broker::clear();
}

TEST(ObjectBroker, ConfigServiceExample) {
    j2::broker::object_broker::clear();

    // 1. 등록 (register_instance)
    auto service = std::make_shared<config_service>();
    j2::broker::object_broker::register_instance<config_service>(service);

    // 2. 조회 (get) 및 동작 호출 검사 (표준 출력 캡처)
    auto current_service = j2::broker::object_broker::get<config_service>();
    ASSERT_NE(current_service, nullptr);

    testing::internal::CaptureStdout();
    current_service->load();
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("success to load configuration"), std::string::npos);

    // 3. 네임드 인스턴스 예시
    j2::broker::object_broker::register_instance<config_service>("backup", std::make_shared<config_service>());
    auto backup_service = j2::broker::object_broker::get<config_service>("backup");
    EXPECT_NE(backup_service, nullptr);

    j2::broker::object_broker::clear();
}
