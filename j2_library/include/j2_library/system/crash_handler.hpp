#pragma once

#include "j2_library/export.hpp"

#include <string>
#include <functional>
#include <vector>

namespace j2 {
    namespace system {

        /**
         * @brief 크래시 발생 시 실행될 콜백 함수 타입
         * @param log_message 크래시 원인 및 콜 스택 정보가 포함된 전체 로그
         */
        using CrashCallback = std::function<void(const std::string& log_message)>;

        class J2LIB_API CrashHandler {
        public:
            /**
             * @brief 크래시 핸들러를 시스템에 등록합니다.
             * @param callback 크래시 발생 시 실행할 커스텀 로직 (파일 저장, 서버 전송 등)
             */
            static void initialize(CrashCallback callback = nullptr);

            /**
             * @brief 현재 시점의 콜 스택(Call Stack) 정보를 문자열 리스트로 가져옵니다.
             * @return std::vector<std::string> 스택 프레임별 정보 (함수명, 주소 등)
             */
            static std::vector<std::string> get_stack_trace();

        private:
            // OS별 네이티브 핸들러
#ifdef _WIN32
            static long __stdcall windows_exception_handler(struct _EXCEPTION_POINTERS* info);
#else
            static void posix_signal_handler(int sig);
#endif

            // C++ terminate 핸들러
            static void cxx_terminate_handler();

            static CrashCallback s_callback;
            static void handle_crash(const std::string& type, const std::string& reason);
        };

    } // namespace system
} // namespace j2


