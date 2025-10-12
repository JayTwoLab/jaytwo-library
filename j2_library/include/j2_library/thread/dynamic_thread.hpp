#pragma once

#include <iostream>
#include <thread>
#include <atomic>
#include <functional>
#include <chrono>

#include "j2_library/export.hpp" 

namespace j2::thread {

J2LIB_API class DynamicThread {
public:
    DynamicThread();
    ~DynamicThread();

    // Operation interval setting function
    void setInterval(std::chrono::milliseconds newInterval);

    // Start Thread (member or regular functions can be registered)
    template <typename Callable, typename... Args>
    void start(Callable&& func, Args&&... args) {
        if (running_.load()) {
            std::cerr << "Thread is already running.\n";
            return;
        }

        // Use std::bind to combine a member function or normal function with a factor
        task_ = std::bind(std::forward<Callable>(func), std::forward<Args>(args)...);
        running_.store(true);
        workerThread_ = std::thread(&DynamicThread::threadFunction, this);
    }
    // NOTE: 템플릿 함수는 호출될 때 컴파일러가 인스턴스를 생성(인스턴스화) 해야 합니다.
    // cpp 파일 안에만 정의가 있으면, 그.cpp를 컴파일하는 번역 단위에서만 인스턴스가 만들어집니다.
    // 따라서, 템플릿 함수는 헤더 파일에 정의를 포함시켜야 합니다.

    // Stop Threads
    void stop();

    // Verify that it is running
    bool isRunning() const;

protected:
    void threadFunction();

    std::thread workerThread_;          // Thread Object
    std::atomic<bool> running_;         // Thread execution status flag
    std::function<void()> task_;        // Action Functions to Run
    std::chrono::milliseconds interval_; // Job interval
};

// Multi-threaded Task Base Classes
J2LIB_API class ThreadTask {
protected:
    virtual void performTask() = 0;
};

} // namespace j2::thread
