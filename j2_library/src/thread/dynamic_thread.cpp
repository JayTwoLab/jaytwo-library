#include "j2_library/thread/dynamic_thread.hpp"

namespace j2::thread {

    dynamic_thread::dynamic_thread()
        : running_(false), task_(nullptr),
        interval_(std::chrono::milliseconds(100)) // 기본 100ms
    {
    }

    dynamic_thread::~dynamic_thread() {
        stop(); // 소멸 시 스레드 종료 보장
    }

    void dynamic_thread::setInterval(std::chrono::milliseconds newInterval) {
        interval_ = newInterval;
    }

    bool dynamic_thread::isRunning() const {
        return running_.load(std::memory_order_acquire);
    }

    void dynamic_thread::stop() {
        if (running_.load(std::memory_order_acquire)) {
            running_.store(false, std::memory_order_release);
            if (workerThread_.joinable()) {
                workerThread_.join();
            }
        }
    }

    void dynamic_thread::threadFunction() {
        while (running_.load(std::memory_order_acquire)) {
            if (task_) {
                task_(); // 등록된 작업 실행
            }
            std::this_thread::sleep_for(interval_); // 주기 대기
        }
    }

} // namespace j2::thread
