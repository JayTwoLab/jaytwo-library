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
    j2::broker::object_broker::clear(); // 테스트 격리를 위해 중개자 초기화

    auto inst = std::make_shared<Foo>(42); // Foo{42} 인스턴스 생성
    j2::broker::object_broker::register_instance<Foo>(inst); // 기본 이름으로 등록

    auto got = j2::broker::object_broker::get<Foo>(); // 기본 이름으로 조회
    ASSERT_NE(got, nullptr); // 조회된 인스턴스가 nullptr이 아닌지 확인
    EXPECT_EQ(got->value, 42); // 조회된 인스턴스의 value가 42인지 확인

    j2::broker::object_broker::clear();
}

TEST(ObjectBroker, RegisterAndGetNamed) {
    j2::broker::object_broker::clear(); // 테스트 격리를 위해 중개자 초기화

    auto defaultInst = std::make_shared<Foo>(1); // Foo{1} 인스턴스 생성
    auto namedInst = std::make_shared<Foo>(7); // Foo{7} 인스턴스 생성

    j2::broker::object_broker::register_instance<Foo>(defaultInst); // 기본 이름으로 등록
    j2::broker::object_broker::register_instance<Foo>("special", namedInst); // "special" 이름으로 등록

    auto gotDefault = j2::broker::object_broker::get<Foo>();
    auto gotNamed = j2::broker::object_broker::get<Foo>("special");

    ASSERT_NE(gotDefault, nullptr); // 조회된 기본 인스턴스가 nullptr이 아닌지 확인
    ASSERT_NE(gotNamed, nullptr); // 조회된 네임드 인스턴스가 nullptr이 아닌지 확인
    EXPECT_EQ(gotDefault->value, 1); // 기본 인스턴스의 value가 1인지 확인
    EXPECT_EQ(gotNamed->value, 7); // 네임드 인스턴스의 value가 7인지 확인

    j2::broker::object_broker::clear();
}

TEST(ObjectBroker, UnregisterInstance) {
    j2::broker::object_broker::clear(); // 테스트 격리를 위해 중개자 초기화

    auto namedInst = std::make_shared<Foo>(99); // Foo{99} 인스턴스 생성
    j2::broker::object_broker::register_instance<Foo>("to_remove", namedInst); // "to_remove" 이름으로 등록

    auto got = j2::broker::object_broker::get<Foo>("to_remove");
    ASSERT_NE(got, nullptr); // 조회된 인스턴스가 nullptr이 아닌지 확인
    EXPECT_EQ(got->value, 99); // 조회된 인스턴스의 value가 99인지 확인

    j2::broker::object_broker::unregister_instance<Foo>("to_remove"); // "to_remove" 이름으로 등록된 인스턴스 제거
    auto after = j2::broker::object_broker::get<Foo>("to_remove"); // 제거 후 다시 조회
    EXPECT_EQ(after, nullptr); // 제거되었기 때문에 인스턴스는 nullptr 이다.

    j2::broker::object_broker::clear(); 
}

TEST(ObjectBroker, ClearRemovesAll) {
    j2::broker::object_broker::clear(); // 테스트 격리를 위해 중개자 초기화

    auto a = std::make_shared<Foo>(5); // Foo{5} 인스턴스 생성
    auto b = std::make_shared<Foo>(6); // Foo{6} 인스턴스 생성

    j2::broker::object_broker::register_instance<Foo>(a); // 기본 이름으로 등록
    j2::broker::object_broker::register_instance<Foo>("other", b); // "other" 이름으로 등록

    EXPECT_NE(j2::broker::object_broker::get<Foo>(), nullptr); // 기본 이름으로 등록된 인스턴스가 존재하는지 확인
    EXPECT_NE(j2::broker::object_broker::get<Foo>("other"), nullptr); // "other" 이름으로 등록된 인스턴스가 존재하는지 확인

    j2::broker::object_broker::clear(); // 모든 인스턴스 제거

    EXPECT_EQ(j2::broker::object_broker::get<Foo>(), nullptr); // 기본 이름으로 등록된 인스턴스가 제거되었는지(nullptr) 확인
    EXPECT_EQ(j2::broker::object_broker::get<Foo>("other"), nullptr); // "other" 이름으로 등록된 인스턴스가 제거되었는지(nullptr) 확인
}

