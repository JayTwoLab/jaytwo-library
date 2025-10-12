// 파일: test_dynamic_thread.cpp
// 목적: j2::thread::DynamicThread 클래스의 기본 동작 검증(GoogleTest)
// 주의: 시간/스레드 관련 테스트는 환경에 따라 변동될 수 있으므로, 여유 있는 간격과 대기를 둡니다.

#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <thread>
#include <functional>

#include "j2_library/thread/dynamic_thread.hpp"  // DynamicThread API 확인용 포함

using namespace std::chrono_literals;

namespace {

    // 간단 유틸: 충분한 시간 대기
    void SleepFor(std::chrono::milliseconds ms) {
        std::this_thread::sleep_for(ms);
    }

    // 멤버 함수 바인딩 검증용 타입
    struct Counter {
        std::atomic<int> value{ 0 };
        void tick() {
            // 스레드에서 주기적으로 호출될 작업
            value.fetch_add(1, std::memory_order_relaxed);
        }
    };

} // namespace

// 1) 람다 작업으로 시작/정지 및 카운트 증가 검증
TEST(DynamicThread, StartsAndStopsWithLambda) {
    j2::thread::DynamicThread dt;

    std::atomic<int> count{ 0 };

    // 테스트 안정성을 위해 비교적 짧은 주기(20ms)와 충분한 대기(200ms) 사용
    dt.setInterval(20ms);  // 주기 설정 (공개 API)  :contentReference[oaicite:3]{index=3}
    dt.start([&] {
        // 주기적으로 호출될 작업
        count.fetch_add(1, std::memory_order_relaxed);
        });                    // start(Callable&&, Args&&...) 템플릿 사용  :contentReference[oaicite:4]{index=4}

    EXPECT_TRUE(dt.isRunning());  // 실행 상태 확인  :contentReference[oaicite:5]{index=5}
    SleepFor(200ms);

    dt.stop();                    // 스레드 종료 및 join  :contentReference[oaicite:6]{index=6}
    EXPECT_FALSE(dt.isRunning());

    // 200ms 동안 20ms 주기면 최소 여러 번 실행되어야 함(환경 여유치 고려, 하한=5)
    EXPECT_GE(count.load(), 5); 
}

// 2) 멤버 함수 바인딩(&Class::method, thisPtr) 동작 검증
TEST(DynamicThread, RunsBoundMemberFunction) {
    j2::thread::DynamicThread dt;
    Counter c;

    dt.setInterval(30ms);
    dt.start(&Counter::tick, &c);   // std::bind로 결합되어 저장됨  :contentReference[oaicite:7]{index=7}

    SleepFor(240ms);
    dt.stop();

    // 최소한 여러 번 호출되었는지 확인(환경 여유치 반영)
    EXPECT_GE(c.value.load(), 5); // 30ms 주기면 최소 5회 이상 실행되어야 함
}

// 3) start()의 중복 호출 방지 동작 검증
//    - running_이 이미 true면 에러 메시지를 출력하고 리턴하는 구현을 확인(상태는 계속 running)
//    - 별도 스레드가 추가 생성되지 않는지까지는 직접 측정하기 어려우므로, 정상 동작/Count 증가만 확인
TEST(DynamicThread, DoubleStartIsIgnoredWhileRunning) {
    j2::thread::DynamicThread dt;
    std::atomic<int> count{ 0 };

    dt.setInterval(25ms);
    dt.start([&] { count.fetch_add(1, std::memory_order_relaxed); });

    // 실행 중 재차 start() 호출 → 무시되어야 함(표준에러로 경고 출력)  :contentReference[oaicite:8]{index=8}
    dt.start([&] { count.fetch_add(1000, std::memory_order_relaxed); }); // 무시되므로 영향 없어야 함

    SleepFor(200ms);
    dt.stop();

    // 첫 작업만 수행되어야 하므로 1000 단위 증가는 없어야 함
    // (정확히 0 보장은 어려우므로, 합리적 상한선으로 500 미만을 기대)
    EXPECT_LT(count.load(), 500); // 1000 단위 증가가 없어야 함
    EXPECT_GE(count.load(), 3); // 최소 몇 번은 실행되어야 함
}

// 4) stop()의 멱등성(idempotent) 검증: 여러 번 호출해도 안전해야 함
TEST(DynamicThread, StopIsIdempotent) {
    j2::thread::DynamicThread dt;
    std::atomic<int> count{ 0 };

    dt.setInterval(15ms);
    dt.start([&] { count.fetch_add(1, std::memory_order_relaxed); });
    // NOTE: 클래스에서 dt.start()를 한 번 호출해 스레드가 이미 실행 중인 상태에서 다시 start()를 호출하면, 내부 코드가 실행을 막습니다. 따라서 start()에 대한 멱등성 테스트는 의미가 없습니다.

    SleepFor(120ms);
    dt.stop();     // 1차 종료
    dt.stop();     // 2차 종료 (실행 중이 아니면 join 시도 없이 반환)  :contentReference[oaicite:9]{index=9}

    EXPECT_FALSE(dt.isRunning()); // 종료 상태 확인
}

// 5) 최소 동작 연속성: setInterval 조정 후에도 주기 실행이 이어지는지 확인
TEST(DynamicThread, CanAdjustIntervalBeforeStart) {
    j2::thread::DynamicThread dt;
    std::atomic<int> count{ 0 };

    // 시작 전에 주기 설정
    dt.setInterval(40ms);
    dt.start([&] { count.fetch_add(1, std::memory_order_relaxed); });

    SleepFor(210ms);
    dt.stop();

    EXPECT_GE(count.load(), 4); // 40ms 주기면 최소 4회 이상 실행되어야 함
}

