
#include <iostream>
#include <string>
#include <filesystem>
#include "j2_library/system/crash_handler.hpp" // 프로젝트 경로에 맞춰 수정

int crash_test_function() {
    // 의도적으로 크래시를 유도하는 함수
    //int* ptr = nullptr; // Null pointer
    //return *ptr; // 이 줄에서 크래시 발생

    std::string path = "invalid_path/new_directory";
    std::filesystem::create_directory(path);
    return 0;
}

int main() {
    std::cout << "--- j2::system::CrashHandler Example Start ---" << std::endl;

    // 1. 크래시 핸들러 초기화
    // 콜백을 등록하여 로그를 파일로 저장하거나 추가 작업을 할 수 있습니다.
    j2::system::CrashHandler::initialize([](const std::string& log_message) {
        std::cout << "\n[User Callback] Crash Detected! Reporting to console...\n";
        std::cout << log_message << std::endl;

        // 여기서 파일 저장 로직을 추가할 수 있습니다.
        // std::ofstream file("crash_log.txt");
        // file << log_message;
        });

    std::cout << "Crash handler initialized. Triggering a crash in 3 seconds..." << std::endl;

    // 2. 의도적인 크래시 유도 (Null Pointer Dereference)
    // 실제 상황에서는 잘못된 메모리 접근 시 즉시 핸들러가 호출됩니다.
    int* invalid_ptr = nullptr;
    // *invalid_ptr = 42;

    std::cout << "Accessing invalid memory now..." << std::endl;
    crash_test_function();
   
    return 0;
}