TEST(ObjectBroker, SameInstanceMultipleNames) {
    j2::broker::object_broker::clear(); // 테스트 격리를 위해 중개자 초기화

    // 동일한 shared_ptr 인스턴스를 여러 이름으로 등록
    auto sharedInst = std::make_shared<Foo>(123); // Foo{123} 인스턴스 생성
    j2::broker::object_broker::register_instance<Foo>(sharedInst); // 기본 이름으로 등록
    j2::broker::object_broker::register_instance<Foo>("alias_a", sharedInst); // "alias_a" 이름으로 등록
    j2::broker::object_broker::register_instance<Foo>("alias_b", sharedInst); // "alias_b" 이름으로 등록

    // 각 이름으로 조회했을 때 동일한 포인터가 반환되는지 확인
    auto gotDefault = j2::broker::object_broker::get<Foo>(); // 기본 이름으로 조회
    auto gotA = j2::broker::object_broker::get<Foo>("alias_a"); // "alias_a" 이름으로 조회
    auto gotB = j2::broker::object_broker::get<Foo>("alias_b"); // "alias_b" 이름으로 조회

    ASSERT_NE(gotDefault, nullptr);
    ASSERT_NE(gotA, nullptr);
    ASSERT_NE(gotB, nullptr);

    // 값과 포인터 동일성 검사
    EXPECT_EQ(gotDefault->value, 123); // 기본 이름으로 조회된 인스턴스의 value가 123인지 확인
    EXPECT_EQ(gotA->value, 123); // "alias_a" 이름으로 조회된 인스턴스의 value가 123인지 확인
    EXPECT_EQ(gotB->value, 123); // "alias_b" 이름으로 조회된 인스턴스의 value가 123인지 확인

    // 같은 shared_ptr 객체를 가리키는지(포인터 동일성) 확인
    EXPECT_EQ(gotDefault, gotA);
    EXPECT_EQ(gotA, gotB);

    j2::broker::object_broker::clear();
}

