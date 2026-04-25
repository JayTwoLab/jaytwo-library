// 파일: test_dynamic_thread.cpp
// 목적: j2::thread::dynamic_thread 클래스의 동작을 GoogleTest로 검증
// - 람다/일반 콜러블 실행
// - 멤버 함수 바인딩 실행
// - thread_task 참조/공유포인터(start(thread_task&), start(shared_ptr<thread_task>)) 실행
// - thread_task 파생형(상속) 객체 등록 실행
// - 시작 전 인터벌 동작 확인
// - 중복 start() 방지
// - stop() 멱등성
// - stop() 이후 추가 실행 없음
//
// 주의: 스레드/시간 관련 테스트는 환경에 따라 변동될 수 있으므로, 여유 시간(마진)을 둡니다.

#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <thread>
#include <functional>
#include <memory>
#include <optional>

#include "j2_library/thread/dynamic_thread.hpp"

namespace {

    // 지정된 밀리초 동안 현재 스레드 일시 정지
    static inline void SleepFor(std::chrono::milliseconds ms) {
        std::this_thread::sleep_for(ms);
    }

    // 단순 카운터 클래스 (멤버 함수 바인딩 테스트용)
    struct Counter {
        std::atomic<int> value{ 0 };
        void tick() { value.fetch_add(1, std::memory_order_relaxed); }
    };

    // thread_task 구현체: 단순 카운터
    struct TaskCounter : j2::thread::thread_task {
        std::atomic<int> value{ 0 };
        void performTask() override { value.fetch_add(1, std::memory_order_relaxed); }
    };

    // thread_task 파생형: 카운터 + 스레드 아이디 기록
    struct DerivedTask : j2::thread::thread_task {
        std::atomic<int> value{ 0 };
        std::atomic<bool> saw_thread_id{ false };
        std::optional<std::thread::id> tid;
        int limit = 0; // 0이면 무제한

        explicit DerivedTask(int max_count = 0) : limit(max_count) {}

        void performTask() override {
            if (!saw_thread_id.load(std::memory_order_relaxed)) {
                tid = std::this_thread::get_id();
                saw_thread_id.store(true, std::memory_order_relaxed);
            }
            int old = value.load(std::memory_order_relaxed);
            if (limit == 0 || old < limit) {
                value.fetch_add(1, std::memory_order_relaxed);
            }
        }
    };

} // namespace


TEST(dynamic_thread, RunsLambda) { 
    j2::thread::dynamic_thread dt;
    std::atomic<int> count{ 0 };

    dt.setInterval(std::chrono::milliseconds(20));
    dt.start([&] { count.fetch_add(1, std::memory_order_relaxed); });

    SleepFor(std::chrono::milliseconds(150));
    dt.stop();

    EXPECT_GE(count.load(std::memory_order_relaxed), 5); // 5회 이상 실행되었는지 확인. 5~7회 예상.
}

TEST(dynamic_thread, RunsBoundMemberFunction) {
    j2::thread::dynamic_thread dt;
    Counter c;

    dt.setInterval(std::chrono::milliseconds(30));
    dt.start(&Counter::tick, &c);

    SleepFor(std::chrono::milliseconds(240));
    dt.stop();

    EXPECT_GE(c.value.load(std::memory_order_relaxed), 5); // 5회 이상 실행되었는지 확인. 5~8회 예상.   
}

TEST(dynamic_thread, CanAdjustIntervalBeforeStart) {
    j2::thread::dynamic_thread dt;
    std::atomic<int> count{ 0 };

    dt.setInterval(std::chrono::milliseconds(40));
    dt.start([&] { count.fetch_add(1, std::memory_order_relaxed); });

    SleepFor(std::chrono::milliseconds(210));
    dt.stop();

    EXPECT_GE(count.load(std::memory_order_relaxed), 4); // 4회 이상 실행되었는지 확인. 4~6회 예상.
}

TEST(dynamic_thread, StartWithThreadTaskReference) {
    j2::thread::dynamic_thread dt;
    TaskCounter task;

    dt.setInterval(std::chrono::milliseconds(25));
    dt.start(task); // thread_task& 오버로드

    SleepFor(std::chrono::milliseconds(180));
    dt.stop();

    EXPECT_GE(task.value.load(std::memory_order_relaxed), 5); // 5회 이상 실행되었는지 확인. 5~7회 예상.
}

