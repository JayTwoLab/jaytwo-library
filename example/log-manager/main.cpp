
#include <iostream>
#include <thread>
#include <chrono>
#include <filesystem>

#include <spdlog/spdlog.h>

#include "j2_library/j2_library.hpp"

#ifndef SPDLOG_ACTIVE_LEVEL
    #define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif

#define hello_logger_name "hello_logger"

#define ht(...) SPDLOG_LOGGER_TRACE   (spdlog::get(hello_logger_name), __VA_ARGS__)  // trace
#define hd(...) SPDLOG_LOGGER_DEBUG   (spdlog::get(hello_logger_name), __VA_ARGS__)  // debug
#define hi(...) SPDLOG_LOGGER_INFO    (spdlog::get(hello_logger_name), __VA_ARGS__)  // info
#define hw(...) SPDLOG_LOGGER_WARN    (spdlog::get(hello_logger_name), __VA_ARGS__)  // warn
#define he(...) SPDLOG_LOGGER_ERROR   (spdlog::get(hello_logger_name), __VA_ARGS__)  // error
#define hc(...) SPDLOG_LOGGER_CRITICAL(spdlog::get(hello_logger_name), __VA_ARGS__)  // critical

int main() {
    // 기본 설정 파일명을 j2_logger_manager_config.ini 로 변경
   
    std::string cmakePath = CMAKE_SOURCE_DIR_PATH; // CMake에서 전달된 매크로 → 문자열
    std::filesystem::path basePath(cmakePath); // filesystem::path 로 감싸기
    std::filesystem::path filePath = basePath / "j2_logger_manager_config_korean.ini"; // 연산자 / 로 하위 경로 추가
    std::string filePathStr = filePath.string(); // 경로를 문자열로 변환

    std::string sectionName = "Log"; // INI 파일의 섹션 이름
    std::string loggerName = hello_logger_name; // spdlog 에서 사용할 로거 이름

    std::string envName = ""; // OS 환경 변수를 이용하여 INI 경로를 설정하지 않으면 값을 ""으로 설정
    // std::string envName = "LOG_MANAGER_CONFIG_PATH"; // LOG_MANAGER_CONFIG_PATH 환경변수에 값이 있는 경우, 해당 값을 INI 파일 경로로 간주하여 파일을 연다. 

    j2::log::logger_manager logMgr;
    if (!logMgr.init(filePathStr, sectionName, loggerName, envName)) {
        return 1; // 초기화 실패 시 종료
    }

    // 로그 레벨 별 메시지를 계속 출력
    while (true) {
        ht("trace message");
        hd("debug message");
        hi("info message : {0} {1}", 123, "Hello");
        hw("warn message");
        he("error message");
        hc("critical message");

        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
    return 0;
}