TEST(ObjectBroker, ConfigServiceExample) {
    j2::broker::object_broker::clear(); // 테스트 격리를 위해 중개자 초기화

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

TEST(ObjectBroker, MultithreadedUsage) {
    // 목적: 여러 스레드가 동시에 같은 shared_ptr을 서로 다른 이름으로 등록하고 조회할 때
    //        object_broker가 스레드 안전하게 동작하는지 확인.
    const int thread_count = 8;
    const int iterations = 200;

    auto sharedInst = std::make_shared<Foo>(777);

    std::atomic<int> success_count{ 0 };
    std::atomic<int> fail_count{ 0 };

    std::vector<std::thread> workers;
    workers.reserve(thread_count);

    // 각 스레드는 고유 이름으로 동일한 shared_ptr을 등록하고 여러번 조회함
    for (int i = 0; i < thread_count; ++i) {
        workers.emplace_back([i, iterations, &sharedInst, &success_count, &fail_count]() {
            const std::string name = "worker_" + std::to_string(i);

            // 등록
            j2::broker::object_broker::register_instance<Foo>(name, sharedInst);

            // 여러번 조회
            for (int k = 0; k < iterations; ++k) {
                auto got = j2::broker::object_broker::get<Foo>(name);
                if (got && got == sharedInst && got->value == 777) {
                    ++success_count;
                }
                else {
                    ++fail_count;
                }

                // 잠깐 대기하여 스케줄링 간섭을 유도
                std::this_thread::sleep_for(std::chrono::microseconds(50));
            }
            });
    }

    for (auto& t : workers) {
        t.join();
    }

    EXPECT_EQ(fail_count.load(), 0);
    EXPECT_EQ(success_count.load(), thread_count * iterations);

    // 이제 동시에 모든 이름을 제거하는 작업을 수행
    std::vector<std::thread> removers;
    removers.reserve(thread_count);
    for (int i = 0; i < thread_count; ++i) {
        removers.emplace_back([i]() {
            const std::string name = "worker_" + std::to_string(i);
            // 여러 번 시도해도 안전해야 함
            j2::broker::object_broker::unregister_instance<Foo>(name);
            });
    }
    for (auto& t : removers) {
        t.join();
    }

    // 제거 후에는 모든 이름에 대해 nullptr 이어야 함
    for (int i = 0; i < thread_count; ++i) {
        const std::string name = "worker_" + std::to_string(i);
        EXPECT_EQ(j2::broker::object_broker::get<Foo>(name), nullptr);
    }
}


// one thread registers the instance, multiple reader threads access it afterward.
TEST(ObjectBroker, SingleRegistrarMultiReaders) {
    j2::broker::object_broker::clear();

    const int reader_count = 8;
    const int iterations = 500;
    auto sharedInst = std::make_shared<Foo>(314); // Foo{314} 인스턴스 생성

    std::atomic<bool> registered{ false };
    std::atomic<int> success_count{ 0 };
    std::atomic<int> fail_count{ 0 };

    // Start reader threads: they wait until registrar sets `registered` then perform reads.
    std::vector<std::thread> readers;
    readers.reserve(reader_count);
    for (int i = 0; i < reader_count; ++i) {
        readers.emplace_back([&registered, &sharedInst, &success_count, &fail_count, iterations]() {
            // wait until registrar registers the instance
            while (!registered.load(std::memory_order_acquire)) {
                std::this_thread::yield();
            }

            // perform multiple concurrent reads
            for (int k = 0; k < iterations; ++k) {
                auto got = j2::broker::object_broker::get<Foo>();
                if (got && got == sharedInst && got->value == 314) {
                    ++success_count;
                }
                else {
                    ++fail_count;
                }
                std::this_thread::sleep_for(std::chrono::microseconds(5));
            }
            });
    }

    // Registrar thread: registers the instance once
    std::thread registrar([&sharedInst, &registered]() {
        // small delay to ensure reader threads are started and waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        j2::broker::object_broker::register_instance<Foo>(sharedInst);
        registered.store(true, std::memory_order_release);
        });

    registrar.join();
    for (auto& t : readers) {
        t.join();
    }

    // All reads after registration must succeed
    EXPECT_EQ(fail_count.load(), 0);
    EXPECT_EQ(success_count.load(), reader_count * iterations);

    j2::broker::object_broker::clear();
}


// Main thread에서 한 번 등록하고, 두 개의 reader 쓰레드가 접근하는 간단한 테스트
TEST(ObjectBroker, MainRegisters_TwoReadersAccess) {
    j2::broker::object_broker::clear();

    // 메인 쓰레드에서 객체 인스턴스 등록
    auto a = std::make_shared<Foo>(42); // Foo{42} 인스턴스 생성
    j2::broker::object_broker::register_instance<Foo>(a); // 기본 이름으로 등록

    const int reads_per_thread = 1000;
    std::atomic<int> success_count{ 0 };
    std::atomic<int> fail_count{ 0 };

    // reader 쓰레드 1
    std::thread reader1([&]() {
        for (int i = 0; i < reads_per_thread; ++i) {
            auto p = j2::broker::object_broker::get<Foo>(); // 기본 이름으로 조회
            if (p && // 조회된 인스턴스가 nullptr이 아닌지 확인
                p == a && // 조회된 인스턴스가 등록된 인스턴스와 동일한 포인터인지 확인
                p->value == 42) { // 조회된 인스턴스의 value가 42인지 확인
                ++success_count;
            }
            else {
                ++fail_count;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
        });

    // reader 쓰레드 2
    std::thread reader2([&]() {
        for (int i = 0; i < reads_per_thread; ++i) {
            auto p = j2::broker::object_broker::get<Foo>(); // 기본 이름으로 조회
            if (p && // 조회된 인스턴스가 nullptr이 아닌지 확인
                p == a && // 조회된 인스턴스가 등록된 인스턴스와 동일한 포인터인지 확인
                p->value == 42) { // 조회된 인스턴스의 value가 42인지 확인
                ++success_count;
            }
            else {
                ++fail_count;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
        });

    reader1.join(); // reader1이 완료될 때까지 대기
    reader2.join(); // reader2가 완료될 때까지 대기

    // 모든 읽기가 성공했는지 검증
    EXPECT_EQ(fail_count.load(), 0);
    EXPECT_EQ(success_count.load(), reads_per_thread * 2);

    j2::broker::object_broker::clear();
}
