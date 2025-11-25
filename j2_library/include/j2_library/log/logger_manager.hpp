#pragma once

#include <string>
#include <memory>
#include <atomic>
#include <thread>
#include <mutex>
#include <filesystem>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <string_view>

#include <cstdlib>
#include <cctype>
#include <cmath>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>  
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/pattern_formatter.h>

// #include <SimpleIni.h>
#include "j2_library/ini/ini.hpp"

#include "j2_library/export.hpp"
#include "j2_library/network/network.hpp"

// INI 기반 spdlog 구성/리로드/디스크 감시/UDP 알림을 제공하는 로거 매니저
namespace j2::log {

class J2LIB_API logger_manager {
public:
    logger_manager();
    ~logger_manager();

    // 초기화.
    // 인자:
    //  defaultConfigPath: 기본 INI 경로(환경 변수로 대체 가능)
    //  sectionName: 로깅 설정 섹션 이름
    //  loggerName: 생성할 로거 이름
    //  envName: 환경 변수 이름(지정 시 해당 변수가 설정된 경우
    //           defaultConfigPath를 대체함)
    // 반환값: 성공 시 true
    bool init(const std::string& defaultConfigPath,
              const std::string& sectionName,
              const std::string& loggerName,
              const std::string& envName = "");

    // 로거 가져오기 (초기화 시 loggerName을 설정한 경우 해당 이름의 로거 반환)
    // 반환값: spdlog 로거 포인터
    std::shared_ptr<spdlog::logger> getLogger() const;

    // 설정 파일(INI)이 변경된 경우 리로드
    bool reloadIfChanged();

    // 설정 파일(INI) 자동 리로드 시작/중지
    bool startAutoReload(unsigned interval_sec = 60);

    // 설정 파일(INI) 자동 리로드 중지
    void stopAutoReload();

private:
    bool loadConfig(bool readAutoReload);
    void applySoftSettings();
    void applyHardSettingsIfNeeded(
        bool old_enableConsole,
        bool old_enableFileAll,
        bool old_enableFileAlerts,
        const std::string& old_allPath,
        const std::string& old_alertsPath,
        std::size_t old_allMaxSize,
        std::size_t old_allMaxFiles,
        std::size_t old_alertMaxSize,
        std::size_t old_alertMaxFiles);
    static void ensureParentDir(const std::string& path);
    bool toBool(const std::string& val, bool default_val) const;
    std::string toLower(const std::string& s) const;
    std::size_t parseSizeBytes(const std::string& s, std::size_t default_val) const;
    spdlog::level::level_enum parseLevel(const std::string& s,
                                         spdlog::level::level_enum def) const;

    // 디스크 감시 + UDP 알림
    void checkDiskAndAct();
    bool sendUdpAlert(const std::string& msg);
    std::string buildUdpMessage(const std::string& tmpl,
                                const std::string& path,
                                unsigned long long availBytes,
                                long double ratioPercent) const;
    static void replaceAll(std::string& s, const std::string& from, const std::string& to);

private:
    // INI/상태
    std::string iniPath_;
    std::string logSection_ = "Log";
    std::string loggerName_;
    // CSimpleIniA ini_;
    j2::ini::Ini ini_; 

    bool utcMode_ = false;

    bool enableConsole_    = true;
    bool enableFileAll_    = true;
    bool enableFileAlerts_ = true;

    spdlog::level::level_enum consoleMin_ = spdlog::level::trace;
    spdlog::level::level_enum allFileMin_ = spdlog::level::trace;
    spdlog::level::level_enum alertsMin_  = spdlog::level::warn;
    spdlog::level::level_enum loggerMin_  = spdlog::level::trace;
    spdlog::level::level_enum flushOn_    = spdlog::level::warn;

    std::size_t flushEverySec_ = 1; // 디폴트 1초 (시간 단위로 파일 플러시 간격 설정)

    // 기본값은 INI에서 덮어씀(필요 시 %Z를 패턴에 넣어 사용 가능)
    std::string patternConsole_ = "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v";
    std::string patternFile_    = "[%Y-%m-%d %H:%M:%S.%e] [%l] [%t] %v";

    std::string allPath_    = "logs/all.log"; // 모든 로그용 로그 파일 경로
    std::string alertsPath_ = "logs/alerts.log"; // 경고 이상 로그용 로그 파일 경로

    std::size_t allMaxSize_ = 100 * 1024 * 1024; // 모든 로그용 로그 파일 크기 (디폴트 100 MB)
    std::size_t allMaxFiles_ = 5; // 모든 로그용 로그 백업 파일 개수
    std::size_t alertMaxSize_ = 100 * 1024 * 1024; // 경고 이상 로그용 로그 파일 크기 (디폴트 100 MB) 
    std::size_t alertMaxFiles_ = 10; // 경고 이상 로그용 로그 백업 파일 개수

    // 디스크 감시(단일)
    bool        diskGuardEnable_ = true;
    std::string diskRoot_;
    double      diskMinFreeRatio_ = 5.0; // 5 percent

    // UDP 알림(Boost.Asio)
    std::string udpIp_;
    std::uint16_t udpPort_ = 0;
    unsigned    udpIntervalSec_ = 60; // 60 secondss
    std::string udpMessageTmpl_ = "DISK LOW: path={path} free={avail_bytes}B ({ratio}%)";
    std::chrono::steady_clock::time_point lastUdpSent_{};

    // 파일 싱크 분리 상태
    bool fileSinksDetachedForDisk_ = false;

    // 로거/싱크
    std::shared_ptr<spdlog::logger> logger_;
    std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> consoleSink_;
    std::shared_ptr<spdlog::sinks::rotating_file_sink_mt> allSink_;
    std::shared_ptr<spdlog::sinks::rotating_file_sink_mt> alertsSink_;
    std::shared_ptr<spdlog::sinks::dist_sink_mt> distSink_;

    // 공통 상태
    std::filesystem::file_time_type lastWriteTime_{};
    std::atomic<bool> autoReloadRunning_{false};
    std::thread autoReloadThread_;
    unsigned autoReloadIntervalSec_{60};
    mutable std::mutex mu_;
};

} // namespace namespace j2::log
