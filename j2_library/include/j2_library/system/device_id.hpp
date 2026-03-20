#pragma once

#include "j2_library/export.hpp"

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <algorithm>

#ifdef _WIN32
    #include <windows.h>
    #include <intrin.h>
#else
    #include <unistd.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <cpuid.h>
#endif

namespace j2 {
    namespace system {

        /**
         * @brief 관리자 권한 없이 장치 고유 ID를 생성하는 클래스
         */
        class J2LIB_API DeviceIdGenerator {
        public:
            /**
             * @brief 현재 기기의 고유 ID(64자 해시)를 반환합니다.
             * @return std::string 64글자의 16진수 문자열
             */
            static std::string get_unique_id();

        private:
            // 내부 보조 함수들
            static std::string get_cpu_info();
            static std::string get_os_machine_id();
            static std::string get_env_info();
            static std::string simple_hash(const std::string& input);
        };

    } // namespace system
} // namespace j2