TEST(dynamic_thread, StartWithThreadTaskSharedPtr) {
    j2::thread::dynamic_thread dt;
    auto sp = std::make_shared<TaskCounter>();

    dt.setInterval(std::chrono::milliseconds(25));
    dt.start(sp); // shared_ptr<thread_task> 오버로드

    SleepFor(std::chrono::milliseconds(180));
    dt.stop();

    EXPECT_GE(sp->value.load(std::memory_order_relaxed), 5); // 5회 이상 실행되었는지 확인. 5~7회 예상.
}

TEST(dynamic_thread, IgnoreDoubleStart) {
    j2::thread::dynamic_thread dt;
    std::atomic<int> count{ 0 };

    dt.setInterval(std::chrono::milliseconds(20));
    dt.start([&] { count.fetch_add(1, std::memory_order_relaxed); });

    // 실행 중 중복 start → 무시되어야 함
    dt.start([&] { count.fetch_add(1000, std::memory_order_relaxed); });

    SleepFor(std::chrono::milliseconds(160));
    dt.stop();

    const int v = count.load(std::memory_order_relaxed);
    EXPECT_LT(v, 1000); // 1000 이상이면 중복 start가 무시되지 않은 것
    EXPECT_GE(v, 4); // 4회 이상 실행되었는지 확인. 4~7회 예상.
}

TEST(dynamic_thread, StopIsIdempotent) {
    j2::thread::dynamic_thread dt;
    std::atomic<int> count{ 0 };

    dt.setInterval(std::chrono::milliseconds(15));
    dt.start([&] { count.fetch_add(1, std::memory_order_relaxed); });

    SleepFor(std::chrono::milliseconds(90));
    dt.stop();  // 1차 멱등성 호출. 여러번 호출해도 문제 없어야 함.
    dt.stop();  // 2차
    dt.stop();  // 3차

    EXPECT_GE(count.load(std::memory_order_relaxed), 3); // 3회 이상 실행되었는지 확인. 3~6회 예상.
}

TEST(dynamic_thread, NoExecutionAfterStop) {
    j2::thread::dynamic_thread dt;
    std::atomic<int> count{ 0 };

    dt.setInterval(std::chrono::milliseconds(10));
    dt.start([&] { count.fetch_add(1, std::memory_order_relaxed); });

    SleepFor(std::chrono::milliseconds(60));
    dt.stop();
    const int afterStop = count.load(std::memory_order_relaxed);

    SleepFor(std::chrono::milliseconds(80));
    EXPECT_EQ(count.load(std::memory_order_relaxed), afterStop); // stop 이후 실행 없음 확인
}

TEST(dynamic_thread, DerivedTaskByReference) {
    j2::thread::dynamic_thread dt;
    DerivedTask task(/*limit=*/0);

    dt.setInterval(std::chrono::milliseconds(20));
    dt.start(task); // thread_task& 오버로드

    SleepFor(std::chrono::milliseconds(140));
    dt.stop();

    EXPECT_GE(task.value.load(std::memory_order_relaxed), 4); // 4회 이상 실행되었는지 확인. 4~6회 예상.
    EXPECT_TRUE(task.saw_thread_id.load(std::memory_order_relaxed)); // 스레드 아이디 기록됨
    ASSERT_TRUE(task.tid.has_value()); // 스레드 아이디 있음
}

TEST(dynamic_thread, DerivedTaskBySharedPtrAndExternalReset) {
    j2::thread::dynamic_thread dt;
    auto sp = std::make_shared<DerivedTask>(/*limit=*/0);

    dt.setInterval(std::chrono::milliseconds(15));
    dt.start(sp);  // 내부에서 shared_ptr 보관

    // 외부 포인터 해제 → 내부 보관분이 수명 보장
    sp.reset();

    SleepFor(std::chrono::milliseconds(90));
    dt.stop();

    // 크래시/데드락 없이 stop() 완료되면 수명 보장 동작으로 간주
    SUCCEED();
}
