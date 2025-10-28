
#include <iostream>
#include <thread>
#include <chrono>

#include <spdlog/spdlog.h>

#include "j2_library/j2_library.hpp"

#ifndef SPDLOG_ACTIVE_LEVEL
    #define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif

#define hname "hello_logger"

#define ht(...) SPDLOG_LOGGER_TRACE   (spdlog::get(hname), __VA_ARGS__)  // trace
#define hd(...) SPDLOG_LOGGER_DEBUG   (spdlog::get(hname), __VA_ARGS__)  // debug
#define hi(...) SPDLOG_LOGGER_INFO    (spdlog::get(hname), __VA_ARGS__)  // info
#define hw(...) SPDLOG_LOGGER_WARN    (spdlog::get(hname), __VA_ARGS__)  // warn
#define he(...) SPDLOG_LOGGER_ERROR   (spdlog::get(hname), __VA_ARGS__)  // error
#define hc(...) SPDLOG_LOGGER_CRITICAL(spdlog::get(hname), __VA_ARGS__)  // critical

int main() {
    // 기본 설정 파일명을 j2_logger_manager_config.ini 로 변경

    // std::string defaultConfig = "j2_logger_manager_config.ini";
    std::string defaultConfig = "C:\\workspace\\dev\\github\\LoggerManager\\j2_logger_manager_config_korean.ini";
    // std::string defaultConfig = "C:\\not_exist.ini";

    std::string sectionName = "Log";
    std::string loggerName = hname;

    std::string envName = ""; // 환경 변수로 INI 경로 설정하지 않으면 "" 사용
    // std::string envName       = "LOG_MANAGER_CONFIG_PATH";

    j2::log::logger_manager logMgr;
    if (!logMgr.init(defaultConfig, sectionName, loggerName, envName)) {
        return 1;
    }

    while (true) {
        ht("trace message");
        hd("debug message");
        hi("info message");
        hw("warn message");
        he("error message");
        hc("critical message");

        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
    return 0;
}
