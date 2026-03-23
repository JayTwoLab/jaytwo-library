#pragma once

#include "j2_library/export.hpp"

#include <string>
#include <vector>
#include <mutex>
#include <algorithm>

namespace j2 {
    namespace config {

        /**
         * @brief 설정 파일의 각 줄 정보를 저장하는 구조체
         */
        struct J2LIB_API config_line {
            std::string key;        // 설정 키 (데이터인 경우에만 존재)
            std::string value;      // 설정 값 (데이터인 경우에만 존재)
            std::string raw_text;   // 원본 텍스트 (주석이나 빈 줄 보존용)
            bool is_data;           // 실제 'key=value' 데이터인지 여부
        };

        class J2LIB_API config_manager {
        public:
            /**
             * @brief Singleton 인스턴스 반환
             */
            static config_manager& get_instance() {
                static config_manager instance;
                return instance;
            }

            /**
             * @brief 설정 파일을 읽어 메모리에 로드 (주석 포함)
             */
            bool load(const std::string& file_path);

            /**
             * @brief 현재 메모리의 설정을 파일로 저장 (주석 및 순서 보존)
             */
            bool save(const std::string& file_path);

            /**
             * @brief 문자열 값 가져오기
             */
            std::string get_string(const std::string& key, const std::string& default_val = "");

            /**
             * @brief 정수 값 가져오기
             */
            int get_int(const std::string& key, int default_val = 0);

            /**
             * @brief 불리언 값 가져오기
             */
            bool get_bool(const std::string& key, bool default_val = false);

            /**
             * @brief 값 설정 (기존 키가 있으면 해당 줄 업데이트, 없으면 맨 뒤에 추가)
             */
            void set(const std::string& key, const std::string& value);

        private:
            config_manager() = default;
            ~config_manager() = default;
            config_manager(const config_manager&) = delete;
            config_manager& operator=(const config_manager&) = delete;

            // 내부 유틸리티: 공백 제거
            void trim(std::string& s);

            // 내부 유틸리티: 따옴표 및 이스케이프 처리
            std::string process_escape(std::string val);

            std::vector<config_line> m_lines; // 파일의 모든 줄 정보를 순서대로 저장
            std::mutex m_mutex;
        };

    } // namespace config
} // namespace j2
