
#include "j2_library/thread/dynamic_thread.hpp"

namespace j2::thread {

void DynamicThread::threadFunction() {
    while (running_.load()) {
        if (task_) {
            task_(); // Performing a task
        }
        std::this_thread::sleep_for(interval_); // Adjusting the Job Interval
    }
}

DynamicThread::DynamicThread()
 : running_(false), task_(nullptr),
    interval_(100) // Set default interval of 100 ms
{
} 

DynamicThread::~DynamicThread() {
    stop(); // End Thread on Object Destruction
}

// Operation interval setting function
void DynamicThread::setInterval(std::chrono::milliseconds newInterval) {
    interval_ = newInterval;
}

// Stop Threads
void DynamicThread::stop() {
    if (running_.load()) {
        running_.store(false);
        if (workerThread_.joinable()) {
            workerThread_.join(); // Wait for the thread to end
        }
    }
}

// Verify that it is running
bool DynamicThread::isRunning() const {
    return running_.load();
}

} // namespace j2::thread
